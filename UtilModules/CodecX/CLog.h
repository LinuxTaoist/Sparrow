/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CDefine.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/05/06
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/05/06 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __CLOG_H__
#define __CLOG_H__

#include <stdio.h>

#define CLOGD(fmt, args...) printf("%4d %s D: " fmt, __LINE__, CLOG_TAG, ##args)
#define CLOGI(fmt, args...) printf("%4d %s I: " fmt, __LINE__, CLOG_TAG, ##args)
#define CLOGW(fmt, args...) printf("%4d %s W: " fmt, __LINE__, CLOG_TAG, ##args)
#define CLOGE(fmt, args...) printf("%4d %s E: " fmt, __LINE__, CLOG_TAG, ##args)

#endif // __CLOG_H__
