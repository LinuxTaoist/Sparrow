/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ProcMutex.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/05/26
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/05/26 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __PROC_MUTEX_H__
#define __PROC_MUTEX_H__

#include <string>
#include <pthread.h>

class ProcMutex {
public:
    ProcMutex(const std::string& mutexName);
    ~ProcMutex();
    void Lock();
    void Unlock();

private:
    void Init();
    void Cleanup();

private:
    std::string mMutexName;
    pthread_mutex_t* mMutex;

};

class ProcLockGuard {
public:
    explicit ProcLockGuard(ProcMutex& mutex);
    ~ProcLockGuard();

private:
    ProcMutex& mMutex;
};

#endif // __PROC_MUTEX_H__
