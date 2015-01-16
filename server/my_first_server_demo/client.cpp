#include <errno.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#define LOG_DEBUG std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|DEBUG|"
#define LOG_ERROR std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|ERROR|"

using namespace std;

int Connect()
{
    int iSockFd = ::socket(AF_INET, SOCK_STREAM, 0);
  	if(-1 == iSockFd)
	{ 
    	LOG_ERROR << "socket error|msg:" << ::strerror(errno) << std::endl;
       	return (-1);
   	}

	struct sockaddr_in stSvrAddr;
	stSvrAddr.sin_family = AF_INET;
	stSvrAddr.sin_port = htons(8888);
	stSvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(::connect(iSockFd, (struct sockaddr*)&stSvrAddr, sizeof(stSvrAddr)) == -1)
	{
		LOG_ERROR << "connect to server error|msg:" << ::strerror(errno) << std::endl;

		return (-1);
	}

	return (iSockFd);
}

int main(int argc, char** argv)
{
    int iSockFd = Connect();
    if(iSockFd < 0)
    {
        return (0);
    }

    char szBuffer[1024];
    string strMsg;

    while(1) {
        cout << "please enter message!" << std::endl;
        getline(cin, strMsg);
        if(strMsg.empty() == false) {
            
            ::write(iSockFd, strMsg.c_str(), strMsg.size());
            ssize_t iReadLen = ::read(iSockFd, szBuffer, sizeof(szBuffer) - 1);
            if(iReadLen < 0) {
                LOG_ERROR << "read from server error|msg:" << ::strerror(errno) << std::endl;
                return (0);
            }
            else if(0 == iReadLen) {
                LOG_ERROR << "read no message from server" << std::endl;
            }
            else {
                szBuffer[iReadLen] = '\0';
                LOG_DEBUG << "read message from server|msg:" << szBuffer << std::endl;
            }
        }    
        else
        {
            LOG_ERROR << "message is empty!!!" << std::endl;
        }
    }


    ::close(iSockFd);

    return (0);
}
