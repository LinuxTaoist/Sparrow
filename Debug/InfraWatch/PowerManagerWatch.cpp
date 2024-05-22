/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManagerWatch.cpp
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
#include "PowerManagerInterface.h"
#include "PowerManagerWatch.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

PowerManagerWatch thePowerManagerWatch;

char PowerManagerWatch::MenuEntry()
{
    InfraWatch::ClearScreen();
    SPR_LOG("================================  Power Manager  ================================\n"
            "\n"
            "    1. PowerOn \n"
            "    2. PowerOff \n"
            "\n"
            "    [Q] Quit\n"
            "\n"
            "=================================================================================\n");

    char input = InfraWatch::WaitUserInputWithoutEnter();
    HandleInputInMenu(input);

    return input;
}

char PowerManagerWatch::HandleInputInMenu(char input)
{
    switch(input)
    {
        case '1':
        {
            PowerManagerInterface::GetInstance()->PowerOn();
            break;
        }

        case '2':
        {
            PowerManagerInterface::GetInstance()->PowerOff();
            break;
        }

        case 'q':
        {
            break;
        }

        default:
            break;
    }

    return 0;
}
