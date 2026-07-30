/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SharedRingBuffer.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/26
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/26 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "SharedRingBuffer.h"

#define SPR_LOG(fmt, args...)   printf(fmt, ##args)
#define SPR_LOGD(fmt, args...)  printf("%4d RingBuf D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...)  printf("%4d RingBuf W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...)  printf("%4d RingBuf E: " fmt, __LINE__, ##args)

const int RETRY_TIMES       = 10;
const int RETRY_INTERVAL_US = 10000;    // 10ms
const int RESERVER_SIZE     = 1024;

SharedRingBuffer::SharedRingBuffer(const std::string& path, uint32_t capacity)
    : mEnable(true), mRoot(nullptr), mData(nullptr), mMapCapacity(capacity), mShmPath(path) {
    int fd = open(mShmPath.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        SPR_LOGE("open failed! (%s)\n", strerror(errno));
        mEnable = false;
        return;
    }

    if (ftruncate(fd, mMapCapacity) == -1) {
        SPR_LOGE("ftruncate failed! (%s)\n", strerror(errno));
        mEnable = false;
        close(fd);
        return;
    }

    void* mapMemory = mmap(NULL, mMapCapacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);  // 无论 mmap 是否成功，都关闭文件描述符

    if (mapMemory == MAP_FAILED) {
        SPR_LOGE("mmap failed! (%s)\n", strerror(errno));
        mEnable = false;
        return;
    }

    mRoot = reinterpret_cast<Root*>(mapMemory);
    mDataCapacity = mMapCapacity - sizeof(Root);

    mRoot->rp = 0;
    mRoot->wp = 0;
    mRoot->rwStatus = CMD_WRITEABLE;
    mData = reinterpret_cast<uint8_t*>(mRoot) + sizeof(Root);
}

// Used for slave mode
SharedRingBuffer::SharedRingBuffer(const std::string& path)
{
    mEnable = true;
    int fd = open(path.c_str(), O_RDWR);
    if (fd == -1) {
        SPR_LOGE("open %s failed! (%s)\n", path.c_str(), strerror(errno));
        mEnable = false;
        mRoot = nullptr;
        mData = nullptr;
        mMapCapacity = 0;
        mDataCapacity = 0;
        mShmPath = path;
        return;
    }

    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1) {
        SPR_LOGE("fstat failed! (%s)\n", strerror(errno));
        close(fd);
        mEnable = false;
        mRoot = nullptr;
        mData = nullptr;
        mMapCapacity = 0;
        mDataCapacity = 0;
        mShmPath = path;
        return;
    }

    off_t fileSize = fileStat.st_size;
    void* mapMemory = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (mapMemory == MAP_FAILED) {
        SPR_LOGE("mmap failed! (%s)\n", strerror(errno));
        mEnable = false;
        mRoot = nullptr;
        mData = nullptr;
        mMapCapacity = 0;
        mDataCapacity = 0;
        mShmPath = path;
        return;
    }

    mShmPath = path;
    mMapCapacity = fileSize;
    mDataCapacity = mMapCapacity - sizeof(Root);
    mRoot = reinterpret_cast<Root*>(mapMemory);

    // Initialize Root structure if it's a new file or corrupted
    // Check if the rwStatus field has a valid value
    if (mRoot->rwStatus != CMD_WRITEABLE && mRoot->rwStatus != CMD_READABLE) {
        SPR_LOGW("Invalid rwStatus (%u), reinitializing...\n", mRoot->rwStatus.load());
        mRoot->rp = 0;
        mRoot->wp = 0;
        mRoot->rwStatus = CMD_WRITEABLE;
    }

    mData = reinterpret_cast<uint8_t*>(mapMemory) + sizeof(Root);
}

SharedRingBuffer::~SharedRingBuffer()
{
    munmap(mRoot, mMapCapacity);
}

int SharedRingBuffer::Write(const void* data, int32_t len)
{
    int ret = -1;
    int retry = RETRY_TIMES;

    if (!mEnable) {
        SPR_LOGE("SharedRingBuffer is disable!\n");
        return -1;
    }

    // It's hard to believe, but it actually happened:
    // Although post after it is written in the shared memory, synchronization still might not be timely,
    // and the AvailSpace() returns 0. Only add a retry to avoid it
    while (retry > 0) {
        // SPSC: writer only reads rp (reader owns it), acquires to see reader's progress
        uint32_t curWp = mRoot->wp.load(std::memory_order_relaxed);
        uint32_t curRp = mRoot->rp.load(std::memory_order_acquire);
        int32_t avail = (curWp >= curRp) ? (mDataCapacity - curWp + curRp) : (curRp - curWp);
        if (avail >= len) {
            if (curWp + (uint32_t)len >= mDataCapacity ||
                curWp >= (mDataCapacity - RESERVER_SIZE)) {
                curWp = 0;
            }
            memmove(reinterpret_cast<uint8_t*>(mData) + curWp, data, len);
            curWp = (curWp + (uint32_t)len) % mDataCapacity;
            // release: make data visible to reader before reader sees new wp
            mRoot->wp.store(curWp, std::memory_order_release);
            SetRWStatus(CMD_READABLE);
            ret = 0;
            break;
        } else {
            SPR_LOGE("AvailSpace invalid! avail = %d, len = %d, retry = %d\n", avail, len, retry);
            DumpErrorInfo();
            retry--;
            usleep(RETRY_INTERVAL_US);
        }
    }

    return ret;
}

