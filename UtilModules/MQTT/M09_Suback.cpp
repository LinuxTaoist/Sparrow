/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : M09_Suback.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/20
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/20 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "M09_Suback.h"

Suback::Suback() : mIdentifier(0), mReturnCode(0)
{
}

Suback::~Suback()
{
}

int32_t Suback::DecodeVariableHeader(const std::string& bytes)
{
    int len = DecodeIntegerFromBytes(mIdentifier, bytes);
    CHECK_RESULT(len);
    return len;
}

int32_t Suback::DecodePayload(const std::string& bytes)
{
    int len = DecodeIntegerFromBytes(mReturnCode, bytes);
    CHECK_RESULT(len);
    return len;
}

