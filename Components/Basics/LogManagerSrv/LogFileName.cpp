/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : LogFileName.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Log file name formatter.
 *  @date       : 2026/09/21
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <cctype>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <time.h>
#include "LogFileName.h"

#define LOG_FILE_TOKEN_BN    "BN" // Base Name
#define LOG_FILE_TOKEN_SI    "SI" // Short Identifier
#define LOG_FILE_TOKEN_MH    "MH" // Monotonic Hour
#define LOG_FILE_TOKEN_ST    "ST" // Start Time
#define LOG_FILE_TOKEN_FX    "FX" // File Extension
#define LOG_FILE_DEFAULT_FORMAT "BN.FX" // Base Name, File Extension

LogFileName::LogFileName(const std::string& fileName,
                                                 const std::string& format)
    : mFormat(format.empty() ? LOG_FILE_DEFAULT_FORMAT : format),
            mFileName(fileName) {
}

int32_t LogFileName::Build(std::string& fileName) const {
    if (mFileName.empty()) {
        return -1;
    }

    Context context;
    if (LoadContext(context) != 0) {
        return -1;
    }

    std::string formatted;
    for (size_t index = 0; index < mFormat.size();) {
        if (index + 1 >= mFormat.size()) {
            formatted.push_back(mFormat[index]);
            ++index;
            continue;
        }

        const std::string token = mFormat.substr(index, 2);
        if (token != LOG_FILE_TOKEN_BN && token != LOG_FILE_TOKEN_SI
            && token != LOG_FILE_TOKEN_MH && token != LOG_FILE_TOKEN_ST
            && token != LOG_FILE_TOKEN_FX) {
            formatted.push_back(mFormat[index]);
            ++index;
            continue;
        }

        const std::string value = GetTokenValue(token, context);
        if (value.empty() && !formatted.empty()
            && (formatted.back() == '.' || formatted.back() == '_'
                || formatted.back() == '-')) {
            formatted.pop_back();
        }
        formatted += value;
        index += 2;
    }

    if (formatted.empty()) {
        return -1;
    }
    fileName = formatted;
    return 0;
}

int32_t LogFileName::LoadContext(Context& context) const {
    context.bootId = GetBootIdText();
    context.monotonicHour = GetMonotonicHourText();
    context.startTime = GetStartTimeText();
    return 0;
}

std::string LogFileName::GetBootIdText() const {
    std::string bootId = "0000";
    std::ifstream bootIdFile("/proc/sys/kernel/random/boot_id");
    std::string bootIdValue;
    if (bootIdFile >> bootIdValue) {
        std::string shortId;
        for (const char value : bootIdValue) {
            if (value == '-') {
                continue;
            }
            shortId.push_back(static_cast<char>(std::toupper(
                static_cast<unsigned char>(value))));
            if (shortId.size() == 4) {
                break;
            }
        }
        if (!shortId.empty()) {
            bootId = shortId;
        }
    }
    return bootId;
}

std::string LogFileName::GetMonotonicHourText() const {
    struct timespec monotonicTime = {};
    if (clock_gettime(CLOCK_MONOTONIC, &monotonicTime) != 0) {
        return "0000";
    }

    const uint64_t elapsedHours = static_cast<uint64_t>(monotonicTime.tv_sec) / 3600;
    std::ostringstream monotonicHour;
    monotonicHour << std::setfill('0') << std::setw(4) << elapsedHours;
    return monotonicHour.str();
}

std::string LogFileName::GetStartTimeText() const {
    std::string startTime = "000000000000";
    const time_t currentTime = time(nullptr);
    struct tm localTime = {};
    if (localtime_r(&currentTime, &localTime) != nullptr
        && localTime.tm_year >= 120) {
        char startTimeBuf[32] = {};
        if (strftime(startTimeBuf, sizeof(startTimeBuf), "%y%m%d%H%M%S", &localTime) != 0) {
            startTime = startTimeBuf;
        }
    }
    return startTime;
}

std::string LogFileName::GetBaseName() const {
    const size_t position = mFileName.rfind('.');
    if (position == std::string::npos || position == 0) {
        return mFileName;
    }
    return mFileName.substr(0, position);
}

std::string LogFileName::GetExtension() const {
    const size_t position = mFileName.rfind('.');
    if (position == std::string::npos || position == 0
        || position + 1 >= mFileName.size()) {
        return std::string();
    }
    return mFileName.substr(position + 1);
}

std::string LogFileName::GetTokenValue(const std::string& token,
                                       const Context& context) const {
    if (token == LOG_FILE_TOKEN_BN) {
        return GetBaseName();
    }
    if (token == LOG_FILE_TOKEN_SI) {
        return context.bootId;
    }
    if (token == LOG_FILE_TOKEN_MH) {
        return context.monotonicHour;
    }
    if (token == LOG_FILE_TOKEN_ST) {
        return context.startTime;
    }
    if (token == LOG_FILE_TOKEN_FX) {
        return GetExtension();
    }
    return std::string();
}
