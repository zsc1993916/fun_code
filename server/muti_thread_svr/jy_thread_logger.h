#ifndef __JY_THREAD_LOGGER_H__
#define __JY_THREAD_LOGGER_H__

#include <pthread.h>

#include <iostream>

class CJYStream
{
public:
    CJYStream(std::ostream* pStream, pthread_mutex_t* pMutex) 
    : m_pStream(pStream) , m_pMutex(pMutex)
    {
    }

    ~CJYStream()
    {
        if(m_pStream) 
        {
            m_pStream->flush();
            ::pthread_mutex_unlock(m_pMutex);
        }
    }

    template <typename P>
    CJYStream& operator << (const P &t)  { if(m_pStream) *m_pStream << t; return (*this); }

    typedef std::ostream& (*F)(std::ostream& os);
    CJYStream& operator << (F f)         { if(m_pStream) (f)(*m_pStream); return (*this); }

    typedef std::ios_base& (*I)(std::ios_base& os);
    CJYStream& operator << (I f)         { if(m_pStream) (f)(*m_pStream); return (*this); }

    //不实现
    CJYStream(const CJYStream& stStream);
    CJYStream& operator=(const CJYStream& stStream);

protected:
    std::ostream*           m_pStream;

    pthread_mutex_t*        m_pMutex;
};

class CJYThreadLogger
{
public:
    static CJYStream LogStream()
    {
        pthread_mutex_lock(&m_stMutex);
        CJYStream stStream(&std::cout, &m_stMutex);
        return (stStream);
    }

    static pthread_mutex_t m_stMutex;
};

pthread_mutex_t CJYThreadLogger::m_stMutex = PTHREAD_MUTEX_INITIALIZER; 

#endif
