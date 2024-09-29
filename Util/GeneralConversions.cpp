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
#include <cstdint>
#include "GeneralConversions.h"

namespace GeneralConversions
{

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

