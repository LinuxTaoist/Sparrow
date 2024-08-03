/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : GeneralUtils.h
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
#ifndef __GENERAL_UTILS_H__
#define __GENERAL_UTILS_H__

#include <string>

namespace GeneralUtils {

/**
 * @brief  Get absolute value
 *
 * @param v  input value
 * @return  absolute value of v
 */
template<typename T>
int AbsValue(T v)
{
    return v > 0 ? v : (0 - v);
}

/**
 * @brief  Get absolute value between v1 and v2
 *
 * @param v1  input value 1
 * @param v2  input value 2
 * @return  absolute value between v1 and v2
 */
template<typename T>
int AbsValue(T v1, T v2)
{
    return v1 > v2 ? (v1 - v2) : (v2 - v1);
}

/**
 * @brief  Register signal handler
 *
 * @param signalHandler  signal handler function
 * @return  0 if success, -1 if failed
 */
int InitSignalHandler(void (*signalHandler)(int));

/**
 * @brief  system command exec without output
 *
 * @param format  command format
 * @param ...  command parameters
 * @return  0 if success, -1 if failed
 */
int SystemCmd(const char *format, ...);

/**
 * @brief  system command exec with output
 *
 * @param out  output string
 * @param format  command format
 * @param ...  command parameters
 * @return  0 if success, -1 if failed
 */
int SystemCmd(std::string& out, const char *format, ...);

/**
 * @brief  Get the current time as a string.
 *
 * @return  current time string
 *
 * The format of the returned string is "2024-08-03 10:36:36.397".
 */
std::string GetCurTimeStr();

/**
 * @brief  Gets a random integer of digits widths
 *
 * @param width  rendom integer of width
 * @return  random integer
 */
int GetRandomInteger(int width);

/**
 * @brief  Get a random string of length len
 *
 * @param width  length of the random string
 * @return  random string
 */
std::string GetRandomString(int width);

/**
 * @brief Get the Substring After Last delimiter object
 *
 * @param str  input string
 * @param delimiter  input delimiter
 * @return  substring after last delimiter
 */
std::string GetSubstringAfterLastDelimiter(const std::string& str, char delimiter);

/**
 * @brief  Get the character after nth target object
 *
 * @param str  Input string to search within.
 * @param targetChar  The target character to find.
 * @param index  The 1-based index of targer.
 * @param out  The character after nth if success, none if failed
 * @return  0 if success, -1 if failed
 */
int GetCharAfterNthTarget(const std::string& str, char targetChar, int index, char& out);

/**
 * @brief  Get the character before nth target object
 *
 * @param str  Input string to search within.
 * @param targetChar  The target character to find.
 * @param index  The 1-based index of targer.
 * @param out  The character before nth if success, none if failed
 * @return  0 if success, -1 if failed
 */
int GetCharBeforeNthTarget(const std::string& str, char targetChar, int index, char& out);

}; // namespace GeneralUtils

#endif // __GENERAL_UTILS_H__
