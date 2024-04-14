/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Parcel.h
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
#ifndef __PARCEL_H__
#define __PARCEL_H__

#include <fcntl.h>
#include <semaphore.h>
#include <string>
#include "SharedRingBuffer.h"

class Parcel
{
public:
    Parcel(const std::string& path, int key, bool master);
    ~Parcel();
    Parcel(const Parcel& other) = delete;
    Parcel& operator=(const Parcel& other) = delete;
    Parcel(Parcel&& other) = delete;
    Parcel& operator=(Parcel&& other) = delete;

    int WriteBool(bool value);
    int ReadBool(bool& value);
    int WriteInt(int value);
    int ReadInt(int& value);
    int WriteString(const std::string& value);
    int ReadString(std::string& value);
    int WriteData(void* data, int size);
    int ReadData(void* data, int& size);
    int Wait();
    int Post();

private:
    bool                mMaster;
    int                 mShmKey;
    sem_t*              mSem ;
    std::string         mShmPath;
    SharedRingBuffer*   mRingBuffer;
};

#endif // __PARCEL_H__
