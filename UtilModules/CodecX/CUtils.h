/*--------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CUtils.h
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
#ifndef __CUTILS_H__
#define __CUTILS_H__

#include <string>
#include <vector>
#include <type_traits>
#include <algorithm>
#include <limits>
#include "CDefine.h"

namespace CUtils {

/**
 * @brief  string（输入容器）转换为单个整数
 * @param [in]  in          待转换的输入string（源容器）
 * @param [out] out         输出整数
 * @param [in]  type        输入string的字节序
 * @param [in]  inOffset    输入string的起始字节偏移（基于源容器）
 * @param [in]  inMaxLen    输入string中从inOffset开始的最大转换字节数（0表示读取剩余所有字节）
 * @return int32_t          成功返回实际读取的字节数，失败返回-1
 */
template <typename T>
int32_t SToI(const std::string& in, T& out, CEndianType type = CENDIAN_BIG, int32_t inOffset = 0, int32_t inMaxLen = 0) {
    static_assert(std::is_integral<T>::value && !std::is_same<T, bool>::value, "T must be non-bool integer");
    constexpr size_t tSize = sizeof(T);
    const size_t srcOff = static_cast<size_t>(inOffset);

    if (inOffset < 0 || inMaxLen < 0 || srcOff >= in.size()) {
        out = 0;
        return -1;
    }

    const size_t remainBytes = in.size() - srcOff;
    const size_t readLen = (inMaxLen == 0) ? remainBytes : std::min(static_cast<size_t>(inMaxLen), remainBytes);

    if (readLen == 0) {
        out = 0;
        return 0;
    }

    if (readLen > tSize) {
        out = 0;
        return -1;
    }

    using U = typename std::make_unsigned<T>::type;
    U temp = 0;
    for (size_t i = 0; i < readLen; ++i) {
        const uint8_t byte = static_cast<uint8_t>(in[srcOff + i]);
        if (type == CENDIAN_BIG) {
            temp = (temp << 8) | byte;
        } else {
            temp |= static_cast<U>(byte) << (8 * i);
        }
    }

    constexpr U tMax = static_cast<U>(std::numeric_limits<T>::max());
    constexpr U tMin = std::is_signed<T>::value ? static_cast<U>(std::numeric_limits<T>::min()) : 0;
    if (std::is_signed<T>::value) {
        if ((readLen == tSize && temp > tMax && temp < tMin) || (readLen < tSize && temp > tMax)) {
            out = 0;
            return -1;
        }
    } else if (temp > tMax) {
        out = 0;
        return -1;
    }

    out = static_cast<T>(temp);
    return static_cast<int32_t>(readLen);
}

/**
 * @brief  string（输入容器）转换为整数vector
 * @param [in]  in          待转换的输入string（源容器）
 * @param [out] out         输出整数vector
 * @param [in]  type        输入string的字节序
 * @param [in]  inOffset    输入string的起始字节偏移（基于源容器）
 * @param [in]  inMaxLen    输入string中从inOffset开始的最大转换字节数（0表示不限制）
 * @return int32_t          成功返回实际转换的字节数，失败返回-1
 */
template <typename T>
int32_t SToV(const std::string& in, std::vector<T>& out, CEndianType type = CENDIAN_BIG, int32_t inOffset = 0, int32_t inMaxLen = 0) {
    static_assert(std::is_integral<T>::value && !std::is_same<T, bool>::value, "T must be non-bool integer");
    constexpr size_t elemSize = sizeof(T);
    const size_t srcOff = static_cast<size_t>(inOffset);

    if (inOffset < 0 || inMaxLen < 0 || srcOff >= in.size()) {
        return -1;
    }

    size_t remainBytes = in.size() - srcOff;
    if (inMaxLen > 0) {
        remainBytes = std::min(remainBytes, static_cast<size_t>(inMaxLen));
    }

    if (remainBytes == 0) {
        return 0;
    }

    if (remainBytes < elemSize) {
        out.clear();
        return -1;
    }

    out.reserve(out.size() + remainBytes / elemSize);

    int32_t convertedBytes = 0;
    size_t currOff = srcOff;
    while (currOff + elemSize <= srcOff + remainBytes) {
        T elem = 0;
        int32_t ret = SToI(in, elem, type, static_cast<int32_t>(currOff), static_cast<int32_t>(elemSize));
        if (ret != static_cast<int32_t>(elemSize)) {
            out.clear();
            return -1;
        }
        out.push_back(elem);
        currOff += elemSize;
        convertedBytes += ret;
    }

    return convertedBytes;
}

