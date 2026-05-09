/*--------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CUtils.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/12/19
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/12/19 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "CUtils.h"

namespace CUtils {

ssize_t ReadTextToHexVector(const std::string& path, std::vector<uint8_t>& out) {
    out.clear();
    std::ifstream f(path);
    if (!f) return -1;

    std::stringstream ss;
    ss << f.rdbuf();
    std::string content = ss.str();

    std::string hexChars;
    for (char c : content) {
        if (isxdigit(static_cast<unsigned char>(c))) {
            hexChars += c;
        }
    }

    for (size_t i = 0; i + 1 < hexChars.size(); i += 2) {
        uint8_t byte = static_cast<uint8_t>(std::stoi(hexChars.substr(i, 2), nullptr, 16));
        out.push_back(byte);
    }

    return static_cast<ssize_t>(out.size());
}

int32_t ReadFile(const std::string& path, std::string& str) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        return -1;
    }

    file.seekg(0, std::ios::end);
    int32_t size = static_cast<int32_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    str.resize(size);
    file.read(&str[0], size);
    if (!file.good() && !file.eof()) {
        file.close();
        return -1;
    }

    file.close();
    return size;
}

} // namespace CUtils
