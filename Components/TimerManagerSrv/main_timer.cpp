/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_sparrow.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */

#include <stdio.h>

#define SPR_LOGD(fmt, args...) printf("%d Timer D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d Timer E: " fmt, __LINE__, ##args)

int main(int agrc, const char *argv[])
{
    return 0;
}