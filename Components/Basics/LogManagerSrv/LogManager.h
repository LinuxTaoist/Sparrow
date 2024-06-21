/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : LogManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
#ifndef __LOG_MANAGER_H__
#define __LOG_MANAGER_H__

#include <set>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>

enum LogLevel
{
    LOG_LEVEL_MIN   = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_BUTT
};

class LogManager
{
public:
    LogManager();
    virtual ~LogManager();
    int MainLoop();

    void SetLogLevel(LogLevel level) { mLogLevelLimit = level; }
    int  GetLogLevel() { return mLogLevelLimit; }

private:
    int EnvReady(const std::string& srvName);
    int UpdateSuffixOfAllFiles();
    int RotateLogsIfNecessary(uint32_t logDataSize);
    int GetLevelFromLogStrs(const std::string& logData);
    int WriteToLogFile(const std::string& logData);
    std::set<std::string> GetSortedLogFiles(const std::string& path, const std::string& fileName);

private:
    bool            mRunning;
    uint8_t         mLogLevelLimit;
    uint32_t        mMaxFileSize;
    std::string     mBaseLogFile;
    std::string     mLogsDirPath;
    std::string     mCurrentLogFile;
    std::ofstream   mLogFileStream;
    std::set<std::string> mLogFilePaths;
};

#endif // __LOG_MANAGER_H__
