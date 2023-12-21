/**
 *******************************************************************************
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file main_log.cc
 *  @author Xiang.D (dx_65535@163.com)
 *  @version 1.0
 *  @brief Blog: https://linuxtaoist.gitee.io
 *  @date 2023-11-22
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/11/22 |  1.0.0.1  | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 *******************************************************************************
 */
#include <stdio.h>
#include <memory>
#include "LogManager.h"
#include "SprMediatorIpcProxy.h"

using namespace std;
using namespace InternalEnum;

int main(int agrc, const char *argv[])
{
    LogManager theLogManager(MODULE_LOGM, "LogM", make_shared<SprMediatorIpcProxy>());
    theLogManager.Start();
    return 0;
}