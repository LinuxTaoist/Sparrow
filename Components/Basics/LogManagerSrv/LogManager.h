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
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>

enum LogLevel
{
    LOG_LEVEL_MIN   = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_BUTT
};

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
    virtual ~LogManager();
    int MainLoop();

    static void SetLogLevel(LogLevel level) { mLogLevelLimit = level; }
    static int  GetLogLevel() { return mLogLevelLimit; }

private:
    int EnvReady(const std::string& srvName);
    int DumpLogAttrs();
    int LoadLogCfgFile(const std::string& cfgPath);
    int UpdateSuffixOfAllFiles();
    int RotateLogsIfNecessary(uint32_t logDataSize);
    int GetLevelFromLogStrs(const std::string& logData);
    int WriteToLogFile(const std::string& logData);
    std::set<std::string> GetSortedLogFiles(const std::string& path, const std::string& fileName);

    void LoadAttrOutputMode(const std::string& value);
    void LoadAttrLevelLimit(const std::string& value);
    void LoadAttrFrameLengthLimit(const std::string& value);
    void LoadAttrFileNumLimit(const std::string& value);
    void LoadAttrFileCapacityLimit(const std::string& value);
    void LoadAttrFileName(const std::string& value);
    void LoadAttrFilePath(const std::string& value);

private:
    static uint8_t  mLogLevelLimit;     // defined with "logging.level"

    bool            mRunning;
    uint8_t         mOutputMode;        // defined with "logging.output"
    uint32_t        mLogFrameLength;    // defined with "logging.frame_length"
    uint32_t        mLogFileNum;        // defined with "logging.file_num"
    uint32_t        mLogFileCapacity;   // defined with "logging.file_capacity"
    std::string     mLogFileName;       // defined with "logging.file_name"
    std::string     mLogsFilePath;      // defined with "logging.file_path"
    std::string     mCurrentLogFile;
    std::ofstream   mLogFileStream;
    std::set<std::string> mLogFilePaths;

    using LoadAttrFunc = void (LogManager::*)(const std::string&);
    std::map<std::string, LoadAttrFunc> mLoadAttrMap;
};

#endif // __LOG_MANAGER_H__
