/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file SprsigId.h
 *  @author Xiang.D (dx_65535@163.com)
 *  @version 1.0
 *  @brief Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date 2022-06-03
 *
 *  In "signal_list.gen" add the message definition.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2022/06/03 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_SIG_ID_H__
#define __SPR_SIG_ID_H__

namespace InternalDefs {

/* 消息ID转枚举 */
#ifdef ENUM_OR_STRING
#undef ENUM_OR_STRING
#endif
#define ENUM_OR_STRING(x) x

enum ESprSigId
{
    #include "SprSigList.gen"
};

}

/**
 * @brief Get the signal name string
 *
 * @param msgID
 * @return Signal description
 */
const char* GetSigName(int msgID);

#endif
