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

#ifndef SPR_LOGD
#define SPR_LOGD(...) printf(__VA_ARGS__)
#endif

#ifndef SPR_LOGI
#define SPR_LOGI(...) printf(__VA_ARGS__)
#endif

#ifndef SPR_LOGW
#define SPR_LOGW(...) printf(__VA_ARGS__)
#endif

#ifndef SPR_LOGE
#define SPR_LOGE(...) printf(__VA_ARGS__)
#endif

#endif // __DEFINE_MACRO__