/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : LogManager.cpp
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
#include "GeneralUtils.h"
#include "LogManager.h"

using namespace std;
using namespace GeneralUtils;

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %4d LOGM D: " fmt, GetCurTimeStr().c_str(), __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %4d LOGM W: " fmt, GetCurTimeStr().c_str(), __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %4d LOGM E: " fmt, GetCurTimeStr().c_str(), __LINE__, ##args)

#define DEFAULT_LOG_FILE_NUM_LIMIT  10
#define DEFAULT_FRAME_LEN_LIMIT     1024
#define DEFAULT_LOG_FILE_MAX_SIZE   10 * 1024 * 1024        // 10MB
#define DEFAULT_BASE_LOG_FILE_NAME  "sprlog.log"
#define DEFAULT_LOGS_STORAGE_PATH   "/tmp/sprlog"
#define LOG_CONFIGURE_FILE_PATH     "sprlog.conf"

static std::shared_ptr<SharedRingBuffer> pLogMCacheMem = nullptr;

uint8_t LogManager::mLogLevelLimit = LOG_LEVEL_BUTT;

LogManager::LogManager()
{
    mRunning            = true;
    mOutputMode         = LOG_OUTPUT_FILE;
    mLogFrameLength     = DEFAULT_FRAME_LEN_LIMIT;
    mLogFileNum         = DEFAULT_LOG_FILE_NUM_LIMIT;
    mLogFileCapacity    = DEFAULT_LOG_FILE_MAX_SIZE;
    mLogFileName        = DEFAULT_BASE_LOG_FILE_NAME;
    mLogsFilePath       = DEFAULT_LOGS_STORAGE_PATH;
    mCurrentLogFile     = DEFAULT_BASE_LOG_FILE_NAME;

    mLoadAttrMap.insert(std::make_pair("logging.output",        &LogManager::LoadAttrOutputMode));
    mLoadAttrMap.insert(std::make_pair("logging.level",         &LogManager::LoadAttrLevelLimit));
    mLoadAttrMap.insert(std::make_pair("logging.file_name",     &LogManager::LoadAttrFileName));
    mLoadAttrMap.insert(std::make_pair("logging.file_num",      &LogManager::LoadAttrFileNumLimit));
    mLoadAttrMap.insert(std::make_pair("logging.file_capacity", &LogManager::LoadAttrFileCapacityLimit));
    mLoadAttrMap.insert(std::make_pair("logging.file_path",     &LogManager::LoadAttrFilePath));
    mLoadAttrMap.insert(std::make_pair("logging.frame_length",  &LogManager::LoadAttrFrameLengthLimit));

    LoadLogCfgFile(LOG_CONFIGURE_FILE_PATH);
    if (access(mLogsFilePath.c_str(), F_OK) != 0)
    {
        int ret = mkdir(mLogsFilePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0) {
            SPR_LOGE("mkdir %s failed! (%s)\n", mLogsFilePath.c_str(), strerror(errno));
            mRunning = false;
        }
    }

    pLogMCacheMem = std::make_shared<SharedRingBuffer>(LOG_CACHE_MEMORY_PATH, LOG_CACHE_MEMORY_SIZE);
    mLogFilePaths = GetSortedLogFiles(mLogsFilePath, mLogFileName);
    EnvReady(SRV_NAME_LOG);

    // Dump log attrs for debug
    // DumpLogAttrs();
}

LogManager::~LogManager()
{
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

int LogManager::DumpLogAttrs()
{
    SPR_LOGD("------------------------- Dump Log Attrs -------------------------\n");
    SPR_LOGD("- mOutputMode         = %d\n", mOutputMode);
    SPR_LOGD("- mLogLevelLimit      = %d\n", mLogLevelLimit);
    SPR_LOGD("- mLogFrameLength     = %dB\n", mLogFrameLength);
    SPR_LOGD("- mLogFileNum         = %d\n", mLogFileNum);
    SPR_LOGD("- mLogFileCapacity    = %dM\n", mLogFileCapacity / (1024 * 1024));
    SPR_LOGD("- mLogFileName        = %s\n", mLogFileName.c_str());
    SPR_LOGD("- mLogsFilePath       = %s\n", mLogsFilePath.c_str());
    SPR_LOGD("- mCurrentLogFile     = %s\n", mCurrentLogFile.c_str());
    SPR_LOGD("------------------------------------------------------------------\n");
    return 0;
}

void LogManager::LoadAttrOutputMode(const std::string& value)
{
    mOutputMode = (value == "file") ? LOG_OUTPUT_FILE : LOG_OUTPUT_STDOUT;
}

void LogManager::LoadAttrLevelLimit(const std::string& value)
{
    if (value == "debug") {
        mLogLevelLimit = LOG_LEVEL_DEBUG;
    } else if (value == "info") {
        mLogLevelLimit = LOG_LEVEL_INFO;
    } else if (value == "warn") {
        mLogLevelLimit = LOG_LEVEL_WARN;
    } else if (value == "error") {
        mLogLevelLimit = LOG_LEVEL_ERROR;
    } else {
        mLogLevelLimit = LOG_LEVEL_BUTT;
    }
}

void LogManager::LoadAttrFrameLengthLimit(const std::string& value)
{
    mLogFrameLength = atoi(value.c_str());
}

void LogManager::LoadAttrFileNumLimit(const std::string& value)
{
    mLogFileNum = atoi(value.c_str());
}

void LogManager::LoadAttrFileCapacityLimit(const std::string& value)
{
    mLogFileCapacity = atoi(value.c_str()) * 1024 * 1024;
}

void LogManager::LoadAttrFileName(const std::string& value)
{
    mLogFileName = value;
    mCurrentLogFile = value;
}

void LogManager::LoadAttrFilePath(const std::string& value)
{
    mLogsFilePath = value;
}

int LogManager::LoadLogCfgFile(const std::string& cfgPath)
{
    std::ifstream file(cfgPath);
    if (!file)
    {
        SPR_LOGE("Open %s fail! \n", cfgPath.c_str());
        return -1;
    }

    SPR_LOGD("Load %s\n", cfgPath.c_str());
    std::string line;
    std::string buffer;
    while (std::getline(file, buffer))
    {
        line += buffer + "\n";
    }

    std::istringstream iss(line);
    std::string keyValue;
    while (std::getline(iss, keyValue, '\n'))
    {
        size_t delimiter = keyValue.find('=');
        if (delimiter != std::string::npos)
        {
            std::string key = keyValue.substr(0, delimiter);
            std::string value = keyValue.substr(delimiter + 1);
            if (mLoadAttrMap.count(key) != 0) {
                ((LogManager*)this->*(mLoadAttrMap[key]))(value);
            }
        }
    }

    return 0;
}

int LogManager::UpdateSuffixOfAllFiles()
{
    while (mLogFilePaths.size() >= mLogFileNum)
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
            oldPath = mLogsFilePath + "/" + mLogFileName;
            suffix = "1";
        }

        // E.g. /tmp/sprlog/sparrow.log.1 -> /tmp/sprlog/sparrow.log.2
        std::string newFile = mLogFileName + "." + suffix;
        std::string newPath = mLogsFilePath + "/" + newFile;
        int ret = rename(oldPath.c_str(), newPath.c_str());
        if (ret != 0) {
            SPR_LOGE("Rename %s to %s failed! (%s)\n", oldPath.c_str(), newPath.c_str(), strerror(errno));
        }

        tmpLogPaths.insert(newPath);
    }

    tmpLogPaths.insert(mLogsFilePath + "/" + mCurrentLogFile);
    mLogFilePaths = std::move(tmpLogPaths);
    return 0;
}

