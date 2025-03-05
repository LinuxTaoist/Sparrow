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
#include "ProcMutex.h"

ProcMutex::ProcMutex(const std::string& mutexName)
    : mShmId(-1), mMutexName(std::string("/") + mutexName), mSharedData(nullptr) {
    Init();
}

ProcMutex::~ProcMutex() {
    if (mSharedData) {
        // Destroy resources only when both reference count and wait count are 0
        if (mSharedData->refCnt == 0 && mSharedData->waitCnt == 0) {
            DeInit();
        } else {
            munmap(mSharedData, sizeof(SharedData));
            close(mShmId);
        }
    }
}

void ProcMutex::Init() {
    mShmId = shm_open(mMutexName.c_str(), O_RDWR | O_CREAT, 0744);
    if (mShmId == -1) {
        perror("shm_open");
        return;
    }
    if (ftruncate(mShmId, sizeof(SharedData)) == -1) {
        perror("ftruncate");
        close(mShmId);
        mShmId = -1;
        return;
    }

    mSharedData = (SharedData*)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, mShmId, 0);
    if (mSharedData == MAP_FAILED) {
        perror("mmap");
        close(mShmId);
        mShmId = -1;
        return;
    }

    pthread_mutexattr_t mutexAttr;
    if (pthread_mutexattr_init(&mutexAttr) != 0) {
        perror("pthread_mutexattr_init");
        munmap(mSharedData, sizeof(SharedData));
        close(mShmId);
        mShmId = -1;
        mSharedData = nullptr;
        return;
    }

    // Set mutex to be process - shared
    if (pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED) != 0) {
        perror("pthread_mutexattr_setpshared");
        pthread_mutexattr_destroy(&mutexAttr);
        munmap(mSharedData, sizeof(SharedData));
        close(mShmId);
        mShmId = -1;
        mSharedData = nullptr;
        return;
    }

    // Set mutex to be robust
    if (pthread_mutexattr_setrobust(&mutexAttr, PTHREAD_MUTEX_ROBUST) != 0) {
        perror("pthread_mutexattr_setrobust");
        return;
    }

    // Check if mutexes, reference count and wait count need initialization
    if (mSharedData->refCnt == 0) {
        if (pthread_mutex_init(&mSharedData->dataMutex, &mutexAttr) != 0) {
            perror("pthread_mutex_init");
            pthread_mutexattr_destroy(&mutexAttr);
            munmap(mSharedData, sizeof(SharedData));
            close(mShmId);
            mShmId = -1;
            mSharedData = nullptr;
            return;
        }

        if (pthread_mutex_init(&mSharedData->waitMutex, &mutexAttr) != 0) {
            perror("pthread_mutex_init (waitMutex)");
            pthread_mutex_destroy(&mSharedData->dataMutex);
            pthread_mutexattr_destroy(&mutexAttr);
            munmap(mSharedData, sizeof(SharedData));
            close(mShmId);
            mShmId = -1;
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

    pthread_mutex_destroy(&mSharedData->dataMutex);
    pthread_mutex_destroy(&mSharedData->waitMutex);
    munmap(mSharedData, sizeof(SharedData));

    if (mShmId != -1) {
        close(mShmId);
        mShmId = -1;
    }

    mSharedData = nullptr;
    shm_unlink(mMutexName.c_str());
}

void ProcMutex::Lock() {
    if (!mSharedData) {
        return;
    }

    // Try to acquire the lock non - blocking
    if (pthread_mutex_trylock(&mSharedData->dataMutex) != 0) {
        AddWait();
        int rc = pthread_mutex_lock(&mSharedData->dataMutex);
        if (rc != 0) {
            perror("pthread_mutex_lock");
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
    pthread_mutex_unlock(&mSharedData->dataMutex);
}

void ProcMutex::AddWait() {
    if (!mSharedData) {
        return;
    }

    pthread_mutex_lock(&mSharedData->waitMutex);
    mSharedData->waitCnt++;
    pthread_mutex_unlock(&mSharedData->waitMutex);
}

void ProcMutex::DelWait() {
    if (!mSharedData) {
        return;
    }

    pthread_mutex_lock(&mSharedData->waitMutex);
    mSharedData->waitCnt--;
    pthread_mutex_unlock(&mSharedData->waitMutex);
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
