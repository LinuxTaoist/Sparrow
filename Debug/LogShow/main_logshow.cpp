/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_log.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *  @TODO:
 *  The component prints log directly to the library, not through LogManager.
 *  LogManager only used to monitor real-time log status and manage log storage.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include <unistd.h>
#include "SharedRingBuffer.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

int main(int argc, const char *argv[])
{
    // int ret = 0;
    // SharedRingBuffer theLogBuffer(LOG_CACHE_MEMORY_PATH);

    // while (1)
    // {
    //     int32_t len = 0;
    //     ret = theLogBuffer.DumpBuffer(&len, sizeof(int32_t));
    //     if (ret == -1) {
    //         usleep(10000);
    //         continue;
    //     }

    //     std::string logs;
    //     logs.resize(len);
    //     char* data = const_cast<char*>(logs.c_str());
    //     ret = theLogBuffer.DumpBuffer(data, len);
    //     if (ret != 0) {
    //         usleep(10000);
    //         continue;
    //     }

    //     SPR_LOG("%s", logs.c_str());
    // }

    return 0;
}