/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : GeneralConversions.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
#ifndef __GENERAL_CONVERSIONS_H__
#define __GENERAL_CONVERSIONS_H__

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdint.h>

namespace GeneralConversions {

/**
 * @brief  String to integer
 *
 * @param value  output integer
 * @param str  input string
 * @return  0 if success, -1 if failed
 */
template <typename T>
int StringToInteger(T& value, const std::string& str)
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

/**
 * @brief  Integer to string
 *
 * @param value  input integer
 * @param str  output string
 * @return  0 if success, -1 if failed
 */
template<typename T>
int IntegerToString(const T& value, std::string& str)
{
    size_t valueSize = sizeof(T);

    for (size_t i = 0; i < valueSize; i++) {
        char ch = static_cast<char>((value >> ((valueSize - 1 - i) * 8)) & 0xFF);
        str.push_back(ch);
    }

    return 0;
}

/**
 * @brief Vector to hex string without space
 *
 * @param vec  input vector
 * @return  The string converted without space
 */
template<typename T>
std::string VectorToHexString(const std::vector<T>& vec)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (const auto& it : vec) {
        ss << std::setw(sizeof(T) * 2) << static_cast<int>(it);
    }

    return ss.str();
}

/**
 * @brief Vector to hex string with space
 *
 * @param vec  input vector
 * @return  The string converted with space
 */
template<typename T>
std::string VectorToHexStringWithSpace(const std::vector<T>& vec)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (const auto& it : vec) {
        ss << std::setw(sizeof(T) * 2) << static_cast<int>(it) << " ";
    }

    return ss.str();
}

/**
 * @brief  Vector to string
 *
 * @param vec  input vector
 * @return  The string converted
 */
template<typename T>
std::string VectorToString(const std::vector<T>& vec)
{
    std::string str(vec.begin(), vec.end());
    return str;
}

}; // namespace GeneralConversions

#endif // __GENERAL_CONVERSIONS_H__
