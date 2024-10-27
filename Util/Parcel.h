/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Parcel.h
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
#ifndef __PARCEL_H__
#define __PARCEL_H__

#include <vector>
#include <string>
#include <fcntl.h>
#include <semaphore.h>
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

    int Wait();
    int Post();
    int WriteBool(bool value);
    int ReadBool(bool& value);
    int WriteInt(int value);
    int ReadInt(int& value);
    int WriteString(const std::string& value);
    int ReadString(std::string& value);
    int WriteData(void* data, int size);
    int ReadData(void* data, int& size);

    template<typename T>
    int WriteVector(const std::vector<T>& vec)
    {
        int size = vec.size() * sizeof(T);
        if (WriteInt(size) != 0) {
            return -1;
        }

        for (const auto& v : vec) {
            if (WriteData((void*)&v, (int)sizeof(T)) != 0) {
                return -1;
            }
        }

        return 0;
    }

    template<typename T>
    int ReadVector(std::vector<T>& vec)
    {
        int totalSize = 0;
        if (ReadInt(totalSize) != 0) {
            return -1;
        }

        unsigned capacity = totalSize / sizeof(T);
        vec.clear();
        for (unsigned i = 0; i < capacity; i++) {
            T value;
            int byteSize;
            if (ReadData((void*)&value, byteSize) != 0) {
                return -1;
            }
            vec.push_back(value);
        }

        return 0;
    }

private:
    bool                mMaster;
    int                 mShmKey;
    sem_t*              mSem ;
    std::string         mShmPath;
    SharedRingBuffer*   mRingBuffer;
};

#endif // __PARCEL_H__
