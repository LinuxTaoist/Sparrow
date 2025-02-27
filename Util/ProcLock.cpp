/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ProcLock.cpp
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
#include "ProcLock.h"

#define SPR_LOG(fmt, args...)   printf(fmt, ##args)
#define SPR_LOGD(fmt, args...)  printf("%4d ProcLock D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...)  printf("%4d ProcLock W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...)  printf("%4d ProcLock E: " fmt, __LINE__, ##args)

// Open the semaphore (do not create if it does not exist),
// used by slave process
ProcLock::ProcLock(const std::string& lockFile) : mLockFile(lockFile), mpSem(nullptr)
{
    mpSem = sem_open(mLockFile.c_str(), O_EXCL);
    if (mpSem == SEM_FAILED) {
        perror("sem_open");
    }
    if (!Lock()) {
        SPR_LOGE("Failed lock!\n");
    }
}

// Open the semaphore (create if it does not exist),
// used by master process
ProcLock::ProcLock(const std::string& lockFile, int32_t val) : mLockFile(lockFile), mpSem(nullptr)
{
    mpSem = sem_open(mLockFile.c_str(), O_CREAT, 0644, val);
    if (mpSem == SEM_FAILED) {
        perror("sem_open");
    }
    if (!Lock()) {
        SPR_LOGE("Failed lock!\n");
    }
}

ProcLock::~ProcLock()
{
    if (mpSem != nullptr) {
        Unlock();
        sem_close(mpSem);
        sem_unlink(mLockFile.c_str());
    }
}

bool ProcLock::Lock()
{
    return sem_wait(mpSem) == 0;
}

bool ProcLock::Unlock()
{
    return sem_post(mpSem) == 0;
}

bool ProcLock::IsLock() const
{
    int val;
    if (sem_getvalue(mpSem, &val) == -1) {
        perror("sem_getvalue");
        return false;
    }
    return val == 0;
}

bool ProcLock::SetValue(int value)
{
    if (value < 0) {
        SPR_LOGE("Invalid value: %d\n", value);
        return false;
    }

    int curValue;
    if (sem_getvalue(mpSem, &curValue) == -1) {
        perror("sem_getvalue");
        return false;
    }

    while (curValue > value) {
        if (sem_trywait(mpSem) != 0) break;
        --curValue;
    }

    while (curValue < value) {
        if (sem_post(mpSem) != 0) {
            perror("sem_post");
            return false;
        }
        ++curValue;
    }

    return true;
}
