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
    std::vector<SMQStatus> mqAttrVec;
    SprMediatorInterface::GetInstance()->GetAllMQStatus(mqAttrVec);

    InfraWatch::ClearScreen();
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
    SPR_LOG("Press 'Q' to back\n");


    char input = InfraWatch::WaitUserInputWithoutEnter();
    HandleInputInMenu(input);

    return input;
}

char MediatorWatch::HandleInputInMenu(char input)
{
    return 0;
}
