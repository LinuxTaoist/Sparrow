/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PLog.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/06/09
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/06/09 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <string>
#include "PLog.h"

PLog& PLog::GetInstance() {
    static PLog instance;
    return instance;
}

PLog::PLog() : mLevel(PLOG_LEVEL_INFO) {

}

void PLog::SetLevel(PLogLevel level) {
    mLevel = level;
}

PLogLevel PLog::GetLevel() {
    return mLevel;
}

void PLog::RegisterPrintCallback(const PLogCallback& callback) {
    mCallback = callback;
}

void PLog::Print(PLogLevel level, int line, const char* tag, const char* fmt, ...) {
    if (level < mLevel) {
        return;
    }

    std::string levelStr;
    switch (level) {
        case PLOG_LEVEL_DEBUG:
            levelStr = "D";
            break;
        case PLOG_LEVEL_INFO:
            levelStr = "I";
            break;
        case PLOG_LEVEL_WARN:
            levelStr = "W";
            break;
        case PLOG_LEVEL_ERROR:
            levelStr = "E";
            break;
        default:
            levelStr = "U";
            break;
    }

    va_list ap;
    va_start(ap, fmt);

    if (mCallback) {
        mCallback((int)level, line, tag, fmt, ap);
        va_end(ap);
        return;
    }

    printf("%4d %s %s: ", line, tag, levelStr.c_str());
    vprintf(fmt, ap);
    va_end(ap);
}
