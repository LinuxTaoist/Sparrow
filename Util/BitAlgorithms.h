/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BitAlgorithms.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/03
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/03 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __BIT_ALGORITHMS_H__
#define __BIT_ALGORITHMS_H__

namespace BitAlgorithms {
/**
 * @brief Extracts a value of range bits from a given value
 *
 * @param value The value from which to extract the bits
 * @param startBit The starting position (inclusive) of the bit range.
 * @param endBit The ending position (inclusive) of the bit range.
 * @return Returns the extracted bits as a value of type T.
 *
 * @Note startBit and endBit are counted starting from 0.
 */
template <typename T>
T GetValueInRange(T value, int startBit, int endBit) {
    T mask = (1ULL << (endBit + 1)) - 1;
    T valueInRange = (value & mask) >> startBit;
    return valueInRange;
}

}; // namespace BitAlgorithms

#endif // __BIT_ALGORITHMS_H__
