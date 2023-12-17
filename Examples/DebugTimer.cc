/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugTimer.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include "SprTimer.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)
#define SPR_LOGD(fmt, args...) printf("%d DebugTimer D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d DebugTimer E: " fmt, __LINE__, ##args)

int main(int agrc, char* argv[])
{
    // 创建定时器，间隔为1秒，回调函数为TimerCallback
    int count = 0;
    SprTimer timer([&count]() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);

        count++;
        std::stringstream ss;
        ss << "Timer triggered! Current time: "
        << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %X");

        SPR_LOGD("%s count = %d\n", ss.str().c_str(), count);
    });

    SPR_LOGD("Start timer...\n");
    timer.Start(2, 1);              // 延迟2秒开始，周期1s，3次

    while(count < 3);
    timer.UpdateInterval(3);        // 周期调整为3s，2次

    while(count < 6);
    timer.UpdateInterval(0);        // 调整为一次性定时器

    while (getchar() != 'q');
    SPR_LOGD("Stop timer...\n");
    timer.Stop();

    return 0;
}