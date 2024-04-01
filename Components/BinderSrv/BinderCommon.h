/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BinderCommon.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/03/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __BINDER_COMMON_H__
#define __BINDER_COMMON_H__

namespace InternalEnum {

enum BinderCmd
{
    BINDER_CMD_INVALID = 0x01,
    BINDER_CMD_GET_VERSION,
    BINDER_CMD_ADD_SERVICE,
    BINDER_CMD_REMOVE_SERVICE,
    BINDER_CMD_GET_SERVICE,
    BINDER_CMD_BUTT
};

}

class BinderInfo
{
public:
    BinderInfo(int32_t key, const std::string& shmName) : mKey(key), mRefCount(0), mShmName(shmName) {}
    ~BinderInfo() = default;

    int32_t GetKey() const { return mKey; }
    int32_t GetRefCount() const { return mRefCount; }
    std::string GetShmName() const { return mShmName; }

private:
    int32_t mKey;
    int32_t mRefCount;
    std::string mShmName;
};

#endif // __BINDER_COMMON_H__