// E.g: sparrow.log sparrow.log.1 sparrow.log.2 ...
int LogManager::RotateLogsIfNecessary(uint32_t logDataSize)
{
    uint32_t curFileSize = static_cast<uint32_t>(mLogFileStream.tellp());
    if (curFileSize + logDataSize > mLogFileCapacity) {
        mLogFileStream.close();

        UpdateSuffixOfAllFiles();
        mLogFileStream.open(mLogsFilePath + '/' + mCurrentLogFile, std::ios_base::app | std::ios_base::out);
        if (!mLogFileStream.is_open()) {
            SPR_LOGE("Open %s failed!\n", mCurrentLogFile.c_str());
        }
    }

    return 0;
}

int LogManager::GetLevelFromLogStrs(const std::string& logData)
{
    int level = LOG_LEVEL_BUTT;

    // 04-03 07:56:23.032  43930     DebugMsg D:
    char levelChar = 0;
    int rc = GetCharBeforeNthTarget(logData, ':', 3, levelChar);
    if (rc == 0) {
        if  (levelChar == 'D') {
            level = LOG_LEVEL_DEBUG;
        } else if (levelChar == 'I') {
            level = LOG_LEVEL_INFO;
        } else if (levelChar == 'W') {
            level = LOG_LEVEL_WARN;
        } else if (levelChar == 'E') {
            level = LOG_LEVEL_ERROR;
        } else {
            level = LOG_LEVEL_BUTT;
        }
    }

    return level;
}

int LogManager::WriteToLogFile(const std::string& logData)
{
    if (logData.size() > DEFAULT_FRAME_LEN_LIMIT) {
        SPR_LOGE("Out of length limit [%d %d]!\n", (int)logData.size(), DEFAULT_FRAME_LEN_LIMIT);
        return -1;
    }

    if (!mLogFileStream.is_open()) {
        mLogFileStream.open(mLogsFilePath + '/' + mCurrentLogFile, std::ios_base::app | std::ios_base::out);
        if (!mLogFileStream.is_open()) {
            SPR_LOGE("Open %s failed!\n", mCurrentLogFile.c_str());
            return -1;
        }
    }

    mLogFileStream.write(logData.c_str(), logData.size());
    mLogFileStream.flush();
    return 0;
}

std::set<std::string> LogManager::GetSortedLogFiles(const std::string& path, const std::string& fileNamePrefix)
{
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        SPR_LOGE("Open %s failed! (%s)\n", mLogsFilePath.c_str(), strerror(errno));
        return {};
    }

    std::set<std::string> matchingFiles;

    // Iterate through each file in the directory
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string currentFile(entry->d_name);

        // Check if the file name starts with the given prefix
        if (currentFile.find(fileNamePrefix) == 0) {
            matchingFiles.insert(mLogsFilePath + '/' + currentFile);
        }
    }

    // If no files were found, insert the current log file
    if (matchingFiles.empty()) {
        matchingFiles.insert(mLogsFilePath + '/' + mCurrentLogFile);
    }

    closedir(dir);
    return matchingFiles;
}

int LogManager::MainLoop()
{
    while (mRunning)
    {
        if (pLogMCacheMem->AvailData() < 10) {
            usleep(10000);
            continue;
        }

        int32_t len = 0;
        int ret = pLogMCacheMem->read(&len, sizeof(int32_t));
        if (ret != 0 || len < 0 || len > DEFAULT_FRAME_LEN_LIMIT) {
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

        // Write the log if level less than the limit
        int level = GetLevelFromLogStrs(value);
        if (level <= mLogLevelLimit) {
            RotateLogsIfNecessary(len);
            WriteToLogFile(value);
        }
    }

    return 0;
}
