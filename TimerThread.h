#ifndef __TIMER_THREAD_H_
#define __TIMER_THREAD_H_

#include <unistd.h>

#include "util/tc_monitor.h"
#include "util/tc_thread.h"

//
#include "globe.h"

//
using namespace tars;
using namespace std;

//
class Log2DBServantImp;

/**
 *
*/
class TimerThread : public TC_Thread, public TC_ThreadLock
{
public:
    /**
     *
    */
    TimerThread();

    /**
     *
    */
    ~TimerThread();


public:
    //
    void initialize(tars::Servant *p)
    {
        m_servant = (Log2DBServantImp *)p;
    }

public:
    //
    void terminate();

protected:
    //
    virtual void run();

protected:
    //
    Log2DBServantImp *m_servant;

private:
    //定时器心跳间隔时间
    int m_iInterval;
    //
    bool m_bShutdown;
};

#endif


