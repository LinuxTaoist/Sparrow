/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : MainMenu.cpp
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
#include <iostream>
#include <stdio.h>
#include "InfraCommon.h"
#include "ManagersWatch.h"
#include "MediatorWatch.h"
#include "CustomDebugWatch.h"
#include "MainMenu.h"

using namespace InfraWatch;

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

MainMenu theMainMenu;

char MainMenu::MenuEntry()
{
    ClearScreen();

    SPR_LOG("==================================  MAIN MENU  ==================================\n"
            "\n"
            "    1. Display All Message Queues \n"
            "    2. Display Managers Status \n"
            "    3. Display Custom Debug Options \n"
            "\n"
            "    [Q] Quit\n"
            "\n"
            "=================================================================================\n");

    char input = WaitUserInputWithoutEnter();
    HandleInputInMenu(input);

    return input;
}

char MainMenu::HandleInputInMenu(char input)
{
    switch(input) {
        case '1':
        {
            CONTINUE_ON_NONQUIT(theMediatorWatch.MenuEntry);
            break;
        }

        case '2':
        {
            CONTINUE_ON_NONQUIT(theManagersWatch.MenuEntry);
            break;
        }

        case '3':
        {
            CONTINUE_ON_NONQUIT(theCustomDebugWatch.MenuEntry);
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

int MainMenu::MenuLoop()
{
    CONTINUE_ON_NONQUIT(MenuEntry);
    return 0;
}
