/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ProcMutex.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/02
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/03/02 | 1.0.0.1   | Xiang.D        | Create file
 *  2025/08/17 | 1.0.0.2   | Xiang.D        | Use atomic variables for thread safety
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __PROC_MUTEX_H__
#define __PROC_MUTEX_H__

#include <mutex>
#include <string>
#include <pthread.h>
#include <atomic>

struct SharedData {
    std::atomic<int> initialized; // 0=未初始化, 1=已初始化 (CAS 保证仅 init 一次)
    std::atomic<int> refCnt;
    std::atomic<int> waitCnt;
    pthread_mutex_t dataMutex;  // Mutex for critical section protection
    pthread_mutex_t waitMutex;  // Mutex for protecting the wait count
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
