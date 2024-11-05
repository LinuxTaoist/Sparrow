/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : MediatorWatch.cpp
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
#include "SprMediatorInterface.h"
#include "InfraCommon.h"
#include "MediatorWatch.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

MediatorWatch theMediatorWatch;

char MediatorWatch::MenuEntry()
{
    char bInput = 0;
    do {
        ShowMQStatus();
        if (bInput == 'M') {
            ShowFieldDetails();
        } else if (bInput == 'S') {
            ShowSignalName();
        } else {
            SPR_LOG("Press 'S' to query signal\n");
            SPR_LOG("Press 'M' to see more \n");
        }

        SPR_LOG("Press 'Q' to back \n");
        char input = InfraWatch::WaitUserInputWithoutEnter();
        bInput = toupper(input);
    } while(bInput != 'Q');

    return bInput;
}

char MediatorWatch::ShowSignalName()
{
    SPR_LOG("\n");
    SPR_LOG("Input Signal Information: \n");
    SPR_LOG("-----------------------------------------------------------------------------------------------\n");
    SPR_LOG("- Input sig id: ");
    int sig = 0;
    InfraWatch::ReadIntFromUserInput(sig);
    SPR_LOG("- Signal %d -> %s\n", sig, SprMediatorInterface::GetInstance()->GetSigalName(sig).c_str());
    SPR_LOG("-----------------------------------------------------------------------------------------------\n");
    SPR_LOG("Press 'S' continue \n");
    return 0;
}

char MediatorWatch::ShowMQStatus()
{
    std::vector<SMQStatus> mqAttrVec;
    SprMediatorInterface::GetInstance()->GetAllMQStatus(mqAttrVec);

    InfraWatch::ClearScreen();
    SPR_LOG("                                   Show All Message Queues                                     \n");
    SPR_LOG("-----------------------------------------------------------------------------------------------\n");
    //        %6d     %5ld  %6ld     %6ld   %8s     %5u   %6u    %5u    %6u     %s\n"
    SPR_LOG(" HANDLE  QLSUM  QMUSED  QCUSED  BLOCK   MLLEN MMUSED MLAST  MTOTAL  QNAME\n");
    SPR_LOG("-----------------------------------------------------------------------------------------------\n");
    for (const auto& mqInfo : mqAttrVec) {
        SPR_LOG("%7d  %5ld  %6ld  %6ld  %s  %ld %6u %5u  %6u  %s\n", mqInfo.handle, mqInfo.mqAttr.mq_maxmsg, mqInfo.maxCount,
                    mqInfo.mqAttr.mq_curmsgs, (mqInfo.mqAttr.mq_flags & O_NONBLOCK) ? "NONBLOCK" : "BLOCK  ",
                    mqInfo.mqAttr.mq_msgsize, mqInfo.maxBytes, mqInfo.lastMsg, mqInfo.total % 100000, mqInfo.mqName);
    }
    SPR_LOG("-----------------------------------------------------------------------------------------------\n");
    return 0;
}

char MediatorWatch::ShowFieldDetails()
{
    SPR_LOG("\n");
    SPR_LOG("Field Descriptions:\n");
    SPR_LOG("-----------------------------------------------------------------------------------------------\n");
    SPR_LOG("- HANDLE: MQ handle identifier                             \n");
    SPR_LOG("- QLSUM : Max message count limit                          \n");
    SPR_LOG("- QMUSED: Peak message count                               \n");
    SPR_LOG("- QCUSED: Current message count                            \n");
    SPR_LOG("- BLOCK : Blocking mode (NONBLOCK/BLOCK)                   \n");
    SPR_LOG("- MLLEN : Max message size limit (bytes)                   \n");
    SPR_LOG("- MMUSED: Peak memory usage (bytes)                        \n");
    SPR_LOG("- MLAST : Last message ID                                  \n");
    SPR_LOG("- MTOTAL: Total messages processed                         \n");
    SPR_LOG("- QNAME : MQ name                                          \n");
    SPR_LOG("-----------------------------------------------------------------------------------------------\n");
    return 0;
}
