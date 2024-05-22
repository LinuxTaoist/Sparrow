/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : InfraCommon.cpp
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
#include <limits>
#include <iostream>
#include <stdio.h>
#include "GeneralUtils.h"
#include "InfraCommon.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

char InfraWatch::WaitUserInputUntilEnter()
{
    char in;
    SPR_LOG(": ");
    std::cin >> std::noskipws >> in;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return in;
}

char InfraWatch::WaitUserInputWithoutEnter()
{
    char in = 0;
    GeneralUtils::SystemCmd("stty raw");
    in = getchar();
    GeneralUtils::SystemCmd("stty cooked");

    return in;
}

void InfraWatch::ClearScreen()
{
    SPR_LOG("\033[2J");
    SPR_LOG("\033[H");
    fflush(stdout);
}
