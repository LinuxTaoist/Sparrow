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
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "SharedRingBuffer.h"
#include "CommonMacros.h"
#include "LogManager.h"

using namespace std;

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)
#define SPR_LOGD(fmt, args...) printf("%04d LOGM D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%04d LOGM W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%04d LOGM E: " fmt, __LINE__, ##args)

#define DEFAULT_LOG_FILE_NUM_LIMIT  10
#define DEFAULT_BASE_LOG_FILE_NAME  "sparrow.log"
#define DEFAULT_LOGS_STORAGE_PATH   "/tmp/sprlog"
#define DEFAULT_LOG_FILE_MAX_SIZE   10 * 1024 * 1024        // 10MB

static std::shared_ptr<SharedRingBuffer> pLogMCacheMem = nullptr;

LogManager::LogManager()
{
    mRunning = true;

    // TODO: value from config
    mMaxFileSize = DEFAULT_LOG_FILE_MAX_SIZE;
    mLogsDirPath = DEFAULT_LOGS_STORAGE_PATH;
    mBaseLogFile = DEFAULT_BASE_LOG_FILE_NAME;
    mCurrentLogFile = DEFAULT_BASE_LOG_FILE_NAME;

    if (access(mLogsDirPath.c_str(), F_OK) != 0)
    {
        int ret = mkdir(mLogsDirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0) {
            SPR_LOGE("mkdir %s failed! (%s)", mLogsDirPath.c_str(), strerror(errno));
        }
    }

    pLogMCacheMem = std::make_shared<SharedRingBuffer>(LOG_CACHE_MEMORY_PATH, LOG_CACHE_MEMORY_SIZE);
    if (pLogMCacheMem == nullptr) {
        SPR_LOGE("pLogMCacheMem is nullptr!");
    }

    mLogFilePaths = GetSortedLogFiles(mLogsDirPath, mBaseLogFile);
    EnvReady(SRV_NAME_LOG);
}

LogManager::~LogManager()
{
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

int LogManager::UpdateSuffixOfAllFiles()
{
    while (mLogFilePaths.size() >= DEFAULT_LOG_FILE_NUM_LIMIT)
    {
        auto it = mLogFilePaths.end();
        --it;
        int ret = remove(it->c_str());
        if (ret != 0) {
            SPR_LOGE("Remove %s failed! (%s)\n", it->c_str(), strerror(errno));
        }

        mLogFilePaths.erase(it);
    }

    std::set<std::string> tmpLogPaths;
    for (auto it = mLogFilePaths.rbegin(); it != mLogFilePaths.rend(); ++it) {
        std::string oldPath = *it;
        std::string suffix;

        // Add 1 to the suffix of an existing file
        auto pos = oldPath.find(".log.");
        if (pos != std::string::npos) {
            suffix = oldPath.substr(pos + 5, 1);
            int version = atoi(suffix.c_str()) + 1;
            suffix = std::to_string(version);
        } else {
            oldPath = mLogsDirPath + "/" + mBaseLogFile;
            suffix = "1";
        }

        // E.g. /tmp/sprlog/sparrow.log.1 -> /tmp/sprlog/sparrow.log.2
        std::string newFile = mBaseLogFile + "." + suffix;
        std::string newPath = mLogsDirPath + "/" + newFile;
        int ret = rename(oldPath.c_str(), newPath.c_str());
        if (ret != 0) {
            SPR_LOGE("Rename %s to %s failed! (%s)\n", oldPath.c_str(), newPath.c_str(), strerror(errno));
        }

        tmpLogPaths.insert(newPath);
    }

    tmpLogPaths.insert(mLogsDirPath + "/" + mCurrentLogFile);
    mLogFilePaths = std::move(tmpLogPaths);
    return 0;
}

// E.g: sparrow.log sparrow.log.1 sparrow.log.2 ...
int LogManager::RotateLogsIfNecessary(uint32_t logDataSize)
{
    uint32_t curFileSize = static_cast<uint32_t>(mLogFileStream.tellp());
    if (curFileSize + logDataSize > mMaxFileSize) {
        mLogFileStream.close();

        UpdateSuffixOfAllFiles();
        mLogFileStream.open(mLogsDirPath + '/' + mCurrentLogFile, std::ios_base::app | std::ios_base::out);
        if (!mLogFileStream.is_open()) {
            SPR_LOGE("Open %s failed!", mCurrentLogFile.c_str());
        }
    }

    return 0;
}

int LogManager::WriteToLogFile(const std::string& logData)
{
    if (!mLogFileStream.is_open()) {
        mLogFileStream.open(mLogsDirPath + '/' + mCurrentLogFile, std::ios_base::app | std::ios_base::out);
        if (!mLogFileStream.is_open()) {
            SPR_LOGE("Open %s failed!", mCurrentLogFile.c_str());
            return -1;
        }
    }

    mLogFileStream.write(logData.c_str(), logData.size());
    mLogFileStream.flush();
    return 0;
}

std::set<std::string> LogManager::GetSortedLogFiles(const std::string& path, const std::string& fileName)
{
    DIR* dir;
    struct dirent* ent;
    std::set<std::string> files;

    if ((dir = opendir(path.c_str())) != nullptr) {
        // Iterating over each file in directory
        while ((ent = readdir(dir)) != nullptr) {
            std::string tmpFile(ent->d_name);
            if (tmpFile.find(fileName) == 0) {
                files.insert(mLogsDirPath + '/' + tmpFile);
            }
        }
        closedir(dir);
    } else {
        SPR_LOGE("Open %s failed! (%s)\n", path.c_str(), strerror(errno));
        return files;
    }

    if (files.empty()) {
        files.insert(mLogsDirPath + '/' + mCurrentLogFile);
    }

    return files;
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
