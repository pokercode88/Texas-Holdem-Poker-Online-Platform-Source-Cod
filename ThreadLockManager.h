#ifndef __THREAD_LOCK_MANAGER_H__
#define __THREAD_LOCK_MANAGER_H__

#include <vector>
#include "util/tc_monitor.h"

//
using namespace tars;

/**
 *
*/
class ThreadLockManager
{
public:
    /**
     *
    */
    virtual ~ThreadLockManager() {};

public:
    //
    int initialize(int iLockNumber);
    //
    int initializeLockPool();

public:
    //
    static ThreadLockManager *ptr();
    //
    void getUserLock(unsigned long iUin, TC_ThreadLock *&userLock);

private:
    //
    ThreadLockManager() {};

private:
    //
    static ThreadLockManager *m_instance;

private:
    //
    int m_iLockNumber;
    //
    vector<TC_ThreadLock *> m_lockPool;
};

#endif



