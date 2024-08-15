/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file SprsigId.cpp
 *  @author Xiang.D (dx_65535@163.com)
 *  @version 1.0
 *  @brief Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date 2022-06-03
 *
 *  In "signal_list.gen" add the message definition.
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2022/06/03 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <string>
#include <vector>
#include "SprSigId.h"

#ifdef ENUM_OR_STRING
#undef ENUM_OR_STRING
#endif
#define ENUM_OR_STRING(x) #x

const int MAX_LENGTH_MSG = 50;
const char sigIdString[][MAX_LENGTH_MSG] = {
    #include "SprSigList.gen"
};

const char* GetSigName(int msgID)
{
    return (msgID < InternalDefs::SIG_ID_BUTT) ? sigIdString[msgID] : "UNDEFINED";
}
