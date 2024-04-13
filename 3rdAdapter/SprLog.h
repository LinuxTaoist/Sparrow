/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprLog.h
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
#ifndef __SPR_LOG_H__
#define __SPR_LOG_H__

#include <stdarg.h>
#include <stdint.h>
#include <semaphore.h>

class SprLog
{
public:
    ~SprLog();

    /**
     * @brief Get the Instance object
     *
     * @return SprLog*
     */
    static SprLog* GetInstance();

    // --------------------------------------------------------------------------------------------
    // - External interfaces for printing logs
    // --------------------------------------------------------------------------------------------
    int32_t d(const char* tag, const char* format, ...);
    int32_t i(const char* tag, const char* format, ...);
    int32_t w(const char* tag, const char* format, ...);
    int32_t e(const char* tag, const char* format, ...);

private:
    SprLog();

    /**
     * @brief LogImpl
     *
     * @param level     log level
     * @param tag       Tag of logs
     * @param format    Format of the output logs string
     * @param args      Variable parameters
     * @return          0 on success, or -1 if an error occurred
     *
     * The interface for printing logs can be connected to other log interfaces
     */
    int32_t LogImpl(const char* level, const char* tag, const char* format, va_list args);

    /**
     * @brief Writes a log string into shared memory.
     * @param logs      The log data to be stored.
     * @return          -1 if an error occurred,
     *                  Otherwise, returns the number of bytes written.
     */
    int32_t LogsToMemory(const char* logs, int32_t len);

private:
    sem_t mWriteSem;
};

#endif //__SPR_LOG_H__
