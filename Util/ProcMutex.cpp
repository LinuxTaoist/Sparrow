/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ProcMutex.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/02
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "ProcMutex.h"

#define SPR_LOGD(fmt, args...)  printf("%s %6d %-12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "ProcMutex", __LINE__, ##args)
#define SPR_LOGW(fmt, args...)  printf("%s %6d %-12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "ProcMutex", __LINE__, ##args)
#define SPR_LOGE(fmt, args...)  printf("%s %6d %-12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "ProcMutex", __LINE__, ##args)

namespace {

// Mutex initialization state machine
const int MUTEX_STATE_UNINITIALIZED = 0;
const int MUTEX_STATE_INITIALIZING  = 1;
const int MUTEX_STATE_READY         = 2;

// Retry configuration for init waiting
const int MUTEX_INIT_WAIT_RETRY  = 100;
const useconds_t MUTEX_INIT_WAIT_US = 1000;

static std::string GetCurTimeStr() {
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);

    struct tm localTime;
    localtime_r(&currentTime.tv_sec, &localTime);
    int milliseconds = static_cast<int>(currentTime.tv_nsec / 1000000);

    char buffer[80];
    snprintf(buffer, sizeof(buffer), "%02d-%02d %02d:%02d:%02d.%03d",
             localTime.tm_mon + 1, localTime.tm_mday,
             localTime.tm_hour, localTime.tm_min, localTime.tm_sec, milliseconds);
    return std::string(buffer);
}

} // anonymous namespace

ProcMutex::ProcMutex(const std::string& mutexName)
    : mShmFd(-1), mMutexName(std::string("/") + mutexName), mSharedData(nullptr) {
    Init();
}

ProcMutex::~ProcMutex() {
    if (!mSharedData) {
        return;
    }

    // Atomically decrement ref count and get previous value to avoid TOCTOU race
    int oldRef = mSharedData->refCnt.fetch_sub(1, std::memory_order_acq_rel);
    if (oldRef - 1 == 0
        && mSharedData->waitCnt.load(std::memory_order_acquire) == 0) {
        // Last holder with no pending waiters: fully destroy shared resource
        DeInit();
        return;
    }

    // Only release local mapping and file descriptor
    munmap(mSharedData, sizeof(SharedData));
    mSharedData = nullptr;
    if (mShmFd != -1) {
        close(mShmFd);
        mShmFd = -1;
    }
}

void ProcMutex::Lock() {
    if (!mSharedData) {
        return;
    }

    int rc = pthread_mutex_trylock(&mSharedData->dataMutex);
    if (rc == 0) {
        return;
    }

    // Handle owner-dead case on trylock: lock is acquired but inconsistent
    if (rc == EOWNERDEAD) {
        SPR_LOGW("dataMutex owner died on trylock, recovering\n");
        pthread_mutex_consistent(&mSharedData->dataMutex);
        return;
    }

    // Lock contended: enter waiting state then block
    AddWait();
    rc = pthread_mutex_lock(&mSharedData->dataMutex);
    if (rc == EOWNERDEAD) {
        SPR_LOGW("dataMutex owner died on lock, recovering\n");
        if (pthread_mutex_consistent(&mSharedData->dataMutex) != 0) {
            SPR_LOGE("failed to restore data mutex consistency\n");
            DelWait();
            return;
        }
    } else if (rc != 0) {
        SPR_LOGE("pthread_mutex_lock failed: %s\n", strerror(rc));
        DelWait();
        return;
    }

    DelWait();
}

void ProcMutex::Unlock() {
    if (!mSharedData) {
        return;
    }

    int rc = pthread_mutex_unlock(&mSharedData->dataMutex);
    if (rc != 0) {
        SPR_LOGE("pthread_mutex_unlock failed: %s\n", strerror(rc));
    }
}