/**
 * @brief  单个整数转换为string
 * @param [in]  in          输入整数
 * @param [out] out         输出string
 * @param [in]  type        输出string的字节序
 * @return int32_t          成功返回sizeof(T)，失败返回-1
 */
template <typename T>
int32_t IToS(const T& in, std::string& out, CEndianType type = CENDIAN_BIG) {
    static_assert(std::is_integral<T>::value && !std::is_same<T, bool>::value, "T must be non-bool integer");
    constexpr size_t elemSize = sizeof(T);
    out.clear();
    out.reserve(elemSize);

    using U = typename std::make_unsigned<T>::type;
    U temp = static_cast<U>(in);
    for (size_t i = 0; i < elemSize; ++i) {
        uint8_t byte = 0;
        if (type == CENDIAN_BIG) {
            byte = static_cast<uint8_t>((temp >> (8 * (elemSize - i - 1))) & 0xFF);
        } else {
            byte = static_cast<uint8_t>((temp >> (8 * i)) & 0xFF);
        }
        out.push_back(static_cast<char>(byte));
    }

    return static_cast<int32_t>(elemSize);
}

/**
 * @brief  整数转换为vector<U>
 * @param [in]  in          待转换的整数
 * @param [out] out         输出vector
 * @param [in]  type        转换字节序
 * @return int32_t          成功返回实际转换的字节数，失败返回-1
 */
template <typename T, typename U>
int32_t IToV(const T& in, std::vector<U>& out, CEndianType type = CENDIAN_BIG) {
    static_assert(std::is_integral<T>::value && !std::is_same<T, bool>::value, "T must be non-bool integer");
    static_assert(std::is_integral<U>::value && !std::is_same<U, bool>::value, "U must be non-bool integer");

    constexpr size_t srcSize = sizeof(T);
    constexpr size_t dstSize = sizeof(U);

    using SrcUType = typename std::make_unsigned<T>::type;
    const SrcUType u_val = static_cast<SrcUType>(in);
    uint8_t srcBytes[srcSize] = {0};

    for (size_t i = 0; i < srcSize; ++i) {
        size_t shift = (type == CENDIAN_BIG) ? (srcSize - 1 - i) * 8 : i * 8;
        srcBytes[i] = static_cast<uint8_t>((u_val >> shift) & 0xFF);
    }

    const size_t dstCount = srcSize / dstSize;
    out.reserve(out.size() + dstCount);

    for (size_t i = 0; i < dstCount; ++i) {
        U dstElem = 0;
        for (size_t j = 0; j < dstSize; ++j) {
            size_t byteIdx = i * dstSize + j;
            size_t shift = (type == CENDIAN_BIG) ? (dstSize - 1 - j) * 8 : j * 8;
            dstElem |= static_cast<U>(srcBytes[byteIdx]) << shift;
        }
        out.push_back(dstElem);
    }

    return static_cast<int32_t>(srcSize);
}

/**
 * @brief  字节vector转换为整数
 * @param [in]  in          待转换的字节vector（源容器，需整数类型）
 * @param [out] out         输出整数
 * @param [in]  type        转换字节序
 * @param [in]  inOffset    输入vector的起始字节偏移（基于源容器，即vector转字节流后的偏移）
 * @param [in]  inMaxLen    输入vector中从inOffset开始的最大转换字节数（0表示读取剩余所有字节）
 * @return int32_t          成功返回实际读取的字节数，失败返回-1
 */
