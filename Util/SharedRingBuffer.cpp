/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SharedRingBuffer.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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
#define SPR_LOGE(fmt, args...)  printf("%4d RingBuf E: " fmt, __LINE__, ##args)

const int RETRY_TIMES       = 3;        // 3 times retry
const int RETRY_INTERVAL_US = 10000;    // 10ms

// Used for master mode
SharedRingBuffer::SharedRingBuffer(std::string path, uint32_t capacity)
    : mCapacity(capacity), mShmPath(path)
{
    int fd = open(mShmPath.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        SPR_LOGE("open failed! (%s)", strerror(errno));
    }

    if (ftruncate(fd, mCapacity) == -1) {
        SPR_LOGE("ftruncate failed! (%s)", strerror(errno));
    }

    void* mapMemory = mmap(NULL, mCapacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapMemory == MAP_FAILED) {
        SPR_LOGE("mmap failed! (%s)", strerror(errno));
    }

    mRoot = static_cast<Root*>(mapMemory);
    if (mRoot == nullptr) {
        SPR_LOGE("mRoot is nullptr!");
        exit(0);
    }

    mRoot->rp = mRoot->wp;
    mData = static_cast<uint8_t*>(mapMemory) + sizeof(Root);
    if (mData == nullptr) {
        SPR_LOGE("mData is nullptr!");
        exit(0);
    }
    close(fd);
}

// Used for slave mode
SharedRingBuffer::SharedRingBuffer(std::string path)
{
    int fd = open(path.c_str(), O_RDWR);
    if (fd == -1) {
        SPR_LOGE("open %s failed! (%s)\n", mShmPath.c_str(), strerror(errno));
    }

    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1) {
        SPR_LOGE("fstat failed! (%s)\n", strerror(errno));
    }

    off_t fileSize = fileStat.st_size;
    void* mapMemory = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapMemory == MAP_FAILED) {
        SPR_LOGE("mmap failed! (%s)\n", strerror(errno));
    }

    mCapacity = fileSize;
    mShmPath = path;
    mRoot = static_cast<Root*>(mapMemory);
    if (mRoot == nullptr) {
        SPR_LOGE("mRoot is nullptr!");
        exit(0);
    }

    mData = static_cast<uint8_t*>(mapMemory) + sizeof(Root);
    if (mData == nullptr) {
        SPR_LOGE("mData is nullptr!");
        exit(0);
    }
    close(fd);
}

SharedRingBuffer::~SharedRingBuffer()
{
    munmap(mRoot, mCapacity);
}

int SharedRingBuffer::write(const void* data, int32_t len)
{
    int ret = -1;
    int retry = RETRY_TIMES;

    // It's hard to believe, but it actually happened:
    // Although post after it is written in the shared memory, synchronization still might not be timely,
    // and the AvailSpace() returns 0. Only add a retry to avoid it
    while (retry > 0) {
        std::lock_guard<std::mutex> lock(mMutex);
        int32_t avail = AvailSpace();
        if (avail >= len) {
            memcpy(static_cast<char*>(mData) + mRoot->wp, data, len);
            mRoot->wp = (mRoot->wp + len) % mCapacity;
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

int SharedRingBuffer::read(void* data, int32_t len)
{
    int ret = -1;
    int retry = RETRY_TIMES;

    // Refer to write comments
    while (retry > 0) {
        std::lock_guard<std::mutex> lock(mMutex);
        int32_t avail = AvailData();
        if (avail >= len) {
            memcpy(data, static_cast<char*>(mData) + mRoot->rp, len);
            mRoot->rp = (mRoot->rp + len) % mCapacity;
            SetRWStatus(CMD_WRITEABLE);
            ret = 0;

            break;
        } else {
            SPR_LOGE("AvailData invalid! avail = %d, len = %d\n", avail, len);
            DumpErrorInfo();
            retry--;
            usleep(RETRY_INTERVAL_US);
        }
    }

    return ret;
}

int32_t SharedRingBuffer::AvailSpace() const noexcept
{
    return (mRoot->wp >= mRoot->rp) ? (mCapacity - mRoot->wp + mRoot->rp) : (mRoot->rp - mRoot->wp);
}

int32_t SharedRingBuffer::AvailData() const noexcept
{
    int32_t diff = mRoot->wp - mRoot->rp;
    return (diff + ((diff < 0) ? mCapacity : 0)) % mCapacity;
}

// Dump the buffer content
// Note: This function is not thread-safe.
//       It's only used for show logs in terminal debug.
int32_t SharedRingBuffer::DumpBuffer(void* data, int32_t len) const noexcept
{
    static uint32_t pos = mRoot->rp;
    int32_t diff = mRoot->wp - pos;

    bool avail = (diff + ((diff < 0) ? mCapacity : 0)) % mCapacity;
    if (!avail) {
        return -1;
    }

    memcpy(data, static_cast<char*>(mData) + mRoot->rp, len);
    pos = (pos + len) % mCapacity;

    return 0;
}

bool SharedRingBuffer::IsReadable() const noexcept
{
    return ((mRoot->rwStatus == CMD_READABLE) && AvailData() != 0);
}

bool SharedRingBuffer::IsWriteable() const noexcept
{
    return ((mRoot->rwStatus == CMD_WRITEABLE && AvailData() != 0));
}

void SharedRingBuffer::SetRWStatus(ECmdType type) const noexcept
{
    mRoot->rwStatus = type;
}

void SharedRingBuffer::DumpMemory(const char* pAddr, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++) {
        SPR_LOGD("0x%p: 0x%x [%c]\n", pAddr, pAddr[i], pAddr[i]);
    }
}

void SharedRingBuffer::DumpErrorInfo()
{
    SPR_LOGD("rp: %d, wp: %d, capacity: %d\n", mRoot->rp, mRoot->wp, mCapacity);
}
