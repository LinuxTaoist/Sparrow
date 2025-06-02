/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetWatch.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/24
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/24 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "InfraCommon.h"
#include "OneNetInterface.h"
#include "OneNetWatch.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

OneNetWatch theOneNetWatch;

char OneNetWatch::MenuEntry()
{
    InfraWatch::ClearScreen();
    SPR_LOG("============================   Manager's  Entrance   ============================\n"
            "\n"
            "    1. Active PC_TEST_01 \n"
            "    2. Active PC_TEST_02 \n"
            "    3. Deactive device   \n"
            "\n"
            "    [Q] Quit \n"
            "\n"
            "=================================================================================\n");

    bool ready = OneNetInterface::GetInstance()->IsReady();
    if (!ready) {
        SPR_LOG(" [!] OneNetMqtt binder connection failed! Please try again or exit.\n");
    }

    char input = InfraWatch::WaitUserInputWithoutEnter();
    HandleInputInMenu(input);
    return input;
}

char OneNetWatch::HandleInputInMenu(char input)
{
    switch(input) {
        case '1': {
            OneNetInterface::GetInstance()->ActiveDevice("PC_TEST_01");
            break;
        }
        case '2': {
            OneNetInterface::GetInstance()->ActiveDevice("PC_TEST_02");
            break;
        }
        case '3': {
            OneNetInterface::GetInstance()->DeactiveDevice();
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