template <typename T, typename U>
int32_t VToI(const std::vector<T>& in, U& out, CEndianType type = CENDIAN_BIG, int32_t inOffset = 0, int32_t inMaxLen = 0) {
    static_assert(std::is_integral<T>::value && !std::is_same<T, bool>::value, "T must be non-bool integer");
    static_assert(std::is_integral<U>::value && !std::is_same<U, bool>::value, "U must be non-bool integer");

    constexpr size_t tSize = sizeof(T);
    constexpr size_t uSize = sizeof(U);
    const size_t srcOff = static_cast<size_t>(inOffset);

    if (inOffset < 0 || inMaxLen < 0) {
        out = 0;
        return -1;
    }

    const size_t totalBytes = in.size() * tSize;
    if (srcOff >= totalBytes) {
        out = 0;
        return -1;
    }

    const size_t remainBytes = totalBytes - srcOff;
    const size_t readLen = (inMaxLen == 0) ? remainBytes : std::min(static_cast<size_t>(inMaxLen), remainBytes);

    if (readLen == 0) {
        out = 0;
        return 0;
    }

    if (readLen > uSize) {
        out = 0;
        return -1;
    }

    std::vector<uint8_t> srcBytes;
    srcBytes.reserve(readLen);
    for (size_t i = 0; i < readLen; ++i) {
        const size_t globalByteIdx = srcOff + i;
        const size_t elemIdx = globalByteIdx / tSize;
        const size_t byteInElemIdx = globalByteIdx % tSize;

        using UnsignedT = typename std::make_unsigned<T>::type;
        const UnsignedT tVal = static_cast<UnsignedT>(in[elemIdx]);
        uint8_t byte = 0;
        if (type == CENDIAN_BIG) {
            byte = static_cast<uint8_t>((tVal >> (8 * (tSize - 1 - byteInElemIdx))) & 0xFF);
        } else {
            byte = static_cast<uint8_t>((tVal >> (8 * byteInElemIdx)) & 0xFF);
        }
        srcBytes.push_back(byte);
    }

    using UnsignedU = typename std::make_unsigned<U>::type;
    UnsignedU temp = 0;
    for (size_t i = 0; i < readLen; ++i) {
        if (type == CENDIAN_BIG) {
            temp = (temp << 8) | srcBytes[i];
        } else {
            temp |= static_cast<UnsignedU>(srcBytes[i]) << (8 * i);
        }
    }

    constexpr UnsignedU uMax = static_cast<UnsignedU>(std::numeric_limits<U>::max());
    constexpr UnsignedU uMin = std::is_signed<U>::value ? static_cast<UnsignedU>(std::numeric_limits<U>::min()) : 0;
    if (std::is_signed<U>::value) {
        if ((readLen == uSize && temp > uMax && temp < uMin) || (readLen < uSize && temp > uMax)) {
            out = 0;
            return -1;
        }
    } else if (temp > uMax) {
        out = 0;
        return -1;
    }

    out = static_cast<U>(temp);
    return static_cast<int32_t>(readLen);
}

/**
 * @brief  整数vector（输入容器）转换为string
 * @param [in]  in              待转换的整数vector（源容器）
 * @param [out] out             输出string
 * @param [in]  type            转换字节序
 * @param [in]  inOffset        输入vector的起始字节偏移（基于源容器，即vector转字节流后的偏移）
 * @param [in]  inMaxLen        输入vector中从inOffset开始的最大转换字节数（0表示不限制）
 * @param [in]  strWriteOffset  输出string的写入起始偏移（默认-1=追加到末尾）
 * @return int32_t              成功返回实际转换的字节数，失败返回-1
 */
