#include "ThreadLockManager.h"
#include "globe.h"

//
ThreadLockManager *ThreadLockManager::m_instance = NULL;

//
int ThreadLockManager::initialize(int iLockNumber)
{
    if (iLockNumber <= 1)
    {
        LOG_ERROR << "Lock size is less than 1, set it to be 1" << endl;
        m_iLockNumber = 1;
    }
    else
    {
        m_iLockNumber = iLockNumber;
    }

    //最少数量
    if (m_iLockNumber < 1)
    {
        m_iLockNumber = 1;
    }

    //初始化
    initializeLockPool();
    return 0;
}

int ThreadLockManager::initializeLockPool()
{
    //清理
    m_lockPool.clear();

    for(int i = 0; i < m_iLockNumber; i++)
    {
        TC_ThreadLock *newLock = new TC_ThreadLock();
        m_lockPool.push_back(newLock);
    }

    return 0;
}

ThreadLockManager *ThreadLockManager::ptr()
{
    if (!m_instance)
        m_instance = new ThreadLockManager();

    return m_instance;
}

void ThreadLockManager::getUserLock(unsigned long iUin, TC_ThreadLock *&userLock)
{
    if (m_lockPool.size() < 1)
    {
        LOG_ERROR << "lockPool's size is too low" << endl;

        m_lockPool.clear();
        if (m_iLockNumber < 1)
        {
            m_iLockNumber = 1;
        }

        for (int i = 0; i < m_iLockNumber; i ++)
        {
            TC_ThreadLock *newLock = new TC_ThreadLock();
            m_lockPool.push_back(newLock);
        }
    }

    unsigned lockSize = m_lockPool.size();
    userLock = m_lockPool[iUin % lockSize];
    return;
}


