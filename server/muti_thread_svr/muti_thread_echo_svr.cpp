#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include "jy_thread_logger.h"

#define LOG_DEBUG CJYThreadLogger::LogStream() << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|DEBUG|"
#define LOG_ERROR CJYThreadLogger::LogStream() << __FILE__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|ERROR|"

volatile int g_iIsQuit = 0;


// 客户端信息
typedef struct connection_desc_s
{
    int                 m_iFd;                  // socket号
    char                m_szClientIP[16];       // ip
    unsigned short      m_usClientPort;         // 端口
}connection_desc_t;

void ProcessSigINT(int iSigNo)
{
    __sync_fetch_and_add(&g_iIsQuit, 1);        //相当与 g_iIsQuit ++
    LOG_DEBUG << "in signal FUNCTION|pid:" << ::getpid() << "|signal:" << iSigNo << std::endl;
}

// 创建服务器socket，绑定ip端口，并监听
int CreateServer()
{
    //创建socket
	int iSockFd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == iSockFd)
	{
		LOG_ERROR << "socket error|msg:" << ::strerror(errno) << std::endl;
		return (-1);
	}

    //socket配置函数，   不懂。。。留坑
    int iFlag = 1;
    ::setsockopt(iSockFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&iFlag, sizeof(iFlag));

    //绑定ip和端口
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

//监听到一个客户端， 返回一个connection_desc_t
connection_desc_t* Accept(int iAcceptFd)
{
    connection_desc_t* pConn = NULL;

    struct sockaddr_in stClientAddr;
    socklen_t uiClientAddrLen = sizeof(stClientAddr);
    int iClientFd = ::accept(iAcceptFd, (struct sockaddr*)&stClientAddr, &uiClientAddrLen);
    if(iClientFd < 0)
    {
        LOG_ERROR << "accept client error|msg:" << ::strerror(errno) << std::endl;
    }
    else
    {
        pConn = new connection_desc_t();
        /*
           inet_ntop() 转化ip表示方法，整数转化为点分十进制
           inet_pton() 反转化
        */
        inet_ntop(AF_INET, &(stClientAddr.sin_addr), pConn->m_szClientIP, sizeof(pConn->m_szClientIP));
        pConn->m_iFd = iClientFd;
        pConn->m_usClientPort = ntohs(stClientAddr.sin_port);
        LOG_DEBUG << "accept client:" << pConn->m_szClientIP << ":" << pConn->m_usClientPort << std::endl;
    }

    return (pConn);
}

void* ProcessClient(void* pConn)
{
    /*
       子进程调用pthread_detach(::pthread_self());
       或者 父进程调用 pthread_detach(thread_id);
       将该子线程状态设置成detached，则该线程运行结束后自动释放所有资源
       原因：不希望子进程因调用 pthread_join 而阻塞
    */
    ::pthread_detach(::pthread_self());
    connection_desc_t stConn = *((connection_desc_t*)pConn);

    delete (connection_desc_t*)pConn;

    char szBuffer[1024];

    //fd_set 一个集合，存文件描述符
    fd_set stReadFdSet;
    int iMaxFd = stConn.m_iFd + 1;

    //timeval 两个成员 秒和毫秒
    struct timeval stTimeout;
    while(0 == __sync_fetch_and_add(&g_iIsQuit, 0))
    {
        stTimeout.tv_sec = 0;
        stTimeout.tv_usec = 100000;

        /*
           fd_set 中的宏
           FD_ZERO 清空set中的fd
           FD_SET(fd, &set) 将fd加入set集合
           FD_CLR(fd, &set) 将fd从set中清除
        */
        FD_ZERO(&stReadFdSet);
        FD_SET(stConn.m_iFd, &stReadFdSet);
        /*
            int select(int maxfdp, fd_set* readfds, fd_set* writefds,
                            fs_set* errorfds, struct timeval* timeout)
            无限期阻塞，测试文件描述符变动
            
            maxfdp: 所有文件描述符最大值+1
            readfds: 被监视的集合, 如果有一个可读的文件会返回一个大于0的值，
            如果没有可读文件根据timeout判断超时，超时返回0，错误返回负值
            writefds: 类似与readfds， 监视是否可写
            errorfds: 类似，监视错误
        */
        int iEventCount = ::select(iMaxFd, &stReadFdSet, NULL, NULL, &stTimeout);
        if(iEventCount > 0)
        {
            ssize_t iRet = ::read(stConn.m_iFd, szBuffer, sizeof(szBuffer) - 1);
            if(-1 == iRet)
            {
                //read error
                LOG_ERROR << "read error|client:" << stConn.m_szClientIP << ":" << stConn.m_usClientPort 
                          << "|msg:" << ::strerror(errno) << std::endl;

                ::close(stConn.m_iFd);
                break;
            }
            else if(0 == iRet)
            {
                LOG_DEBUG << "client request to close connection|client:" << stConn.m_szClientIP << ":" << stConn.m_usClientPort << std::endl;
                ::close(stConn.m_iFd);
                break;
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

    LOG_DEBUG << "thread exit now|tid:" << pthread_self() << std::endl;

    return (NULL);
}

void InstallSigINTHandler()
{
    struct sigaction stAction;
    stAction.sa_handler = ProcessSigINT;   //函数指针
    stAction.sa_flags = 0;
    //stAction.sa_mask  信号集
    /* 类型：sigset_t 
       源码：
       typedef struct {
            unsigned long sig[];
       } sigset_t;
    */
    sigemptyset(&(stAction.sa_mask));     //初始化信号集
    //信号安装
    ::sigaction(SIGINT, &stAction, NULL);
}

int main(int argc, char** argv)
{
    InstallSigINTHandler();
    
	int iAcceptFd = CreateServer();
	if(iAcceptFd < 0)
	{
		return (0);
	}

    while(0 == __sync_fetch_and_add(&g_iIsQuit, 0))
    {
        connection_desc_t* pConn = Accept(iAcceptFd);
        if(pConn != NULL)
        {
            pthread_t iTid;
            //创建新的进程
            ::pthread_create(&iTid, NULL, ProcessClient, pConn);
            LOG_DEBUG << "create now thread to process client|tid:" << iTid << std::endl;
        }
    }

    LOG_DEBUG << "server exit now!!!" << std::endl;
    ::pthread_exit(NULL);

	return (0);
}

