/**
 *------------------------------------------------------------------------------
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
 *------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */
#ifndef __CONVERT_H__
#define __CONVERT_H__

#include <string>
#include <sstream>
#include<stdint.h>

namespace Convert {
    template <typename T>
    int8_t stringToInt(T& value, const std::string& str)
    {
        if (str.size() < sizeof(T)) {
            return -1;
        }

        value = 0;
        for (size_t i = 0; i < sizeof(T); i++) {
            value <<= 8;
            value |= static_cast<unsigned char>(str[i]);
        }

        return 0;
    }

    template<typename T>
    int8_t intToString(const T& value, std::string& str)
    {
        size_t valueSize = sizeof(T);

        for (size_t i = 0; i < valueSize; i++) {
            char ch = static_cast<char>((value >> ((valueSize - 1 - i) * 8)) & 0xFF);
            str.push_back(ch);
        }

        return 0;
    }
};

#endif
