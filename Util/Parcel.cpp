/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Parcel.cpp
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
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include "Parcel.h"

#define SPR_LOG(fmt, args...)   printf(fmt, ##args)
#define SPR_LOGD(fmt, args...)  printf("%4d Parcel D: [%c] " fmt, __LINE__, mMaster ? 'm' : 's', ##args)
#define SPR_LOGE(fmt, args...)  printf("%4d Parcel E: [%c] " fmt, __LINE__, mMaster ? 'm' : 's', ##args)

typedef  uint16_t       NODE_LENGTH_T;

#define  SHM_ROOT_PATH  "/tmp/"

const int ERR_PARCEL_SUCCESS        = 0;
const int ERR_PARCEL_WAIT_FAILED    = -300;
const int ERR_PARCEL_WAIT_TIMEOUT   = -301;
const int ERR_PARCEL_POST_FAILED    = -302;
const int ERR_PARCEL_WRITE_FAILED   = -303;
const int ERR_PARCEL_READ_FAILED    = -304;
const int SHM_MAX_SIZE              = 10 * 1024;    // 10KB

Parcel::Parcel(const std::string& path, int key, bool master) : mMaster(master), mShmKey(key), mShmPath(path)
{
    mRingBuffer = new (std::nothrow)SharedRingBuffer(std::string(SHM_ROOT_PATH) + path, SHM_MAX_SIZE);
    if (mRingBuffer == nullptr) {
        SPR_LOGE("mRingBuffer is nullptr!\n");
    }

    mSem = sem_open((mShmPath + std::to_string(mShmKey)).c_str(), O_CREAT, 0666, 1);
    if (mSem == SEM_FAILED) {
        SPR_LOGE("sem_open %s failed! (%s)\n", (mShmPath + std::to_string(mShmKey)).c_str(), strerror(errno));
    }
}

Parcel::~Parcel()
{
    if (SEM_FAILED != mSem) {
        if (sem_close(mSem) != 0) {
            SPR_LOGE("sem_close failed! (%s)\n", strerror(errno));
        }

        // delete semaphore only when master
        if (mMaster) {
            std::string semPath = mShmPath + std::to_string(mShmKey);
            if (sem_unlink(semPath.c_str()) != 0) {
                SPR_LOGE("sem_unlink %s failed! (%s)\n", semPath.c_str(), strerror(errno));
            }
        }
    }

    if (mRingBuffer != nullptr) {
        delete mRingBuffer;
        mRingBuffer = nullptr;
    }
}

int Parcel::WriteBool(bool value)
{
    NODE_LENGTH_T len = sizeof(value);
    if (ERR_PARCEL_SUCCESS != mRingBuffer->Write(&len, sizeof(NODE_LENGTH_T)) ) {
        return ERR_PARCEL_WRITE_FAILED;
    }

    uint8_t data = value ? 1 : 0;
    if (ERR_PARCEL_SUCCESS != mRingBuffer->Write(&data, len)) {
        return ERR_PARCEL_WRITE_FAILED;
    }

    return ERR_PARCEL_SUCCESS;
}

int Parcel::ReadBool(bool& value)
{
    NODE_LENGTH_T len = 0;
    if (ERR_PARCEL_SUCCESS != mRingBuffer->Read(&len, sizeof(NODE_LENGTH_T))) {
        return ERR_PARCEL_READ_FAILED;
    }

    uint8_t data = 0;
    if (ERR_PARCEL_SUCCESS != mRingBuffer->Read(&data, len)) {
        return ERR_PARCEL_READ_FAILED;
    }

    value = data ? true : false;
    return ERR_PARCEL_SUCCESS;
}

int Parcel::WriteInt(int value)
{
    int netValue = htonl(value);
    NODE_LENGTH_T len = sizeof(netValue);
    if (ERR_PARCEL_SUCCESS != mRingBuffer->Write(&len, sizeof(NODE_LENGTH_T))) {
        return ERR_PARCEL_WRITE_FAILED;
    }

    uint8_t* data = reinterpret_cast<uint8_t*>(&netValue);
    if (ERR_PARCEL_SUCCESS != mRingBuffer->Write(data, len)) {
        return ERR_PARCEL_WRITE_FAILED;
    }

    return ERR_PARCEL_SUCCESS;
}

