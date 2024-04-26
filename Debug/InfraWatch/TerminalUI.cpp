/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TerminalUI.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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
#include <limits>
#include <iostream>
#include <stdio.h>
#include "TerminalUI.h"
#include "SprMediatorInterface.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

#define WAIT_UNTIL_QUIT(handle) while((handle()) != 'q')

TerminalUI::TerminalUI()
{
}
TerminalUI::~TerminalUI()
{
}
int TerminalUI::MainMenuLoop()
{
    WAIT_UNTIL_QUIT(DisplayMainMenuAndHandleInput);
    return 0;
}
char TerminalUI::WaitUserInput()
{
    char input;
    SPR_LOG(" : ");
    std::cin >> std::noskipws >> input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return input;
}

void TerminalUI::ClearScreen()
{
    SPR_LOG("\033[2J");
    SPR_LOG("\033[H");
    fflush(stdout);
}
char TerminalUI::DisplayMainMenuAndHandleInput()
{
    ClearScreen();

    SPR_LOG("==================================  MAIN MENU  ==================================\n"
            "\n"
            "    1. Display All Message Queues\n"
            "    2. Display Manager Status \n"
            "    3. Advanced Debug Options \n"
            "\n"
            "    [q] Quit\n"
            "\n"
            "=================================================================================\n");


    char input = WaitUserInput();
    HandleInputInMainMenu(input);

    return input;
}
char TerminalUI::HandleInputInMainMenu(char input)
{
    switch(input) {
        case '1':
        {
            WAIT_UNTIL_QUIT(DisplayMessageQueueStatusAndHandleInput);
            break;
        }

        case '2':
        {
            WAIT_UNTIL_QUIT(DisplayManagerStatusAndHandleInput);
            break;
        }

        case '3':
        {
            WAIT_UNTIL_QUIT(DisplayManagerStatusAndHandleInput);
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

char TerminalUI::DisplayMessageQueueStatusAndHandleInput()
{
    std::vector<SMQStatus> mqAttrVec;
    SprMediatorInterface::GetInstance()->GetAllMQStatus(mqAttrVec);

    ClearScreen();
    SPR_LOG("                                   Show All Message Queues                                     \n");
    SPR_LOG("-----------------------------------------------------------------------------------------------\n");
    //        %6d     %5ld  %6ld     %6ld   %8s     %5u   %6u    %5u    %6u     %s\n"
    SPR_LOG(" HANDLE  QLSUM  QMUSED  QCUSED  BLOCK   MLLEN MMUSED MLAST  MTOTAL  NAME\n");
    SPR_LOG("-----------------------------------------------------------------------------------------------\n");
    for (const auto& mqInfo : mqAttrVec) {
        SPR_LOG("%7d  %5ld  %6ld  %6ld  %s  %ld %6u %5u  %6u  %s\n", mqInfo.handle, mqInfo.mqAttr.mq_maxmsg, mqInfo.maxCount,
                    mqInfo.mqAttr.mq_curmsgs, (mqInfo.mqAttr.mq_flags & O_NONBLOCK) ? "NONBLOCK" : "BLOCK  ",
                    mqInfo.mqAttr.mq_msgsize, mqInfo.maxBytes, mqInfo.lastMsg, mqInfo.total % 100000, mqInfo.mqName);
    }

    SPR_LOG("-----------------------------------------------------------------------------------------------\n");
    SPR_LOG(" Press q to back\n");


    char input = WaitUserInput();
    HandleInputInMessageQueueMenu(input);

    return input;
}

char TerminalUI::HandleInputInMessageQueueMenu(char input)
{
    return 0;
}

char TerminalUI::DisplayManagerStatusAndHandleInput()
{
    ClearScreen();
    SPR_LOG("+---------------------------------------------------------------------+\n");
    SPR_LOG("|                            Manager Debug                            |\n");
    SPR_LOG("+---------------------------------------------------------------------+\n");
    SPR_LOG("| [q] Back                                                            |\n");
    SPR_LOG("+---------------------------------------------------------------------+\n");

    char input = WaitUserInput();
    HandleInputInManagerStatusMenu(input);

    return input;
}

char TerminalUI::HandleInputInManagerStatusMenu(char input)
{
    return 0;
}
