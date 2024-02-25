/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugTimer.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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
#include <stdio.h>
#include "SprTimerManager.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)
#define SPR_LOGD(fmt, args...) printf("%d DebugTimer D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d DebugTimer E: " fmt, __LINE__, ##args)

int main(int agrc, char* argv[])
{
    return 0;
}