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
#include <stdio.h>
#include <stdint.h>
#include "SprSigId.h"

#ifdef ENUM_OR_STRING
#undef ENUM_OR_STRING
#endif
#define ENUM_OR_STRING(x) #x

const int32_t MAX_LENGTH_MSG = 50;
const char sigIdString[][MAX_LENGTH_MSG] = {
    #include "SprSigList.gen"
};

const char* GetSigName(uint32_t msgID)
{
    if (msgID < InternalDefs::SIG_ID_PUBLIC_END) {
        return sigIdString[msgID];
    }

    static char warnText[64] = {};
    snprintf(warnText, sizeof(warnText), "SIGID_0X%X", msgID);
    return warnText;
}
