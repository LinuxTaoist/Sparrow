/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ProcMutex.cpp
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
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <cerrno>
#include "ProcMutex.h"

#define SPR_LOGD(fmt, args...)  printf("%4d ProcMutex D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...)  printf("%4d ProcMutex W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...)  printf("%4d ProcMutex E: " fmt, __LINE__, ##args)

ProcMutex::ProcMutex(const std::string& mutexName)
    : mShmFd(-1), mMutexName(std::string("/") + mutexName), mSharedData(nullptr) {
    Init();
}

ProcMutex::~ProcMutex() {
    if (mSharedData) {
        // Destroy resources only when both reference count and wait count are 0
        if (mSharedData->refCnt == 0 && mSharedData->waitCnt == 0) {
            DeInit();
        } else {
            munmap(mSharedData, sizeof(SharedData));
            close(mShmFd);
            mSharedData = nullptr;
            mShmFd = -1;
        }
    }
}

void ProcMutex::Init() {
    mShmFd = shm_open(mMutexName.c_str(), O_RDWR | O_CREAT, 0744);
    if (mShmFd == -1) {
        SPR_LOGE("shm_open failed! (%s)\n", strerror(errno));
        return;
    }
    if (ftruncate(mShmFd, sizeof(SharedData)) == -1) {
        SPR_LOGE("ftruncate failed! (%s)\n", strerror(errno));
        close(mShmFd);
        mShmFd = -1;
        return;
    }

    mSharedData = reinterpret_cast<SharedData*>(mmap(NULL, sizeof(SharedData),
                                        PROT_READ | PROT_WRITE, MAP_SHARED, mShmFd, 0));
    if (mSharedData == MAP_FAILED) {
        SPR_LOGE("mmap failed! (%s)\n", strerror(errno));
        close(mShmFd);
        mShmFd = -1;
        return;
    }

    pthread_mutexattr_t mutexAttr;
    int rc = pthread_mutexattr_init(&mutexAttr);
    if (rc != 0) {
        SPR_LOGE("init mutexattr failed! (%s)\n", strerror(rc));
        munmap(mSharedData, sizeof(SharedData));
        close(mShmFd);
        mShmFd = -1;
        mSharedData = nullptr;
        return;
    }

    // Set mutex to be process - shared
    rc = pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
    if (rc != 0) {
        SPR_LOGE("setpshared mutexattr failed! (%s)\n", strerror(rc));
        pthread_mutexattr_destroy(&mutexAttr);
        munmap(mSharedData, sizeof(SharedData));
        close(mShmFd);
        mShmFd = -1;
        mSharedData = nullptr;
        return;
    }

    // Set mutex to be robust
    rc = pthread_mutexattr_setrobust(&mutexAttr, PTHREAD_MUTEX_ROBUST);
    if (rc != 0) {
        SPR_LOGE("setrobust mutexattr failed! (%s)\n", strerror(rc));
        pthread_mutexattr_destroy(&mutexAttr);
        munmap(mSharedData, sizeof(SharedData));
        close(mShmFd);
        mShmFd = -1;
        mSharedData = nullptr;
        return;
    }

    // Check if mutexes, reference count and wait count need initialization
    if (mSharedData->refCnt == 0) {
        rc = pthread_mutex_init(&mSharedData->dataMutex, &mutexAttr);
        if (rc != 0) {
            SPR_LOGE("init data mutex failed! (%s)\n", strerror(rc));
            pthread_mutexattr_destroy(&mutexAttr);
            munmap(mSharedData, sizeof(SharedData));
            close(mShmFd);
            mShmFd = -1;
            mSharedData = nullptr;
            return;
        }

        rc = pthread_mutex_init(&mSharedData->waitMutex, &mutexAttr);
        if (rc != 0) {
            SPR_LOGE("init wait mutex failed! (%s)\n", strerror(rc));
            pthread_mutex_destroy(&mSharedData->dataMutex);
            pthread_mutexattr_destroy(&mutexAttr);
            munmap(mSharedData, sizeof(SharedData));
            close(mShmFd);
            mShmFd = -1;
            mSharedData = nullptr;
            return;
        }

        mSharedData->refCnt = 0;
        mSharedData->waitCnt = 0;
    }

    pthread_mutexattr_destroy(&mutexAttr);
}

void ProcMutex::DeInit() {
    if (!mSharedData) {
        return;
    }

    int rc = pthread_mutex_destroy(&mSharedData->dataMutex);
    if (rc != 0) {
        SPR_LOGE("destory data mutex failed! (%s)\n", strerror(rc));
    }

    rc = pthread_mutex_destroy(&mSharedData->waitMutex);
    if (rc != 0) {
        SPR_LOGE("destroy wait mutex failed! (%s)\n", strerror(rc));
    }

    munmap(mSharedData, sizeof(SharedData));
    if (mShmFd != -1) {
        if (close(mShmFd) == -1) {
            SPR_LOGE("close failed! (%s)\n", strerror(errno));
        }
        mShmFd = -1;
    }

    mSharedData = nullptr;
    shm_unlink(mMutexName.c_str());
}

void ProcMutex::Lock() {
    if (!mSharedData) {
        return;
    }

    // Try to acquire the lock non - blocking
    int rc = pthread_mutex_trylock(&mSharedData->dataMutex);
    if (rc != 0) {
        AddWait();
        rc = pthread_mutex_lock(&mSharedData->dataMutex);
        if (rc != 0) {
            SPR_LOGE("pthread_mutex_lock failed: %s\n", strerror(rc));
        }
        DelWait();
    }

    // Increment reference count after successful lock
    mSharedData->refCnt++;
}

void ProcMutex::Unlock() {
    if (!mSharedData) {
        return;
    }

    // Decrement reference count after unlock
    mSharedData->refCnt--;
    int rc = pthread_mutex_unlock(&mSharedData->dataMutex);
    if (rc != 0) {
        SPR_LOGE("pthread_mutex_unlock failed: %s\n", strerror(rc));
    }
}

void ProcMutex::AddWait() {
    if (!mSharedData) {
        return;
    }

    int rc = pthread_mutex_lock(&mSharedData->waitMutex);
    if (rc != 0) {
        SPR_LOGE("pthread_mutex_lock (waitMutex) failed: %s\n", strerror(rc));
        return;
    }
    mSharedData->waitCnt++;
    rc = pthread_mutex_unlock(&mSharedData->waitMutex);
    if (rc != 0) {
        SPR_LOGE("pthread_mutex_unlock (waitMutex) failed: %s\n", strerror(rc));
    }
}

void ProcMutex::DelWait() {
    if (!mSharedData) {
        return;
    }

    int rc = pthread_mutex_lock(&mSharedData->waitMutex);
    if (rc != 0) {
        SPR_LOGE("pthread_mutex_lock (waitMutex) failed: %s\n", strerror(rc));
        return;
    }
    mSharedData->waitCnt--;
    rc = pthread_mutex_unlock(&mSharedData->waitMutex);
    if (rc != 0) {
        SPR_LOGE("pthread_mutex_unlock (waitMutex) failed: %s\n", strerror(rc));
    }
}

ProcLockGuard::ProcLockGuard(ProcMutex& pMutex, std::mutex& tMutex)
    : mTMutex(tMutex), mPMutex(pMutex) {
    mTMutex.lock();
    mPMutex.Lock();
}

ProcLockGuard::~ProcLockGuard() {
    mPMutex.Unlock();
    mTMutex.unlock();
}
