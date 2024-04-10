/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Shared.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <random>
#include <chrono>
#include <random>
#include "GeneralUtils.h"

namespace GeneralUtils {
    std::string ProduceRandomStr(int len)
    {
        std::string strRandom;
        const std::string seedStr = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> distribution(0, seedStr.size() - 1);

        for (int i = 0; i < len; i++) {
            int randomValue = distribution(generator);
            strRandom += seedStr[randomValue];
        }

        return strRandom;
    }

    int ProduceRandomInt(int len)
    {
        int maxValue = pow(10, len) - 1;
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distribution(0, maxValue);

        return distribution(generator);
    }

    std::string GetCurTimeStr()
    {
        struct timespec currentTime;
        clock_gettime(CLOCK_REALTIME, &currentTime);

        time_t seconds = currentTime.tv_sec;
        struct tm* localTime = localtime(&seconds);
        int milliseconds = currentTime.tv_nsec / 1000000;

        char buffer[80];
        sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
                localTime->tm_hour, localTime->tm_min, localTime->tm_sec, milliseconds);

        return std::string(buffer);
    }
};
