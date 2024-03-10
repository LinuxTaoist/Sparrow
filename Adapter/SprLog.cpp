/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprLog.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/03/02
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/02 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include <stdarg.h>
#include "SprLog.h"

#define LOG_BUFFER_MAX_SIZE     256

SprLog::SprLog()
{
}

SprLog::~SprLog()
{
}

SprLog* SprLog::GetInstance()
{
    static SprLog instance;
    return &instance;
}

int32_t SprLog::d(const char* tag, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int32_t result = LogImpl("D", tag, format, args);
    va_end(args);

    return result;
}

int32_t SprLog::i(const char* tag, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int32_t result = LogImpl("I", tag, format, args);
    va_end(args);

    return result;
}

int32_t SprLog::w(const char* tag, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int32_t result = LogImpl("W", tag, format, args);
    va_end(args);

    return result;
}

int32_t SprLog::e(const char* tag, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int32_t result = LogImpl("E", tag, format, args);
    va_end(args);

    return result;
}

int32_t SprLog::LogImpl(const char* level, const char* tag, const char* format, va_list args)
{
    char buffer[LOG_BUFFER_MAX_SIZE] = {0};
    int32_t result = vsnprintf(buffer, sizeof(buffer), format, args);
    if (result < 0 || result >= (int32_t)sizeof(buffer)) {
        return -1;
    }

    std::lock_guard<std::mutex> lock(mMutex);
    std::string log = std::string(level) + " " + std::string(tag) + " " + std::string(buffer);
    printf("%s", log.c_str());

    return result;
}