int Parcel::ReadInt(int& value)
{
    NODE_LENGTH_T len = 0;
    if (ERR_PARCEL_SUCCESS != mRingBuffer->Read(&len, sizeof(NODE_LENGTH_T))) {
        return ERR_PARCEL_READ_FAILED;
    }

    uint8_t data[sizeof(int)];
    if (ERR_PARCEL_SUCCESS != mRingBuffer->Read(data, len)) {
        return ERR_PARCEL_READ_FAILED;
    }

    int temp;
    memcpy(&temp, data, sizeof(int));
    value = ntohl(temp);

    return ERR_PARCEL_SUCCESS;
}

int Parcel::WriteString(const std::string& value)
{
    NODE_LENGTH_T len = value.length();
    if (ERR_PARCEL_SUCCESS != mRingBuffer->Write(&len, sizeof(NODE_LENGTH_T))) {
        SPR_LOGE("Write string len failed!\n");
        return ERR_PARCEL_WRITE_FAILED;
    }

    if (ERR_PARCEL_SUCCESS != mRingBuffer->Write(value.c_str(), len)) {
        SPR_LOGE("Write string failed!\n");
        return ERR_PARCEL_WRITE_FAILED;
    }

    return ERR_PARCEL_SUCCESS;
}

int Parcel::ReadString(std::string& value)
{
    int ret = 0;
    NODE_LENGTH_T len = 0;

    ret = mRingBuffer->Read(&len, sizeof(NODE_LENGTH_T));
    if (len == 0 || ERR_PARCEL_SUCCESS != ret) {
        SPR_LOGE("Read string len failed! len = %d, ret = %d\n", len, ret);
        return ERR_PARCEL_READ_FAILED;
    }

    value.resize(len);
    char* data = const_cast<char*>(value.c_str());
    if (ERR_PARCEL_SUCCESS != mRingBuffer->Read(data, len)) {
        SPR_LOGE("Read string failed!\n");
        return ERR_PARCEL_READ_FAILED;
    }

    return ERR_PARCEL_SUCCESS;
}

int Parcel::WriteData(void* data, int size)
{
    NODE_LENGTH_T len = size;
    if (ERR_PARCEL_SUCCESS != mRingBuffer->Write(&len, sizeof(NODE_LENGTH_T))) {
        return ERR_PARCEL_WRITE_FAILED;
    }

    if (ERR_PARCEL_SUCCESS != mRingBuffer->Write(data, len)) {
        return ERR_PARCEL_WRITE_FAILED;
    }

    return ERR_PARCEL_SUCCESS;
}

int Parcel::ReadData(void* data, int& size)
{
    int ret = 0;
    NODE_LENGTH_T len = 0;
    ret = mRingBuffer->Read(&len, sizeof(NODE_LENGTH_T));
    if (len == 0 || ERR_PARCEL_SUCCESS != ret) {
        return ERR_PARCEL_READ_FAILED;
    }

    size = len;
    if (ERR_PARCEL_SUCCESS != mRingBuffer->Read(data, len)) {
        return ERR_PARCEL_READ_FAILED;
    }

    return ERR_PARCEL_SUCCESS;
}

int Parcel::Wait()
{
    while (!mRingBuffer->IsReadable()) {
        if (sem_wait(mSem) != 0) {
            SPR_LOGE("sem_wait failed! (%s)\n", strerror(errno));
            return ERR_PARCEL_WAIT_FAILED;
        }
    }

    return ERR_PARCEL_SUCCESS;
}

int Parcel::TimedWait(int timeout)
{
    while (!mRingBuffer->IsReadable()) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout / 1000;
        ts.tv_nsec += (timeout % 1000) * 1000000;
        if (sem_timedwait(mSem, &ts) != 0) {
            SPR_LOGE("sem_timedwait failed! (%s)\n", strerror(errno));
            int ret = (errno == ETIMEDOUT) ? ERR_PARCEL_WAIT_TIMEOUT : ERR_PARCEL_WAIT_FAILED;
            return ret;
        }
    }

    return ERR_PARCEL_SUCCESS;
}

int Parcel::Post()
{
    if (sem_post(mSem) != 0) {
        SPR_LOGE("sem_post failed! (%s)\n", strerror(errno));
        return ERR_PARCEL_POST_FAILED;
    }

    return ERR_PARCEL_SUCCESS;
}
