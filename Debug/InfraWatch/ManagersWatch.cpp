/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ManagersWatch.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/23
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/04/23 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include "InfraCommon.h"
#include "ManagersWatch.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

ManagersWatch theManagersWatch;

char ManagersWatch::MenuEntry()
{
    InfraWatch::ClearScreen();
    SPR_LOG("+---------------------------------------------------------------------+\n");
    SPR_LOG("|                            Manager Debug                            |\n");
    SPR_LOG("+---------------------------------------------------------------------+\n");
    SPR_LOG("| [Q] Back                                                            |\n");
    SPR_LOG("+---------------------------------------------------------------------+\n");

    char input = InfraWatch::WaitUserInputWithoutEnter();
    HandleInputInMenu(input);
    return input;
}

char ManagersWatch::HandleInputInMenu(char input)
{
    return 0;
}