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
 * @brief Extracts a value of range bits from a given value and stores it in the result reference.
 *
 * This function takes a value of type T and extracts a range of bits specified by startBit and endBit.
 * The extracted value is then stored in the result reference.
 *
 * @param value The value from which to extract the bits.
 * @param startBit The starting position (inclusive) of the bit range. Counting starts from 0.
 * @param endBit The ending position (inclusive) of the bit range. Counting starts from 0.
 * @param result A reference to a variable of type T where the extracted bit - range value will be stored.
 * @return Returns true if the extraction is successful (i.e., the input parameters are valid), false otherwise.
 *
 * startBit and endBit are counted starting from 0. The function checks if the input parameters
 * are within the valid range for the type T. If they are not, the function returns false.
 */
template <typename T>
bool GetValueInRange(T value, int startBit, int endBit, T& result) {
    if (startBit < 0 || endBit < 0 || startBit > endBit ||
        startBit >= (int)sizeof(T) * 8 || endBit >= (int)sizeof(T) * 8) {
        return false;
    }

    T mask = ((T(1) << (endBit - startBit + 1)) - 1) << startBit;
    result = (value & mask) >> startBit;
    return true;
}

}; // namespace BitAlgorithms

#endif // __BIT_ALGORITHMS_H__
