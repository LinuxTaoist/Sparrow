/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprLog.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <sys/time.h>
#include "CommonMacros.h"
#include "SharedRingBuffer.h"
#include "SprLog.h"

#define PID_PRINT_WIDTH_LIMIT       6
#define LOG_BUFFER_SIZE_LIMIT       512
#define SEMAPHORE_NAME              "/SprLogSem"

static SharedRingBuffer* pLogSCacheMem = nullptr;

// Defined with the same as CoreTypeDefs.h
enum ELogLevel
{
    LOG_LEVEL_MIN   = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_BUTT
};

SprLog::SprLog()
{
    mWriteSem = sem_open(SEMAPHORE_NAME, O_CREAT, 0644, 1);
    if (SEM_FAILED == mWriteSem) {
        perror("sem_open failed");
    }

    mPrintLevel = LOG_LEVEL_BUTT;
    pLogSCacheMem = new (std::nothrow) SharedRingBuffer(LOG_CACHE_MEMORY_PATH);
}

SprLog::~SprLog()
{
    // refer comment in SprLog::GetInstance()
    // if (SEM_FAILED != mWriteSem) {
    //     sem_close(mWriteSem);
    //     sem_unlink(SEMAPHORE_NAME);
    // }

    // if (pLogSCacheMem != nullptr) {
    //     delete pLogSCacheMem;
    //     pLogSCacheMem = nullptr;
    // }
}

SprLog* SprLog::GetInstance()
{
    // never delete this instance
    static SprLog *instance = new (std::nothrow) SprLog();
    return instance;
}

int32_t SprLog::SetLevel(int32_t level)
{
    mPrintLevel = level;
    return 0;
}

int32_t SprLog::GetLevel()
{
    return mPrintLevel;
}

int32_t SprLog::d(const char* tag, const char* format, ...)
{
    if (mPrintLevel < LOG_LEVEL_DEBUG) {
        return 0;
    }

    va_list args;
    va_start(args, format);
    int32_t result = LogImpl("D", tag, format, args);
    va_end(args);

    return result;
}

int32_t SprLog::i(const char* tag, const char* format, ...)
{
    if (mPrintLevel < LOG_LEVEL_INFO) {
        return 0;
    }

    va_list args;
    va_start(args, format);
    int32_t result = LogImpl("I", tag, format, args);
    va_end(args);

    return result;
}

int32_t SprLog::w(const char* tag, const char* format, ...)
{
    if (mPrintLevel < LOG_LEVEL_WARN) {
        return 0;
    }

    va_list args;
    va_start(args, format);
    int32_t result = LogImpl("W", tag, format, args);
    va_end(args);

    return result;
}

int32_t SprLog::e(const char* tag, const char* format, ...)
{
    if (mPrintLevel < LOG_LEVEL_ERROR) {
        return 0;
    }

    va_list args;
    va_start(args, format);
    int32_t result = LogImpl("E", tag, format, args);
    va_end(args);

    return result;
}

static std::string GetCurrentTimestamp()
{
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) != 0) {
        perror("gettimeofday");
        return "null";
    }

    struct tm local_tm;
    localtime_r(&tv.tv_sec, &local_tm);

    char buffer[30] = {0};
    strftime(buffer, sizeof(buffer), "%m-%d %H:%M:%S.", &local_tm);

    // millisecond
    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "%.3ld", tv.tv_usec / 1000);
    return buffer;
}

// 04-03 07:56:23.032  43930     DebugMsg D:
static int FormatLog(std::string& log, const char* level, const char* tag, const char* buffer)
{
    __pid_t pid = getpid();
    std::ostringstream oss;
    std::string timestamp = GetCurrentTimestamp();

    oss << timestamp;
    oss << " " << std::right << std::setw(PID_PRINT_WIDTH_LIMIT) << pid;
    oss << " " << std::left << std::setw(TAG_PRINT_WIDTH_LIMIT) << tag;
    oss << " " << level;
    oss << ": " << buffer;

    bool hasNewline = std::any_of(buffer, buffer + strlen(buffer), [](char c){ return c == '\n'; });
    if (!hasNewline) {
        oss << "\n";
    }

    log += oss.str();
    return 0;
}

int32_t SprLog::LogImpl(const char* level, const char* tag, const char* format, va_list args)
{
    char buffer[LOG_BUFFER_SIZE_LIMIT] = {0};
    int32_t result = vsnprintf(buffer, sizeof(buffer), format, args);
    if (result < 0 || result >= (int32_t)sizeof(buffer)) {
        char prefix[11] = {0};
        memcpy(prefix, buffer, 10);
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer),
            "%s...... [TRUNCATED] LEN:%d >= LIMIT:%zu [LOG CONTENT TRUNCATED]",
            prefix, result, sizeof(buffer));
        result = -1;
    }

    std::string log;
    FormatLog(log, level, tag, buffer);
    sem_wait(mWriteSem);
    LogsToMemory(log.c_str(), log.length());
    sem_post(mWriteSem);

    return result;
}

int32_t SprLog::LogsToMemory(const char* logs, int32_t len)
{
    unsigned char buffer[len + sizeof(int32_t)] = {0};

    memcpy(buffer, &len, sizeof(int32_t));
    memcpy(buffer + sizeof(int32_t), logs, len);
    return pLogSCacheMem->Write(buffer, sizeof(int32_t) + len);
}
