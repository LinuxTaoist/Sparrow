/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ProcMutex.cpp
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
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "ProcMutex.h"

#define SPR_LOG(fmt, args...)   printf(fmt, ##args)
#define SPR_LOGD(fmt, args...)  printf("%4d ProcMutex D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...)  printf("%4d ProcMutex W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...)  printf("%4d ProcMutex E: " fmt, __LINE__, ##args)

ProcMutex::ProcMutex(const std::string& mutexName) : mMutexName(mutexName), mMutex(nullptr) {
    Init();
}

ProcMutex::~ProcMutex() {
    Cleanup();
}

void ProcMutex::Lock() {
    pthread_mutex_lock(mMutex);
}

void ProcMutex::Unlock() {
    pthread_mutex_unlock(mMutex);
}

void ProcMutex::Init() {
    int shmId = shm_open(mMutexName.c_str(), O_RDWR | O_CREAT, 0744);
    if (shmId == -1) {
        perror("shm_open");
        return;
    }

    int rc = ftruncate(shmId, sizeof(pthread_mutex_t));
    if (rc == -1) {
        perror("ftruncate");
        close(shmId);
        return;
    }

    mMutex = (pthread_mutex_t *)mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, shmId, 0);

    pthread_mutexattr_t mutexAttr;
    pthread_mutexattr_init(&mutexAttr);
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);

    if (((pthread_mutex_t*)mMutex)->__data.__kind == 0) {
        pthread_mutex_init(mMutex, &mutexAttr);
    }

    pthread_mutexattr_destroy(&mutexAttr);
}

void ProcMutex::Cleanup() {
}

// ----------------------------------------------------------------------------------------------------------------------
// ProcMutex RAII Guard
// ----------------------------------------------------------------------------------------------------------------------
ProcLockGuard::ProcLockGuard(ProcMutex& mutex) : mMutex(mutex) {
    mMutex.Lock();
}

ProcLockGuard::~ProcLockGuard() {
    mMutex.Unlock();
}
