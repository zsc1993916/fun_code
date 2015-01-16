#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <iostream>

using namespace std;

#define LOG_DEBUG std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|DEBUG|"
#define LOG_ERROR std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|ERROR|"

#define MAX_EVENTS 4096

typedef struct connection {
    int m_iFd;
    char m_szClientIP[16];
    unsigned short m_usClientPort;
}connection_info;

volatile int g_iIsQuit = 0;

struct epoll_event stEvent, events[MAX_EVENTS];
int iServerFd, iEpollFd;
char sReadBuffer[1024];

/*
 *  create a server and bind
 */
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

void ProcessClient(int fd) {
    ssize_t iRet = read(fd, sReadBuffer, sizeof(sReadBuffer) - 1);
    if(-1 == iRet) {
        perror("read error:");
    }
    else if (0 == iRet) {
        cout << "client request to close connection" << endl;
    }
    else {
        sReadBuffer[iRet] = '\0';
        cout << "recv client message|msg_len:" << iRet << "|msg_content:"
            << sReadBuffer << endl;
        write(fd, sReadBuffer, (size_t)iRet);
    }
}

void SetNonblocking(int sock) {
    int op = fcntl(sock, F_GETFL);
    if(op < 0) {
        perror("fcntl(sock, GETEL)");
        exit(1);
    }
    op = op | O_NONBLOCK;
    if(fcntl(sock, F_SETFL, op) < 0) {
        perror("fcntl(sock, SETFL, op)");
        exit(1);
    }
}

void EpollLoop() {
    int iEventCnt;
    while(0 == g_iIsQuit) {
        iEventCnt = epoll_wait(iEpollFd, events, MAX_EVENTS, -1);

        //cout << "iEventCnt:" << iEventCnt << endl;

        for(int index = 0; index < iEventCnt; index ++) {

            //cout << "iServerFd " << iServerFd << endl;
            //cout << "events[index].data.fd " << events[index].data.fd << endl;
            if(iServerFd == events[index].data.fd) {
                struct sockaddr_in stClientAddr;
                connection_info * pCon = NULL;
                socklen_t uiClientAddrLen = sizeof(stClientAddr);
                int iClientFd = accept(iServerFd, (struct sockaddr*)&stClientAddr, &uiClientAddrLen);

                pCon = new connection_info();
                pCon->m_iFd = iClientFd;
                inet_ntop(AF_INET, &(stClientAddr.sin_addr), pCon->m_szClientIP, sizeof(pCon->m_szClientIP));
                pCon->m_usClientPort = ntohs(stClientAddr.sin_port);

                if(-1 == iClientFd) {
                    perror("accept error:");
                    continue;
                }
                else {
                    /*connect test*/
                    cout << "accept client:" << pCon->m_szClientIP << ":" <<
                        pCon->m_usClientPort << endl;

                    SetNonblocking(iClientFd);
                    stEvent.data.fd = iClientFd;
                    stEvent.events = EPOLLIN | EPOLLET;
                    epoll_ctl(iEpollFd, EPOLL_CTL_ADD, stEvent.data.fd, &stEvent);
                }
            }
            else {
                cout << "read request!" << endl;
                if(events[index].events & EPOLLIN) {
                    ProcessClient(events[index].data.fd);
                }
            }
        }
    }
}

void ProcessSigINT(int iSig) {
    __sync_fetch_and_add(&g_iIsQuit, 1);
    cout << "In signal FUNCTION|pid:" << getpid() << "|signal:" << iSig << endl;
}

void InstallSigHandler() {
    struct sigaction stAction;
    stAction.sa_handler = ProcessSigINT;
    stAction.sa_flags = 0;
    sigemptyset(&(stAction.sa_mask));
    sigaction(SIGINT, &stAction, NULL);
}

int main(int argc, char** argv)
{
	iServerFd = CreateServer();
	if(iServerFd < 0)
	{
		return (0);
	}

    SetNonblocking(iServerFd);
    iEpollFd = epoll_create(65535);

    memset(&stEvent, 0, sizeof(stEvent));
    stEvent.data.fd = iServerFd;
    stEvent.events = EPOLLIN | EPOLLET;
    epoll_ctl(iEpollFd, EPOLL_CTL_ADD, iServerFd, &stEvent);

    EpollLoop();

	return (0);
}

