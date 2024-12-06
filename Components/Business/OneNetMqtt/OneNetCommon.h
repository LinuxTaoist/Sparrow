/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetCommon.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/20
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/20 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __ONENET_COMMON_H__
#define __ONENET_COMMON_H__

#define CHECK_ONENET_RET_VALIDITY(__expr) do {      \
    int32_t __ret = (__expr);                       \
    if (__ret == -1) {                              \
        return __ret;                               \
    }                                               \
} while(0)

#define CHECK_ONENET_POINTER_NONRET(__p) do {                   \
    if ((__p) == nullptr) {                                     \
        SPR_LOGE("INVALID POINTER: %s nullptr!\n", (#__p));     \
        return ;                                                \
    }                                                           \
} while(0)

#define CHECK_ONENET_POINTER(__p, __err) do {                   \
    if ((__p) == nullptr) {                                     \
        SPR_LOGE("INVALID POINTER: %s is nullptr!\n", (#__p));  \
        return __err;                                           \
    }                                                           \
} while(0)

struct SOneNetPublishParam
{
    uint8_t     flags;
    uint16_t    identifier;
    char        topic[128];
    char        payload[512];
};

#endif // __ONENET_COMMON_H__
