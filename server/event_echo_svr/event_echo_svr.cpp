#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include <map>
#include <iostream>

#define LOG_DEBUG std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|DEBUG|"
#define LOG_ERROR std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|ERROR|"

typedef struct connection_desc_s
{
    int                 m_iFd;
    char                m_szClientIP[16];
    unsigned short      m_usClientPort;
}connection_desc_t;

int CreateServer()
{
	int iSockFd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == iSockFd)
	{
		LOG_ERROR << "socket error|msg:" << ::strerror(errno) << std::endl;
		return (-1);
	}

    int iFlag = 1;
    ::setsockopt(iSockFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&iFlag, sizeof(iFlag));

	struct sockaddr_in stBindAddr;
   	stBindAddr.sin_family = AF_INET;
   	stBindAddr.sin_port = htons(8888);
   	stBindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   	if(::bind(iSockFd, (struct sockaddr*)&stBindAddr, sizeof(stBindAddr)) == -1)
   	{
   		LOG_ERROR << "bind address error|msg:" << ::strerror(errno) << std::endl;

   		return (-1);
   	}

   	if(::listen(iSockFd, 1024) == -1)
   	{
   		LOG_ERROR << "listen error|msg:" << ::strerror(errno) << std::endl;

   		return (-1);
   	}

   	return (iSockFd);
}

connection_desc_t Accept(int iAcceptFd)
{
    connection_desc_t stConn;

    struct sockaddr_in stClientAddr;
    socklen_t uiClientAddrLen = sizeof(stClientAddr);
    int iClientFd = ::accept(iAcceptFd, (struct sockaddr*)&stClientAddr, &uiClientAddrLen);
    if(-1 == iClientFd)
    {
        LOG_ERROR << "accept client error|msg:" << ::strerror(errno) << std::endl;
        stConn.m_iFd = -1;
    }
    else
    {
        inet_ntop(AF_INET, &(stClientAddr.sin_addr), stConn.m_szClientIP, sizeof(stConn.m_szClientIP));
        stConn.m_iFd = iClientFd;
        stConn.m_usClientPort = ntohs(stClientAddr.sin_port);
        LOG_DEBUG << "accept client:" << stConn.m_szClientIP << ":" << stConn.m_usClientPort << std::endl;
    }

    return (stConn);
}

int ProcessClient(const connection_desc_t& stConn)
{
    static char szBuffer[1024];

    ssize_t iRet = ::read(stConn.m_iFd, szBuffer, sizeof(szBuffer) - 1);
    if(-1 == iRet)
    {
        //read error
        LOG_ERROR << "read error|client:" << stConn.m_szClientIP << ":" << stConn.m_usClientPort 
                  << "|msg:" << ::strerror(errno) << std::endl;

        ::close(stConn.m_iFd);
        return (-1);
    }
    else if(0 == iRet)
    {
        LOG_DEBUG << "client request to close connection|client:" << stConn.m_szClientIP << ":" << stConn.m_usClientPort << std::endl;
        ::close(stConn.m_iFd);
        return (-1);
    }
    else
    {
        szBuffer[iRet] = '\0';
        LOG_DEBUG << "recv client message|client:" << stConn.m_szClientIP << ":" << stConn.m_usClientPort << "|msg_len:" 
                  << iRet << "|msg_content:" << szBuffer << std::endl;

        ::write(stConn.m_iFd, szBuffer, (size_t)iRet);
    }

    return (0);
}

int main(int argc, char** argv)
{
	int iAcceptFd = CreateServer();
	if(iAcceptFd < 0)
	{
		return (0);
	}

    std::map<int, connection_desc_t> stConnContainer;

    fd_set stReadFdSet;
    FD_ZERO(&stReadFdSet);
    FD_SET(iAcceptFd, &stReadFdSet);
    int iMaxFd = iAcceptFd + 1;

    struct timeval stTimeout;
	while(1)
	{
        stTimeout.tv_sec = 0;
        stTimeout.tv_usec = 100000;
        int iEventCount = ::select(iMaxFd, &stReadFdSet, NULL, NULL, &stTimeout); 
		if(iEventCount > 0)
		{
            fd_set stTmpReadFdSet;
            FD_ZERO(&stTmpReadFdSet);
            iMaxFd = iAcceptFd + 1;

            std::map<int, connection_desc_t>::iterator client_it;
            for(client_it = stConnContainer.begin(); client_it != stConnContainer.end(); )
            {
                if(FD_ISSET(client_it->first, &stReadFdSet) != 0)
                {
                    if(ProcessClient(client_it->second) != 0)
                    {
                        stConnContainer.erase(client_it ++);
                        continue;
                    }                   
                }                
                
                FD_SET(client_it->first, &stTmpReadFdSet);
                iMaxFd = client_it->first >= iMaxFd ? client_it->first + 1 : iMaxFd;

                client_it ++; 
            }

            if(FD_ISSET(iAcceptFd, &stReadFdSet) != 0)
            {
                connection_desc_t stConn = Accept(iAcceptFd);
                if(stConn.m_iFd >= 0)
                {
                    stConnContainer.insert(std::pair<int, connection_desc_t>(stConn.m_iFd, stConn));
                    FD_SET(stConn.m_iFd, &stTmpReadFdSet);
                    iMaxFd = stConn.m_iFd >= iMaxFd ? stConn.m_iFd + 1 : iMaxFd;
                }
            }

            FD_SET(iAcceptFd, &stTmpReadFdSet);

            stReadFdSet = stTmpReadFdSet;
        }
        else
        {
            FD_ZERO(&stReadFdSet);
            std::map<int, connection_desc_t>::iterator client_it;
            for(client_it = stConnContainer.begin(); client_it != stConnContainer.end(); client_it ++)
            {                                                
                FD_SET(client_it->first, &stReadFdSet);
            }

            FD_SET(iAcceptFd, &stReadFdSet);
        }
	}

	return (0);
}

