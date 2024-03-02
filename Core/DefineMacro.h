/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DefineNacro.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/03/02
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/02 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __DEFINE_MACRO__
#define __DEFINE_MACRO__

#include "SprLog.h"

// --------------------------------------------------------------------------------------------
// - Log interface macro
// --------------------------------------------------------------------------------------------
#define LOGD(tag, fmt, args...)  SprLog::GetInstance()->d(tag, "%4d: " fmt, __LINE__, ##args)
#define LOGI(tag, fmt, args...)  SprLog::GetInstance()->i(tag, "%4d: " fmt, __LINE__, ##args)
#define LOGW(tag, fmt, args...)  SprLog::GetInstance()->w(tag, "%4d: " fmt, __LINE__, ##args)
#define LOGE(tag, fmt, args...)  SprLog::GetInstance()->e(tag, "%4d: " fmt, __LINE__, ##args)

#endif // __DEFINE_MACRO__