/**
 *******************************************************************************
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file main_log.cpp
 *  @author Xiang.D (dx_65535@163.com)
 *  @version 1.0
 *  @brief Blog: https://linuxtaoist.gitee.io
 *  @date 2023-11-22
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/22 |  1.0.0.1  | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 *******************************************************************************
 */
#include "LogManager.h"

using namespace std;
using namespace InternalEnum;

int main(int agrc, const char *argv[])
{
    LogManager theLogManager(MODULE_LOGM, "LogM");
    theLogManager.MainLoop();
    return 0;
}
