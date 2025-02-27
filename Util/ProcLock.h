/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ProcLock.h
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
#ifndef __PROC_LOCK_H__
#define __PROC_LOCK_H__

#include <string>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

class ProcLock {
public:
    ProcLock(const std::string& lockFile);
    ProcLock(const ProcLock&) = delete;
    ProcLock& operator=(const ProcLock&) = delete;
    ProcLock(ProcLock&&) = delete;
    ProcLock& operator=(ProcLock&&) = delete;
    ProcLock(const std::string& lockFile, int32_t val = 1);
    ~ProcLock();
    bool Lock();
    bool Unlock();
    bool IsLock() const;
    bool SetValue(int value);

private:
    std::string mLockFile;
    sem_t* mpSem;
};

#endif // __PROC_LOCK_H__
