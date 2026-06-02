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
#include <map>
#include <string>
#include <fstream>
#include <semaphore.h>
#include "CoreTypeDefs.h"

enum LogOutputMode
{
    LOG_OUTPUT_MIN   = 0,
    LOG_OUTPUT_STDOUT,
    LOG_OUTPUT_FILE,
    LOG_OUTPUT_BUTT
};

class LogManager
{
public:
    LogManager();
    ~LogManager();
    int MainLoop();
    static int StopWork();

private:
    LogManager(const LogManager&) = delete;
    LogManager& operator = (const LogManager&) = delete;
    LogManager(LogManager&&) = delete;
    LogManager& operator = (LogManager&&) = delete;

    int EnvReady(const std::string& srvName);
    int DumpLogAttrs();
    int LoadLogCfgFile(const std::string& cfgPath);
    int OpenCurrentLogFile();
    int UpdateSuffixOfAllFiles();
    int FlushLogFileIfNecessary(bool force = false);
    int RotateLogsIfNecessary(uint32_t logDataSize);
    int GetLevelFromLogStrs(const std::string& logData);
    int WriteToLogFile(const std::string& logData);
    int WriteLog(const std::string& logData, int level);
    std::set<std::string> GetSortedLogFiles(const std::string& path, const std::string& fileName);

    void LoadAttrOutputMode(const std::string& value);
    void LoadAttrLevelLimit(const std::string& value);
    void LoadAttrFrameLengthLimit(const std::string& value);
    void LoadAttrFileNumLimit(const std::string& value);
    void LoadAttrFileCapacityLimit(const std::string& value);
    void LoadAttrFileName(const std::string& value);
    void LoadAttrFilePath(const std::string& value);

private:
    static bool     mRunning;
    uint8_t         mLogLevelLimit;     // defined with "logging.level"
    uint8_t         mOutputMode;        // defined with "logging.output"
    uint32_t        mLogFrameLength;    // defined with "logging.frame_length"
    uint32_t        mLogFileNum;        // defined with "logging.file_num"
    uint32_t        mLogFileCapacity;   // defined with "logging.file_capacity"
    uint32_t        mPendingFlushCount;
    uint64_t        mLastFlushTickSec;
    std::string     mLogFileName;       // defined with "logging.file_name"
    std::string     mLogsFilePath;      // defined with "logging.file_path"
    std::string     mCurrentLogFile;
    sem_t*          mReadSem;
    std::ofstream   mLogFileStream;
    std::set<std::string> mLogFilePaths;

    using LoadAttrFunc = void (LogManager::*)(const std::string&);
    std::map<std::string, LoadAttrFunc> mLoadAttrMap;
};

#endif // __LOG_MANAGER_H__
