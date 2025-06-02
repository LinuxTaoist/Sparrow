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
#include "PowerManagerWatch.h"
#include "OneNetWatch.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

ManagersWatch theManagersWatch;

char ManagersWatch::MenuEntry()
{
    InfraWatch::ClearScreen();

    SPR_LOG("============================   Manager's  Entrance   ============================\n"
            "\n"
            "    1. PowerManager \n"
            "    2. OneNetMqtt   \n"
            "\n"
            "    [Q] Quit \n"
            "\n"
            "=================================================================================\n");

    char input = InfraWatch::WaitUserInputWithoutEnter();
    HandleInputInMenu(input);
    return input;
}

char ManagersWatch::HandleInputInMenu(char input)
{
    switch(input) {
        case '1': {
            CONTINUE_ON_NONQUIT(thePowerManagerWatch.MenuEntry);
            break;
        }
        case '2': {
            CONTINUE_ON_NONQUIT(theOneNetWatch.MenuEntry);
            break;
        }
        case 'q': {
            break;
        }
        default:
            break;
    }

    return 0;
}