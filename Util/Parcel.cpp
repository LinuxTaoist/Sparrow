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

const int SHM_MAX_SIZE  = 10 * 1024;    // 10KB

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
        sem_close(mSem);
        sem_unlink((mShmPath + std::to_string(mShmKey)).c_str());
    }

    if (mRingBuffer != nullptr) {
        delete mRingBuffer;
        mRingBuffer = nullptr;
    }
}

int Parcel::WriteBool(bool value)
{
    int ret = 0;

    NODE_LENGTH_T len = sizeof(value);
    ret = mRingBuffer->Write(&len, sizeof(NODE_LENGTH_T));
    if (ret != 0) {
        return ret;
    }

    uint8_t data = value ? 1 : 0;
    ret = mRingBuffer->Write(&data, len);
    return ret;
}

int Parcel::ReadBool(bool& value)
{
    int ret = 0;

    NODE_LENGTH_T len = 0;
    ret = mRingBuffer->Read(&len, sizeof(NODE_LENGTH_T));
    if (ret != 0) {
        return ret;
    }

    uint8_t data = 0;
    ret = mRingBuffer->Read(&data, len);
    value = data ? true : false;

    return ret;
}

int Parcel::WriteInt(int value)
{
    int ret = 0;

    int netValue = htonl(value);
    NODE_LENGTH_T len = sizeof(netValue);
    ret = mRingBuffer->Write(&len, sizeof(NODE_LENGTH_T));
    if (ret != 0) {
        return ret;
    }

    uint8_t* data = reinterpret_cast<uint8_t*>(&netValue);
    ret = mRingBuffer->Write(data, len);
    return ret;
}

int Parcel::ReadInt(int& value)
{
    int ret = 0;

    NODE_LENGTH_T len = 0;
    ret = mRingBuffer->Read(&len, sizeof(NODE_LENGTH_T));
    if (ret != 0 || len != sizeof(int)) {
        return ret;
    }

    uint8_t data[sizeof(int)];
    ret = mRingBuffer->Read(data, len);
    if (ret == 0) {
        int temp;
        memcpy(&temp, data, sizeof(int));
        value = ntohl(temp);
    }

    return ret;
}

int Parcel::WriteString(const std::string& value)
{
    NODE_LENGTH_T len = value.length();
    if (mRingBuffer->Write(&len, sizeof(NODE_LENGTH_T)) != 0) {
        SPR_LOGE("Write string len failed!\n");
        return -1;
    }

    return mRingBuffer->Write(value.c_str(), len);
}

int Parcel::ReadString(std::string& value)
{
    int ret = 0;

    NODE_LENGTH_T len = 0;
    ret = mRingBuffer->Read(&len, sizeof(NODE_LENGTH_T));
    if (ret != 0 || len == 0) {
        SPR_LOGE("Read string len failed! len = %d, ret = %d\n", len, ret);
        return -1;
    }

    value.resize(len);
    char* data = const_cast<char*>(value.c_str());
    ret = mRingBuffer->Read(data, len);
    return ret;
}

int Parcel::WriteData(void* data, int size)
{
    NODE_LENGTH_T len = size;
    if (mRingBuffer->Write(&len, sizeof(NODE_LENGTH_T)) != 0) {
        return -1;
    }

    return mRingBuffer->Write(data, len);
}

int Parcel::ReadData(void* data, int& size)
{
    int ret = 0;
    NODE_LENGTH_T len = 0;
    ret = mRingBuffer->Read(&len, sizeof(NODE_LENGTH_T));
    if (ret != 0 || len == 0) {
        return -1;
    }

    size = len;
    return mRingBuffer->Read(data, len);
}

int Parcel::Wait()
{
    while (!mRingBuffer->IsReadable())  {
        if (sem_wait(mSem) != 0) {
            SPR_LOGE("sem_wait failed! (%s)\n", strerror(errno));
        }
    }

    return 0;
}

int Parcel::Post()
{
    if (sem_post(mSem) != 0) {
        SPR_LOGE("sem_post failed! (%s)\n", strerror(errno));
    }

    return 0;
}
