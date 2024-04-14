/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SharedRingBuffer.h
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
#ifndef __SHARED_RING_BUFFER_H__
#define __SHARED_RING_BUFFER_H__

#include <mutex>
#include <string>
#include <stdint.h>

struct Root
{
    uint8_t  cmd;       // 使能状态
    uint8_t  busy;      // 忙碌状态
    uint8_t  rwStatus;  // 可读状态
    uint32_t wp;        // 写入位置
    uint32_t rp;        // 读取位置
};

enum ECmdType
{
    CMD_WRITEABLE   = 0x01,
    CMD_READABLE    = 0x02,
    CMD_BUTT,
};

class SharedRingBuffer
{
public:
    /**
     * @brief Constructs a master Shared Ring Buffer object.
     * @param path  The path to the shared memory.
     * @param capacity The buffer's capacity.
     *
     * Intended for use in master mode with shared memory refreshing.
     */
    SharedRingBuffer(const std::string& path, uint32_t capacity);

    /**
     * @brief Constructs a slave Shared Ring Buffer object
     * @param path The path to the shared memory.
     *
     * This constructor creates an instance of a slave Shared Ring Buffer, typically used by client applications.
     * It facilitates access and utilization of the shared buffer by referencing it through the specified path.
     */
    explicit SharedRingBuffer(const std::string& path);
    ~SharedRingBuffer();

    bool    IsReadable()    const noexcept;
    bool    IsWriteable()   const noexcept;
    int     write(const void* data, int32_t len);
    int     read(void* data, int32_t len);
    // int     DumpBuffer(void* data, int32_t len) const noexcept;

    int32_t AvailSpace()    const noexcept;
    int32_t AvailData()     const noexcept;


private:
    void    AdjustPosIfOverflow(uint32_t* pos, int32_t size) const noexcept;
    void    SetRWStatus(ECmdType type) const noexcept;
    // void    DumpMemory(const char* pAddr, uint32_t size);
    void    DumpErrorInfo();

private:
    Root*       mRoot;
    void*       mData;
    uint32_t    mCapacity;
    std::mutex  mMutex;
    std::string mShmPath;
};

#endif // __SHARED_RING_BUFFER_H__
