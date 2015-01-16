#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>

#define LOG_DEBUG std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|DEBUG|"
#define LOG_ERROR std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|ERROR|"

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

   	if(::listen(iSockFd, 1) == -1)
   	{
   		LOG_ERROR << "listen error|msg:" << ::strerror(errno) << std::endl;

   		return (-1);
   	} 

   	return (iSockFd);
}

int main(int argc, char** argv)
{
	int iAcceptFd = CreateServer();
	if(iAcceptFd < 0)
	{
		return (0);
	}

    char szClientIP[16];
	char pReadBuffer[1024];
	while(1)
	{
		struct sockaddr_in stClientAddr;
		socklen_t uiClientAddrLen = sizeof(stClientAddr);
		int iClientFd = ::accept(iAcceptFd, (struct sockaddr*)&stClientAddr, &uiClientAddrLen);
		if(-1 == iClientFd)
		{
			LOG_ERROR << "accept client error|msg:" << ::strerror(errno) << std::endl;

			continue;
		}

        inet_ntop(AF_INET, &(stClientAddr.sin_addr), szClientIP, sizeof(szClientIP));
        LOG_DEBUG << "accept client:" << szClientIP << ":" << ntohs(stClientAddr.sin_port) << std::endl;

		ssize_t iRet = ::read(iClientFd, pReadBuffer, sizeof(pReadBuffer) - 1);
		if(-1 == iRet)
		{
			//read error
			LOG_ERROR << "read error|msg:" << ::strerror(errno) << std::endl;
		}
		else if(0 == iRet)
		{
			LOG_DEBUG << "client request to close connection" << std::endl;
		}
		else
		{
			//int* pIntBuffer = (int*)pReadBuffer;
			//int iLength = ntohl(*pIntBuffer);
			pReadBuffer[iRet] = '\0';
			LOG_DEBUG << "recv client message|msg_len:" << iRet << "|msg_content:"
					  << pReadBuffer << std::endl;

			::write(iClientFd, pReadBuffer, (size_t)iRet);
		}

		::close(iClientFd);
	}

	return (0);
}

