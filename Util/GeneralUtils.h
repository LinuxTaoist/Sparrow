/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : GeneralUtils.h
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
#ifndef __GENERAL_UTILS_H__
#define __GENERAL_UTILS_H__

#include <string>

namespace GeneralUtils {
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

    int RandomDecimalDigits(int digits);
    int SystemCmd(std::string& out, const char *format, ...);

    std::string GetCurTimeStr();
    std::string RandomString(int len);

    // String general interfaces
    std::string GetSubstringAfterLastDelimiter(const std::string& str, char delimiter);
};

#endif //__GENERAL_UTILS_H__
