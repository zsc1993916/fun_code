#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include <set>
#include <iostream>

#define LOG_DEBUG std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|DEBUG|"
#define LOG_ERROR std::cout << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|ERROR|"

volatile int g_iIsQuit = 0;
std::set<pid_t> g_pChildList;

typedef struct connection_desc_s
{
    int                 m_iFd;
    char                m_szClientIP[16];
    unsigned short      m_usClientPort;
}connection_desc_t;

void ProcessSigChild(int iSigNo) 
{
	pid_t iPid;
	int iStat;
	
	while((iPid = ::waitpid(-1, &iStat, WNOHANG)) > 0) 
    {
        g_pChildList.erase(iPid);
        LOG_DEBUG << "child terminated|pid:" << iPid << std::endl;
	}
}

void ProcessSigINT(int iSigNo)
{
	//得到当前进程的id和信号的编号
    g_iIsQuit = 1;
    LOG_DEBUG << "in signal FUNCTION|pid:" << ::getpid() << "|signal:" << iSigNo << std::endl;
}

int CreateServer()
{
    //创建socket
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
    //bind 将socket和端口号绑定
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
    stConn.m_iFd = -1;

    struct sockaddr_in stClientAddr;
    socklen_t uiClientAddrLen = sizeof(stClientAddr);
    int iClientFd = ::accept(iAcceptFd, (struct sockaddr*)&stClientAddr, &uiClientAddrLen);
    if(iClientFd < 0)
    {
        LOG_ERROR << "accept client error|msg:" << ::strerror(errno) << std::endl;
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

    fd_set stReadFdSet;
    int iMaxFd = stConn.m_iFd + 1;

    struct timeval stTimeout;
    while(0 == g_iIsQuit)
    {
    	//io复用
        stTimeout.tv_sec = 0;
        stTimeout.tv_usec = 100000;
        FD_ZERO(&stReadFdSet);
        FD_SET(stConn.m_iFd, &stReadFdSet);
        //判断当前有没有事件触发
        int iEventCount = ::select(iMaxFd, &stReadFdSet, NULL, NULL, &stTimeout);
        if(iEventCount > 0)
        {
        	//
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
        }
    }

    return (0);
}
//绑定对应信号所处理的函数
void InstallSigINTHandler()
{
	//sig结构体是用来处理信号的结构体
	//siga。。有两个函数之指针
    struct sigaction stAction;
    stAction.sa_handler = ProcessSigINT;//信号处理函数(是一个函数指针)
    stAction.sa_flags = SA_RESTART;//通过设置sa_flags来确定掉用saA的那个函数指针
    sigemptyset(&(stAction.sa_mask));//清空sa_mask的信号集;sa_mask集的信号集搁置，调用当前的信号
    //表示当我按下键盘时,会调用这个函数
    ::sigaction(SIGINT, &stAction, NULL);//调用当前的sig..函数 sigaction(int signum,const struct sigaction *act ,struct sigaction *oldact)
    									//通过sigint来设置信号的处理函数,并保留原来的处理函数
}

void InstallSigUsr1Handler()
{
    struct sigaction stAction;
    stAction.sa_handler = ProcessSigINT;
    stAction.sa_flags = SA_RESTART;
    sigemptyset(&(stAction.sa_mask));
    ::sigaction(SIGUSR1, &stAction, NULL);
}

int main(int argc, char** argv)
{
	//
    InstallSigINTHandler();
    //进程termin或stop时会调用Process..
    ::signal(SIGCHLD, ProcessSigChild);
    //创建服务器
	int iAcceptFd = CreateServer();
	if(iAcceptFd < 0)
	{
		return (0);
	}

    while(0 == g_iIsQuit)
    {

        connection_desc_t stConn = Accept(iAcceptFd);
        if(stConn.m_iFd >= 0)
        {
            pid_t iPid = ::fork();
            if(iPid > 0)
            {
                //in parent process
                ::close(stConn.m_iFd);
                g_pChildList.insert(iPid);
            }
            else if(0 == iPid)
            {
                //in child process
                ::close(iAcceptFd);
                //对SIGCHLD,和SIGINT的信号种类忽略
                ::signal(SIGCHLD, SIG_IGN);
                ::signal(SIGINT, SIG_IGN);
                InstallSigUsr1Handler();
                ProcessClient(stConn);
                ::_exit(0);
            }
            else
            {
                //fork error
                ::close(stConn.m_iFd);
                LOG_ERROR << "fork error|msg:" << ::strerror(errno) << std::endl; 
            }
        }
    }
    //将SIGCHLD信号种类设置为默认
    //子进程的回收利用
	::signal(SIGCHLD, SIG_DFL);
    for(std::set<pid_t>::const_iterator it = g_pChildList.begin(); it != g_pChildList.end(); it ++)
    {
        if(::kill(*it, SIGUSR1) != 0)
        {
            LOG_ERROR << "kill error|msg:" << ::strerror(errno) << std::endl;
            continue;
        }

        int iStatus;
        int iRet = ::waitpid(*it, &iStatus, 0);
        if(0 == iRet)
        {
            LOG_ERROR << "child don't exit|pid:" << *it << std::endl;
        }
        else if(-1 == iRet)
        {
            LOG_ERROR << "waitpid error|msg:" << ::strerror(errno) << std::endl;
        }
        else
        {
            LOG_DEBUG << "child process exit|pid:" << *it << std::endl;
        }
    }

    LOG_DEBUG << "server exit now!!!" << std::endl;

	return (0);
}
