/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : LogManager.cpp
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
#include <memory>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "SprMediatorIpcProxy.h"
#include "SharedRingBuffer.h"
#include "DefineMacro.h"
#include "LogManager.h"

using namespace std;
using namespace InternalEnum;

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)
#define SPR_LOGD(fmt, args...) printf("%04d LOGM D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%04d LOGM W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%04d LOGM E: " fmt, __LINE__, ##args)

#define DEFAULT_LOGS_STORAGE_PATH   "/tmp/sprlog"
#define DEFAULT_LOG_FILE_NAME       "sprlog"
#define DEFAULT_LOG_FILE_MAX_SIZE   10 * 1024 * 1024        // 10MB

#define CACHE_MEMORY_PATH           "/tmp/SprLog.shm"
#define CACHE_MEMORY_SIZE           10 * 1024 * 1024        // 10MB

static SharedRingBuffer* pLogMCacheMem = nullptr;

LogManager::LogManager(ModuleIDType id, const std::string& name)
{
    mRunning = true;

    // TODO: value from config
    mMaxFileSize = DEFAULT_LOG_FILE_MAX_SIZE;
    mLogsPath = DEFAULT_LOGS_STORAGE_PATH;
    mCurrentLogFile = DEFAULT_LOG_FILE_NAME;

    if (access(mLogsPath.c_str(), F_OK) != 0)
    {
        int ret = mkdir(mLogsPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0) {
            SPR_LOGE("mkdir %s failed! (%s)", mLogsPath.c_str(), strerror(errno));
        }
    }

    pLogMCacheMem = new (std::nothrow) SharedRingBuffer(CACHE_MEMORY_PATH, CACHE_MEMORY_SIZE, true);
    if (pLogMCacheMem == nullptr) {
        SPR_LOGE("pLogMCacheMem is nullptr!");
    }

    EnvReady(SRV_NAME_LOG);
}

LogManager::~LogManager()
{
    if (pLogMCacheMem != nullptr) {
        delete pLogMCacheMem;
        pLogMCacheMem = nullptr;
    }

    mLogFileStream.close();
}

int LogManager::EnvReady(const std::string& srvName)
{
    std::string node = "/tmp/" + srvName;
    int fd = creat(node.c_str(), 0644);
    if (fd != -1) {
        close(fd);
    }

    return 0;
}

int LogManager::RotateLogsIfNecessary(uint32_t logDataSize)
{
    if (static_cast<uint32_t>(mLogFileStream.tellp()) + logDataSize > mMaxFileSize) {
        mLogFileStream.close();
        mLogFileStream.open(GetNextLogFileName(), std::ios_base::app | std::ios_base::out);
        if (!mLogFileStream.is_open()) {
            SPR_LOGE("Open %s failed!", mCurrentLogFile.c_str());
        }
    }

    return 0;
}

int LogManager::WriteToLogFile(const std::string& logData)
{
    if (!mLogFileStream.is_open()) {
        mLogFileStream.open(GetNextLogFileName(), std::ios_base::app | std::ios_base::out);
        if (!mLogFileStream.is_open()) {
            SPR_LOGE("Open %s failed!", mCurrentLogFile.c_str());
            return -1;
        }
    }

    mLogFileStream.write(logData.c_str(), logData.size());
    mLogFileStream.flush();
    return 0;
}

std::string LogManager::GetNextLogFileName() const
{
    std::ostringstream oss;
    oss << mLogsPath << "/" << mCurrentLogFile << "_" << time(nullptr) << ".log";
    return oss.str();
}

int LogManager::MainLoop()
{
    while (mRunning)
    {
        if (pLogMCacheMem->AvailData() <= 0) {
            usleep(10000);
            continue;
        }

        int32_t len = 0;
        int ret = pLogMCacheMem->read(&len, sizeof(int32_t));
        if (ret != 0 || len < 0) {
            SPR_LOGE("read memory failed! len = %d, ret = %d\n", len, ret);
            usleep(10000);
            continue;
        }

        std::string value;
        value.resize(len);
        char* data = const_cast<char*>(value.c_str());
        ret = pLogMCacheMem->read(data, len);
        if (ret != 0) {
            SPR_LOGE("read failed! len = %d\n", len);
        }

        RotateLogsIfNecessary(len);
        WriteToLogFile(value);
    }

    return 0;
}
