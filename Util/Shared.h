/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Convert.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SHARED_H__
#define __SHARED_H__

#include <string>

namespace Shared {
    std::string ProduceRandomStr(int len);

    template<typename T>
    int AbsValue(T v)
    {
        return v > 0 ? v : (0 - v);
    }

    template<typename T>
    int AbsValue(T v1, T v2)
    {
        return v1 > v2 ? (v1 - v2) : (v2 - v1);
    }

    std::string GetCurTimeStr();
};

#endif