int SharedRingBuffer::Read(void* data, int32_t len)
{
    int ret = -1;
    int retry = RETRY_TIMES;

    if (!mEnable) {
        SPR_LOGE("SharedRingBuffer is disable!\n");
        return -1;
    }

    // Refer to write comments
    while (retry > 0) {
        // SPSC: reader only reads wp (writer owns it), acquires to see writer's data
        uint32_t curWp = mRoot->wp.load(std::memory_order_acquire);
        uint32_t curRp = mRoot->rp.load(std::memory_order_relaxed);
        int32_t diff = curWp - curRp;
        int32_t avail = (diff + ((diff < 0) ? (int32_t)mDataCapacity : 0)) % (int32_t)mDataCapacity;
        if (avail >= len) {
            if (curRp + (uint32_t)len >= mDataCapacity ||
                curRp >= (mDataCapacity - RESERVER_SIZE)) {
                curRp = 0;
            }
            memcpy(data, reinterpret_cast<uint8_t*>(mData) + curRp, len);
            curRp = (curRp + (uint32_t)len) % mDataCapacity;
            // release: make space visible to writer before writer sees new rp
            mRoot->rp.store(curRp, std::memory_order_release);
            SetRWStatus(CMD_WRITEABLE);
            ret = 0;
            break;
        } else {
            SPR_LOGW("AvailData invalid! avail = %d, len = %d. (%d)\n", avail, len, retry);
            DumpErrorInfo();
            retry--;
            usleep(RETRY_INTERVAL_US);
        }
    }

    return ret;
}

int32_t SharedRingBuffer::AvailSpace() const noexcept
{
    if (!mEnable) {
        SPR_LOGE("SharedRingBuffer is disable!\n");
        return -1;
    }

    uint32_t wp = mRoot->wp.load(std::memory_order_acquire);
    uint32_t rp = mRoot->rp.load(std::memory_order_acquire);
    return (wp >= rp) ? (mDataCapacity - wp + rp) : (rp - wp);
}

int32_t SharedRingBuffer::AvailData() const noexcept
{
    if (!mEnable) {
        SPR_LOGE("SharedRingBuffer is disable!\n");
        return -1;
    }

    uint32_t wp = mRoot->wp.load(std::memory_order_acquire);
    uint32_t rp = mRoot->rp.load(std::memory_order_acquire);
    int32_t diff = wp - rp;
    return (diff + ((diff < 0) ? mDataCapacity : 0)) % mDataCapacity;
}

// // Dump the buffer content
// // Note: This function is not thread-safe.
// //       It's only used for show logs in terminal debug.
// int32_t SharedRingBuffer::DumpBuffer(void* data, int32_t len) const noexcept
// {
//     static uint32_t pos = mRoot->rp;
//     int32_t diff = mRoot->wp - pos;

//     bool avail = (diff + ((diff < 0) ? mDataCapacity : 0)) % mDataCapacity;
//     if (!avail) {
//         return -1;
//     }

//     memcpy(data, reinterpret_cast<uint8_t*>(mData) + mRoot->rp, len);
//     pos = (pos + len) % mDataCapacity;

//     return 0;
// }

bool SharedRingBuffer::IsReadable() const noexcept
{
    if (!mEnable) {
        SPR_LOGE("SharedRingBuffer is disable!\n");
        return false;
    }

    return ((mRoot->rwStatus == CMD_READABLE) && AvailData() != 0);
}

bool SharedRingBuffer::IsWriteable() const noexcept
{
    if (!mEnable) {
        SPR_LOGE("SharedRingBuffer is disable!\n");
        return false;
    }

    return ((mRoot->rwStatus == CMD_WRITEABLE && AvailSpace() != 0));
}

bool SharedRingBuffer::IsEnabled() const noexcept
{
    return mEnable;
}

void SharedRingBuffer::AdjustPosIfOverflow(uint32_t* pos, int32_t size) const noexcept
{
    if (!mEnable) {
        SPR_LOGE("SharedRingBuffer is disable!\n");
        return;
    }

    if (pos == nullptr) {
        SPR_LOGE("pos is nullptr!\n");
        return;
    }

    if (*pos + size >= mDataCapacity || *pos >= (mDataCapacity - RESERVER_SIZE)) {
        *pos = 0;
    }
}

void SharedRingBuffer::SetRWStatus(ECmdType type) const noexcept
{
    if (!mEnable) {
        SPR_LOGE("SharedRingBuffer is disable!\n");
        return;
    }

    mRoot->rwStatus.store(type, std::memory_order_release);
}

// void SharedRingBuffer::DumpMemory(const char* pAddr, uint32_t size)
// {
//     for (uint32_t i = 0; i < size; i++) {
//         SPR_LOGD("0x%p: 0x%x [%c]\n", pAddr, pAddr[i], pAddr[i]);
//     }
// }

void SharedRingBuffer::DumpErrorInfo()
{
    if (!mEnable) {
        SPR_LOGE("SharedRingBuffer is disable!\n");
        return ;
    }

    SPR_LOGD("rp: %u, wp: %u, dataCapacity: %u\n",
             mRoot->rp.load(std::memory_order_relaxed),
             mRoot->wp.load(std::memory_order_relaxed),
             mDataCapacity);
}
