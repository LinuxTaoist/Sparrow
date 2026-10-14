/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : LogSink.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Log output sink.
 *  @date       : 2026/09/20
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __LOG_SINK_H__
#define __LOG_SINK_H__

#include <set>
#include <string>
#include <fstream>
#include <stdint.h>
#include "LogConfiger.h"
#include "LogFileName.h"
#include "LogPacker.h"

class LogSink {
public:
    explicit LogSink(const LogConfiger::LogModuleAttrs& attrs);
    ~LogSink();

    int32_t Write(const std::string& data, int32_t level);
    int32_t Flush(bool force = false);

private:
    int32_t LoadConfig(const LogConfiger::LogModuleAttrs& attrs);
    int32_t UpdateCurrentFileName();
    int32_t UpdatePackFileName();
    int32_t OpenFile();
    int32_t RotateIfNeeded(uint32_t dataSize);
    int32_t WriteFile(const std::string& data);
    uint32_t ReadUint32(const LogConfiger::LogModuleAttrs& attrs,
                        const std::string& key,
                        uint32_t defaultValue) const;
    uint32_t ReadUint32AllowZero(const LogConfiger::LogModuleAttrs& attrs,
                                 const std::string& key,
                                 uint32_t defaultValue) const;

private:
    bool mEnabled;
    bool mPackEnabled;
    int32_t mLevel;
    uint32_t mMaxFileCount;
    uint32_t mPackFileCount;
    uint32_t mFileLimit;
    uint64_t mPackTotalSizeLimit;
    uint32_t mFlushThreshold;
    uint32_t mFlushInterval;
    uint32_t mPendingFlushes;
    uint64_t mLastFlushMs;
    std::string mOutput;
    std::string mFile;
    std::string mFileNameFormat;
    std::string mCurrentFile;
    std::string mPackFile;
    std::string mPackDirectory;
    std::string mPackFileNameFormat;
    std::string mPackType;
    std::string mPackCommand;
    std::string mPackCommandOverride;
    std::string mDirectory;
    std::ofstream mStream;
    LogPacker mPacker;
};

#endif // __LOG_SINK_H__
