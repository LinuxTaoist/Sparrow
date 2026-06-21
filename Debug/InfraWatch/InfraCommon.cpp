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
#include <ctype.h>
#include <string>
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

int InfraWatch::ReadIntFromUserInput(int& input)
{
    std::string line;
    if (ReadLineFromUserInput(line) != 0) {
        return -1;
    }

    const char* text = line.c_str();
    char* endPtr = nullptr;
    long result = strtol(text, &endPtr, 10);
    if (endPtr == text) {
        return -1;
    }

    while (*endPtr != '\0' && isspace(static_cast<unsigned char>(*endPtr))) {
        endPtr++;
    }
    if (*endPtr != '\0') {
        return -1;
    }

    input = (int)result;
    return 0;
}

int InfraWatch::ReadLineFromUserInput(std::string& input)
{
    if (!std::getline(std::cin, input)) {
        return -1;
    }

    return 0;
}
