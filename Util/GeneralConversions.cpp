/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : GeneralConversions.cpp
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
#include <string.h>
#include <stdint.h>
#include "GeneralConversions.h"

namespace GeneralConversions
{

std::string HexStringToAscii(const std::string& hexString)
{
    if (hexString.empty() || hexString.size() % 2 != 0) {
        return "";
    }

    std::string result;
    for (size_t i = 0; i < hexString.size(); i += 2) {
        uint8_t high = static_cast<uint8_t>(hexString[i]);
        uint8_t low = static_cast<uint8_t>(hexString[i + 1]);

        if (!((high >= '0' && high <= '9') || (high >= 'A' && high <= 'F') || (high >= 'a' && high <= 'f')) ||
            !((low >= '0' && low <= '9') || (low >= 'A' && low <= 'F') || (low >= 'a' && low <= 'f'))) {
            return "";
        }

        uint8_t byte = 0;
        if (high >= '0' && high <= '9') {
            byte = static_cast<uint8_t>(high - '0') << 4;
        } else if (high >= 'A' && high <= 'F') {
            byte = static_cast<uint8_t>(high - 'A' + 10) << 4;
        } else {
            byte = static_cast<uint8_t>(high - 'a' + 10) << 4;
        }

        if (low >= '0' && low <= '9') {
            byte |= static_cast<uint8_t>(low - '0');
        } else if (low >= 'A' && low <= 'F') {
            byte |= static_cast<uint8_t>(low - 'A' + 10);
        } else {
            byte |= static_cast<uint8_t>(low - 'a' + 10);
        }

        result.push_back(byte);
    }

    return result;
}

int DumpBytesAscall(const std::string& bytes, std::string& out)
{
    const int32_t BYTES_PER_LINE = 16; // Number of bytes per line in the dump.
    int32_t length = bytes.length();

    out.clear();
    for (int32_t i = 0; i < length; i += BYTES_PER_LINE) {
        std::stringstream posBytes, hexBytes, ascallBytes;
        int32_t startByte = i;
        int32_t endByte = std::min(startByte + BYTES_PER_LINE, length);

        // address bytes
        posBytes << std::hex << std::setw(8) << std::setfill('0') << startByte << " ";

        ascallBytes << " |";
        hexBytes << std::hex << std::setfill('0');
        for (int32_t j = 0; j < BYTES_PER_LINE; ++j) {
            if (startByte + j < endByte) {
                hexBytes << std::setw(2) << static_cast<int>(bytes[startByte + j]) << " ";
                ascallBytes << static_cast<char>(bytes[startByte + j]);
            } else {
                hexBytes << "   ";
            }
        }
        ascallBytes << "|";

        out += posBytes.str() + hexBytes.str() + ascallBytes.str() + "\n";
    }

    return 0;
}

}; // namespace GeneralConversions

