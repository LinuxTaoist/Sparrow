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

#include <map>
#include <memory>
#include <string>
#include <semaphore.h>
#include "CoreTypeDefs.h"
#include "LogSink.h"
#include "LogConfiger.h"

class SharedRingBuffer;

class LogManager {
public:
    LogManager();
    ~LogManager();
    int32_t MainLoop();
    static int32_t StopWork();

private:
    LogManager(const LogManager&) = delete;
    LogManager& operator = (const LogManager&) = delete;
    LogManager(LogManager&&) = delete;
    LogManager& operator = (LogManager&&) = delete;

    int32_t EnvReady(const std::string& srvName);
    int32_t LoadConfig(const std::string& path);
    int32_t TryReadRecord(std::string& moduleName, std::string& data, int32_t& level);
    int32_t Write(const std::string& moduleName, const std::string& data, int32_t level);
    int32_t Flush(bool force = false);
    std::string GetConfigPath();

private:
    static bool     mRunning;
    std::unique_ptr<SharedRingBuffer> mCache;
    LogConfiger     mConfiger;
    uint32_t        mFrameLength;
    sem_t*          mReadSem;
    std::map<std::string, LogSink> mSinks;
};

#endif // __LOG_MANAGER_H__
