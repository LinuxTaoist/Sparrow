/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : LogManager.h
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
#ifndef __LOG_MANAGER_H__
#define __LOG_MANAGER_H__

#include <set>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>

class LogManager
{
public:
    LogManager();
    virtual ~LogManager();
    int MainLoop();

private:
    int EnvReady(const std::string& srvName);
    int UpdateSuffixOfAllFiles();
    int RotateLogsIfNecessary(uint32_t logDataSize);
    int WriteToLogFile(const std::string& logData);
    std::set<std::string> GetSortedLogFiles(const std::string& path, const std::string& fileName);

private:
    bool            mRunning;
    uint32_t        mMaxFileSize;
    std::string     mBaseLogFile;
    std::string     mLogsDirPath;
    std::string     mCurrentLogFile;
    std::ofstream   mLogFileStream;
    std::set<std::string> mLogFilePaths;
};

#endif
