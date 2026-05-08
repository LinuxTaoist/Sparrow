/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : FileUtils.h
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
#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace FileUtils {

#define MAX_FILE_SIZE 10240     // 10KB

template <typename T>
T SwapEndian(T val) {
    auto* p = reinterpret_cast<uint8_t*>(&val);
    std::reverse(p, p + sizeof(T));
    return val;
}

template <typename T>
ssize_t ReadFileToVector(const std::string& path, std::vector<T>& out, bool isBigEndian = true) {
    out.clear();
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        return -1;
    }

    f.seekg(0, std::ios::end);
    size_t fileSize = f.tellg();
    f.seekg(0);

    if (fileSize > MAX_FILE_SIZE) {
        return -1;
    }

    if (fileSize == 0) {
        return 0;
    }

    constexpr size_t tSize = sizeof(T);
    out.resize(fileSize / tSize);
    f.read(reinterpret_cast<char*>(out.data()), fileSize);
    if (!f) {
        return -1;
    }

    if (isBigEndian) {
        for (auto& v : out) v = SwapEndian(v);
    }

    return static_cast<ssize_t>(fileSize);
}

template <typename T>
ssize_t ReadCharTextToHexVector(const std::string& path, std::vector<T>& out, bool isBigEndian = true) {
    out.clear();
    std::ifstream f(path);
    if (!f) {
        return -1;
    }

    std::stringstream ss;
    ss << f.rdbuf();
    std::string content = ss.str();
    if (content.size() > MAX_FILE_SIZE) {
        return -1;
    }

    std::string hexBytes;
    for (char c : content) {
        if (isxdigit(c)) {
            hexBytes += c;
        }
    }

    constexpr size_t charPerNum = sizeof(T) * 2;
    for (size_t i = 0; i + charPerNum <= hexBytes.size(); i += charPerNum) {
        std::string part = hexBytes.substr(i, charPerNum);
        T val = static_cast<T>(stoull(part, nullptr, 16));
        if (!isBigEndian) {
            val = SwapEndian(val);
        }

        out.push_back(val);
    }

    return static_cast<ssize_t>(out.size() * sizeof(T));
}

} // namespace FileUtils

#endif // __FILE_UTILS_H__
