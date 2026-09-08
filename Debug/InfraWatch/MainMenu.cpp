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
#include <stdio.h>
#include "InfraCommon.h"
#include "ManagersWatch.h"
#include "MediatorWatch.h"
#include "CustomDebugWatch.h"
#include "MainMenu.h"

using namespace InfraWatch;

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

MainMenu& MainMenu::GetInstance() {
    static MainMenu instance;
    return instance;
}

void MainMenu::Usage() {
    ClearScreen();
    SPR_LOG("==================================  MAIN MENU  ==================================\n"
            "\n"
            "    1. Display All Message Queues \n"
            "    2. Manager's Entrance \n"
            "    3. Custom Debug Options \n"
            "\n"
            "    [Q] Quit\n"
            "\n"
            "=================================================================================\n");
}

char MainMenu::Menu(char input) {
    switch(input) {
        case '1': {
            MediatorWatch::GetInstance().Entry();
            break;
        }
        case '2': {
            ManagersWatch::GetInstance().Entry();
            break;
        }
        case '3': {
            CustomDebugWatch::GetInstance().Entry();
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

int MainMenu::MenuLoop() {
    char input = 0;
    do {
        Usage();
        input = WaitUserInputWithoutEnter();
        Menu(input);
    } while (input != 'q' && input != 'Q');

    return 0;
}