template <typename T>
int32_t VToS(const std::vector<T>& in, std::string& out, CEndianType type = CENDIAN_BIG,
             int32_t inOffset = 0, int32_t inMaxLen = 0, int32_t strWriteOffset = -1) {
    static_assert(std::is_integral<T>::value && !std::is_same<T, bool>::value, "T must be non-bool integer");
    if (inOffset < 0 || inMaxLen < 0) {
        return -1;
    }

    const size_t writeOff = (strWriteOffset < 0) ? out.size() : static_cast<size_t>(strWriteOffset);
    if (writeOff > out.size()) {
        return -1;
    }

    constexpr size_t elemSize = sizeof(T);
    std::vector<uint8_t> srcBytes;
    srcBytes.reserve(in.size() * elemSize);
    std::string tempStr;
    for (const auto& elem : in) {
        IToS(elem, tempStr, type);
        for (char c : tempStr) {
            srcBytes.push_back(static_cast<uint8_t>(c));
        }
    }

    const size_t srcTotalBytes = srcBytes.size();
    const size_t srcOff = static_cast<size_t>(inOffset);
    if (srcOff >= srcTotalBytes) {
        return -1;
    }

    size_t remainSrcBytes = srcTotalBytes - srcOff;
    if (inMaxLen > 0) {
        remainSrcBytes = std::min(remainSrcBytes, static_cast<size_t>(inMaxLen));
    }

    if (remainSrcBytes == 0) {
        return 0;
    }

    if (writeOff + remainSrcBytes > out.size()) {
        out.resize(writeOff + remainSrcBytes, '\0');
    }

    for (size_t i = 0; i < remainSrcBytes; ++i) {
        out[writeOff + i] = static_cast<char>(srcBytes[srcOff + i]);
    }

    return static_cast<int32_t>(remainSrcBytes);
}

/**
 * @brief  vector<T> 转换为 vector<U>
 * @param [in]  in          待转换的vector<T>
 * @param [out] out         输出的vector<U>
 * @param [in]  type        转换字节序
 * @param [in]  inOffset    输入vector的起始字节偏移（基于源容器，即vector转字节流后的偏移）
 * @param [in]  inMaxLen    输入vector中从inOffset开始最大转换字节数（0表示不限制）
 * @return int32_t          成功返回实际转换的字节数，失败返回-1
 */
template <typename T, typename U>
int32_t VToV(const std::vector<T>& in, std::vector<U>& out, CEndianType type = CENDIAN_BIG,
            int32_t inOffset = 0, int32_t inMaxLen = 0) {
    static_assert(std::is_integral<T>::value && !std::is_same<T, bool>::value, "T must be non-bool integer");
    static_assert(std::is_integral<U>::value && !std::is_same<U, bool>::value, "U must be non-bool integer");

    constexpr size_t uElemSize = sizeof(U);
    if (inOffset < 0 || inMaxLen < 0) {
        return -1;
    }

    std::vector<uint8_t> srcBytes;
    srcBytes.reserve(in.size() * sizeof(T));
    for (const auto& elem : in) {
        std::vector<uint8_t> tempBytes;
        if (IToV(elem, tempBytes, type) < 0) {
            return -1;
        }
        srcBytes.insert(srcBytes.end(), tempBytes.begin(), tempBytes.end());
    }

    const size_t srcTotalBytes = srcBytes.size();
    const size_t srcOff = static_cast<size_t>(inOffset);
    if (srcOff > srcTotalBytes) {
        return -1;
    }

    size_t remainBytes = srcTotalBytes - srcOff;
    if (inMaxLen > 0) {
        remainBytes = std::min(remainBytes, static_cast<size_t>(inMaxLen));
    }

    if (remainBytes == 0) {
        return 0;
    }

    const size_t existingElemCount = out.size();
    const size_t newElemCount = (remainBytes + uElemSize - 1) / uElemSize;
    out.reserve(existingElemCount + newElemCount);

    int32_t convertedBytes = 0;
    for (size_t i = 0; i < newElemCount; ++i) {
        U elem = 0;
        const size_t elemStartByte = srcOff + i * uElemSize;
        size_t readBytes = std::min(uElemSize, remainBytes - i * uElemSize);
        int32_t ret = CUtils::VToI(srcBytes, elem, type, static_cast<int32_t>(elemStartByte), static_cast<int32_t>(readBytes));
        if (ret < 0) {
            out.resize(existingElemCount);
            return -1;
        }
        out.push_back(elem);
        convertedBytes += ret;
    }

    return convertedBytes;
}

}; // namespace CUtils

#endif // __CUTILS_H__
