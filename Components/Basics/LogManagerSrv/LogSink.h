#ifndef __LOG_SINK_H__
#define __LOG_SINK_H__

#include <set>
#include <string>
#include <fstream>
#include <stdint.h>
#include "LogConfiger.h"
#include "LogFileName.h"

class LogSink {
public:
    explicit LogSink(const LogConfiger::LogModuleAttrs& attrs);
    ~LogSink();

    int32_t Write(const std::string& data, int32_t level);
    int32_t Flush(bool force = false);

private:
    int32_t LoadConfig(const LogConfiger::LogModuleAttrs& attrs);
    int32_t UpdateCurrentFileName();
    int32_t OpenFile();
    int32_t RotateIfNeeded(uint32_t dataSize);
    int32_t WriteFile(const std::string& data);
    int32_t ShiftFiles();
    std::set<std::string> ListFiles(const std::string& path,
                                    const std::string& name) const;
    uint32_t ReadUint32(const LogConfiger::LogModuleAttrs& attrs,
                        const std::string& key,
                        uint32_t defaultValue) const;

private:
    bool mEnabled;
    int32_t mLevel;
    uint32_t mMaxFileCount;
    uint32_t mFileLimit;
    uint32_t mFlushThreshold;
    uint32_t mFlushInterval;
    uint32_t mPendingFlushes;
    uint64_t mLastFlushMs;
    std::string mOutput;
    std::string mFile;
    std::string mFileNameFormat;
    std::string mCurrentFile;
    std::string mDirectory;
    std::ofstream mStream;
    std::set<std::string> mFiles;
};

#endif // __LOG_SINK_H__
