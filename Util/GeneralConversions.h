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
 * @param out  output integer
 * @param in  input string
 * @return  0 if success, -1 if failed
 */
template <typename T>
int ToInteger(const std::string& in, T& out)
{
    size_t size = sizeof(T);
    if (in.size() < size) {
        return -1;
    }

    out = 0;
    for (size_t i = 0; i < size; i++) {
        out <<= 8;
        out |= static_cast<unsigned char>(in[i]);
    }

    return size;
}

/**
 * @brief  Integer to string
 *
 * @param in  input integer
 * @param out  output string
 * @return  0 if success, -1 if failed
 */
template<typename T>
int ToString(const T& in, std::string& out)
{
    size_t size = sizeof(T);
    for (size_t i = 0; i < size; i++) {
        char ch = static_cast<char>((in >> ((size - 1 - i) * 8)) & 0xFF);
        out.push_back(ch);
    }

    return size;
}

/**
 * @brief Vector to hex string without space
 *
 * @param vec  input vector
 * @return  The string converted without space
 */
template<typename T>
std::string ToHexString(const std::vector<T>& vec)
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
std::string ToHexStringWithSpace(const std::vector<T>& vec)
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
std::string ToString(const std::vector<T>& vec)
{
    std::string str(vec.begin(), vec.end());
    return str;
}

/**
 * @brief Dump ascall bytes
 *
 * @param in bytes
 * @return 0 on success, or -1 if an error occurred
 */
int DumpBytesAscall(const std::string& in, std::string& out);

}; // namespace GeneralConversions

#endif // __GENERAL_CONVERSIONS_H__
