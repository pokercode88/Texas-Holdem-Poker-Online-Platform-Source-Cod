#include "TimerThread.h"
#include "Log2DBServantImp.h"

TimerThread::TimerThread()
{
    m_bShutdown = false;
    m_iInterval = 1;
    m_servant = NULL;
}

TimerThread::~TimerThread()
{
    terminate();
}

void TimerThread::terminate()
{
    if (!isAlive())
        return;

    m_bShutdown = true;
    LOG_DEBUG << "TimerThread terminated." << endl;
    // TC_ThreadLock::Lock sync(*this);
    // notifyAll();
}

void TimerThread::run()
{
    LOG_DEBUG << "TimerThread running" << endl;

    while (!m_bShutdown)
    {
        __TRY__

        sleep(m_iInterval);

        //if (m_servant)
        //{
        //  m_servant->ActiveTimer();
        //  m_servant->getHandle()->notifyFilter();
        //}

        __CATCH__
    }
}


