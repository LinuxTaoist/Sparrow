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

#define CHECK_ONENET_RET_VALIDITY(expr) do {    \
    int32_t _ret = (expr);                      \
    if (_ret == -1) {                           \
        return _ret;                            \
    }                                           \
} while(0)

#define CHECK_ONENET_POINTER_NONRET(p) do {                     \
    if ((p) == nullptr) {                                       \
        SPR_LOGE("INVALID POINTER: %s nullptr!\n", (#p));       \
        return ;                                                \
    }                                                           \
} while(0)

#define CHECK_ONENET_POINTER(p, __err) do {                     \
    if ((p) == nullptr) {                                       \
        SPR_LOGE("INVALID POINTER: %s is nullptr!\n", (#p));    \
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