void ProcMutex::Init() {
    mShmFd = shm_open(mMutexName.c_str(), O_RDWR | O_CREAT, 0644);
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
        mSharedData = nullptr;
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

    // CAS ensures exactly one process performs mutex initialization
    int expected = MUTEX_STATE_UNINITIALIZED;
    if (mSharedData->initialized.compare_exchange_strong(
            expected, MUTEX_STATE_INITIALIZING,
            std::memory_order_acq_rel,
            std::memory_order_acquire)) {
        mSharedData->refCnt.store(0, std::memory_order_relaxed);
        mSharedData->waitCnt.store(0, std::memory_order_relaxed);

        rc = pthread_mutex_init(&mSharedData->dataMutex, &mutexAttr);
        if (rc != 0) {
            SPR_LOGE("init data mutex failed! (%s)\n", strerror(rc));
            mSharedData->initialized.store(MUTEX_STATE_UNINITIALIZED, std::memory_order_release);
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
            mSharedData->initialized.store(MUTEX_STATE_UNINITIALIZED, std::memory_order_release);
            pthread_mutexattr_destroy(&mutexAttr);
            munmap(mSharedData, sizeof(SharedData));
            close(mShmFd);
            mShmFd = -1;
            mSharedData = nullptr;
            return;
        }

        mSharedData->initialized.store(MUTEX_STATE_READY, std::memory_order_release);
    } else {
        // Wait for peer process to finish initialization
        int retry = 0;
        while (mSharedData->initialized.load(std::memory_order_acquire) == MUTEX_STATE_INITIALIZING
               && retry < MUTEX_INIT_WAIT_RETRY) {
            usleep(MUTEX_INIT_WAIT_US);
            ++retry;
        }

        if (mSharedData->initialized.load(std::memory_order_acquire) != MUTEX_STATE_READY) {
            SPR_LOGE("mutex %s init timeout, state=%d\n",
                     mMutexName.c_str(), mSharedData->initialized.load());
            pthread_mutexattr_destroy(&mutexAttr);
            munmap(mSharedData, sizeof(SharedData));
            close(mShmFd);
            mShmFd = -1;
            mSharedData = nullptr;
            return;
        }

        // Recover robust state from crashed previous owner
        rc = pthread_mutex_trylock(&mSharedData->dataMutex);
        if (rc == EOWNERDEAD) {
            SPR_LOGW("dataMutex owner died on re-entry, recovering\n");
            pthread_mutex_consistent(&mSharedData->dataMutex);
            pthread_mutex_unlock(&mSharedData->dataMutex);
        } else if (rc == 0) {
            pthread_mutex_unlock(&mSharedData->dataMutex);
        } else if (rc == EINVAL) {
            SPR_LOGE("dataMutex invalid on re-entry: %s\n", mMutexName.c_str());
        }

        rc = pthread_mutex_trylock(&mSharedData->waitMutex);
        if (rc == EOWNERDEAD) {
            SPR_LOGW("waitMutex owner died on re-entry, recovering\n");
            pthread_mutex_consistent(&mSharedData->waitMutex);
            pthread_mutex_unlock(&mSharedData->waitMutex);
        } else if (rc == 0) {
            pthread_mutex_unlock(&mSharedData->waitMutex);
        } else if (rc == EINVAL) {
            SPR_LOGE("waitMutex invalid on re-entry: %s\n", mMutexName.c_str());
        }
    }

    mSharedData->refCnt.fetch_add(1, std::memory_order_relaxed);
    pthread_mutexattr_destroy(&mutexAttr);
}

void ProcMutex::DeInit() {
    if (!mSharedData) {
        return;
    }

    int rc = pthread_mutex_destroy(&mSharedData->dataMutex);
    if (rc != 0) {
        SPR_LOGE("destroy data mutex failed! (%s)\n", strerror(rc));
    }

    rc = pthread_mutex_destroy(&mSharedData->waitMutex);
    if (rc != 0) {
        SPR_LOGE("destroy wait mutex failed! (%s)\n", strerror(rc));
    }

    munmap(mSharedData, sizeof(SharedData));
    mSharedData = nullptr;

    if (mShmFd != -1) {
        if (close(mShmFd) == -1) {
            SPR_LOGE("close shm fd failed! (%s)\n", strerror(errno));
        }
        mShmFd = -1;
    }

    shm_unlink(mMutexName.c_str());
}

void ProcMutex::AddWait() {
    if (!mSharedData) {
        return;
    }

    int rc = pthread_mutex_lock(&mSharedData->waitMutex);
    if (rc == EOWNERDEAD) {
        SPR_LOGW("waitMutex owner died in AddWait, recovering\n");
        pthread_mutex_consistent(&mSharedData->waitMutex);
    } else if (rc != 0) {
        SPR_LOGE("lock waitMutex failed: %s\n", strerror(rc));
        return;
    }

    mSharedData->waitCnt.fetch_add(1, std::memory_order_relaxed);
    pthread_mutex_unlock(&mSharedData->waitMutex);
}

void ProcMutex::DelWait() {
    if (!mSharedData) {
        return;
    }

    int rc = pthread_mutex_lock(&mSharedData->waitMutex);
    if (rc == EOWNERDEAD) {
        SPR_LOGW("waitMutex owner died in DelWait, recovering\n");
        pthread_mutex_consistent(&mSharedData->waitMutex);
    } else if (rc != 0) {
        SPR_LOGE("lock waitMutex failed: %s\n", strerror(rc));
        return;
    }

    mSharedData->waitCnt.fetch_sub(1, std::memory_order_relaxed);
    pthread_mutex_unlock(&mSharedData->waitMutex);
}

// -----------------------------------------------------------------------------
// ProcLockGuard
// -----------------------------------------------------------------------------

ProcLockGuard::ProcLockGuard(ProcMutex& pMutex, std::mutex& tMutex)
    : mTMutex(tMutex), mPMutex(pMutex) {
    mTMutex.lock();
    mPMutex.Lock();
}

ProcLockGuard::~ProcLockGuard() {
    mPMutex.Unlock();
    mTMutex.unlock();
}
