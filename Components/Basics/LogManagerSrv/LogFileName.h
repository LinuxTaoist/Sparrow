/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : LogFileName.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Log file name formatter.
 *  @date       : 2026/09/21
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __LOG_FILE_NAME_H__
#define __LOG_FILE_NAME_H__

#include <string>
#include <stdint.h>

class LogFileName {
public:
    LogFileName(const std::string& fileName,
                const std::string& format);
    ~LogFileName() = default;

    int32_t Build(std::string& fileName) const;

private:
    struct Context {
        std::string bootId;         // Short Identifier
        std::string monotonicHour;  // Monotonic Hour
        std::string startTime;      // Start Time
    };

    int32_t LoadContext(Context& context) const;
    std::string GetBootIdText() const;
    std::string GetMonotonicHourText() const;
    std::string GetStartTimeText() const;
    std::string GetBaseName() const;
    std::string GetExtension() const;
    std::string GetTokenValue(const std::string& token,
                              const Context& context) const;

private:
    std::string mFormat;
    std::string mFileName;
};

#endif // __LOG_FILE_NAME_H__
