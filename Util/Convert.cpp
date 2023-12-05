/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Convert.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */
#include <string.h>
#include <cstdint>
#include "Convert.h"

#define SPR_LOGD(fmt, args...) printf("%d SprMsg D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d SprMsg W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SprMsg E: " fmt, __LINE__, ##args)

namespace Convert
{
    int8_t charPtrToUint32(uint32_t& value, const char* buf, int size)
    {
        if (buf == nullptr || size < (int)sizeof(uint32_t))
        {
            SPR_LOGE("param is invalid!\n");
            return -1;
        }

        memcpy(&value, buf, sizeof(uint32_t));
        return 0;
    }

    int8_t uint32ToString(uint32_t value, std::string& buf)
    {
        const char* pByte = reinterpret_cast<const char*>(&value);
        buf.append(pByte, sizeof(uint32_t));
        return 0;
    }
};

