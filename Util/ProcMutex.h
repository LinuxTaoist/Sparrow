/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ProcMutex.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/02
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __PROC_MUTEX_H__
#define __PROC_MUTEX_H__

#include <mutex>
#include <string>
#include <pthread.h>
#include <atomic>

struct SharedData {
    std::atomic<int> initialized; // 0=uninitialized, 1=initializing, 2=ready
    std::atomic<int> refCnt;      // Number of processes currently holding the shared mutex
    std::atomic<int> waitCnt;     // Number of waiting lock attempts
    pthread_mutex_t dataMutex;    // Mutex for critical section protection
    pthread_mutex_t waitMutex;    // Mutex for wait count protection
};

class ProcMutex {
public:
    explicit ProcMutex(const std::string& mutexName);
    ~ProcMutex();

    void Lock();
    void Unlock();

private:
    void Init();
    void DeInit();
    void AddWait();
    void DelWait();

private:
    int mShmFd;
    std::string mMutexName;
    SharedData* mSharedData;
};

class ProcLockGuard {
public:
    explicit ProcLockGuard(ProcMutex& pMutex, std::mutex& tMutex);
    ~ProcLockGuard();

private:
    std::mutex& mTMutex;
    ProcMutex& mPMutex;
};

#endif // __PROC_MUTEX_H__
