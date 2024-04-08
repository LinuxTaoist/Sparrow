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

#include <thread>
#include <fstream>
#include <sstream>
#include "SprObserver.h"

class LogManager : public SprObserver
{
public:
    LogManager(ModuleIDType id, const std::string& name);
    virtual ~LogManager();

    int ProcessMsg(const SprMsg& msg);

private:
    void RotateLogsIfNecessary(uint32_t logDataSize);
    void WriteToLogFile(const std::string& logData);
    std::string GetNextLogFileName() const;

    static void ReadLoop(LogManager* pSelf);

private:
    bool            mRunning;
    uint32_t        mMaxFileSize;
    std::string     mLogsPath;;
    std::string     mCurrentLogFile;
    std::ofstream   mLogFileStream;
    std::thread     mThread;
};

#endif
