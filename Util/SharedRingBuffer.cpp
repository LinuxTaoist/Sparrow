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

// Used for master mode
SharedRingBuffer::SharedRingBuffer(const std::string& path, uint32_t capacity)
    : mMapCapacity(capacity), mShmPath(path)
{
    mEnable = true;
    int fd = open(mShmPath.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        SPR_LOGE("open failed! (%s)\n", strerror(errno));
        mEnable = false;
    }

    if (ftruncate(fd, mMapCapacity) == -1) {
        SPR_LOGE("ftruncate failed! (%s)\n", strerror(errno));
        mEnable = false;
    }

    void* mapMemory = mmap(NULL, mMapCapacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapMemory == MAP_FAILED) {
        SPR_LOGE("mmap failed! (%s)\n", strerror(errno));
        mEnable = false;
    }

    mRoot = reinterpret_cast<Root*>(mapMemory);
    if (mRoot == nullptr) {
        SPR_LOGE("mRoot is nullptr!\n");
        mEnable = false;
    }

    mDataCapacity = mMapCapacity - sizeof(Root);
    mRoot->rp = mRoot->wp;
    mData = reinterpret_cast<uint8_t*>(mRoot) + sizeof(Root);
    if (mData == nullptr) {
        SPR_LOGE("mData is nullptr!\n");
        mEnable = false;
    }

    close(fd);
}

// Used for slave mode
SharedRingBuffer::SharedRingBuffer(const std::string& path)
{
    mEnable = true;
    int fd = open(path.c_str(), O_RDWR);
    if (fd == -1) {
        SPR_LOGE("open %s failed! (%s)\n", mShmPath.c_str(), strerror(errno));
        mEnable = false;
    }

    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1) {
        SPR_LOGE("fstat failed! (%s)\n", strerror(errno));
        mEnable = false;
    }

    off_t fileSize = fileStat.st_size;
    void* mapMemory = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapMemory == MAP_FAILED) {
        SPR_LOGE("mmap failed! (%s)\n", strerror(errno));
        mEnable = false;
    }

    mShmPath = path;
    mMapCapacity = fileSize;
    mDataCapacity = mMapCapacity - sizeof(Root);
    mRoot = reinterpret_cast<Root*>(mapMemory);
    if (mRoot == nullptr) {
        SPR_LOGE("mRoot is nullptr!\n");
        mEnable = false;
    }

    mData = reinterpret_cast<uint8_t*>(mapMemory) + sizeof(Root);
    if (mData == nullptr) {
        SPR_LOGE("mData is nullptr!\n");
        mEnable = false;
    }

    close(fd);
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
        std::lock_guard<std::mutex> lock(mMutex);
        int32_t avail = AvailSpace();
        if (avail >= len) {
            AdjustPosIfOverflow(&mRoot->wp, len);
            memmove(reinterpret_cast<uint8_t*>(mData) + mRoot->wp, data, len);
            mRoot->wp = (mRoot->wp + (uint32_t)len) % mDataCapacity;
            SetRWStatus(CMD_READABLE);
            ret = 0;
            break;
        } else {
            SPR_LOGE("AvailSpace invalid! avail = %d\n", avail);
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
        std::lock_guard<std::mutex> lock(mMutex);
        int32_t avail = AvailData();
        if (avail >= len) {
            AdjustPosIfOverflow(&mRoot->rp, len);
            memcpy(data, reinterpret_cast<uint8_t*>(mData) + mRoot->rp, len);
            mRoot->rp = (mRoot->rp + len) % mDataCapacity;
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

    return (mRoot->wp >= mRoot->rp) ? (mDataCapacity - mRoot->wp + mRoot->rp) : (mRoot->rp - mRoot->wp);
}

int32_t SharedRingBuffer::AvailData() const noexcept
{
    if (!mEnable) {
        SPR_LOGE("SharedRingBuffer is disable!\n");
        return -1;
    }

    int32_t diff = mRoot->wp - mRoot->rp;
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

    return ((mRoot->rwStatus == CMD_WRITEABLE && AvailData() != 0));
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

    mRoot->rwStatus = type;
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

    SPR_LOGD("rp: %u, wp: %u, dataCapacity: %u\n", mRoot->rp, mRoot->wp, mDataCapacity);
}
