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
#include <sstream>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
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

#define SPR_LOGI(fmt, args...) printf("%s %6d %-12s I: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "LOGM", __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %6d %-12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "LOGM", __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %6d %-12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "LOGM", __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %6d %-12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "LOGM", __LINE__, ##args)

#define DEFAULT_LOG_FILE_NUM_LIMIT  10
#define DEFAULT_FRAME_LEN_LIMIT     1024
#define DEFAULT_LOG_FILE_MAX_SIZE   10 * 1024 * 1024        // 10MB
#define DEFAULT_BASE_LOG_FILE_NAME  "sprlog.log"
#define LOG_CONFIGURE_FILE_PATH     "sprlog.conf"
#define LOG_WRITE_SEMAPHORE_NAME    "/SprLogSem"
#define LOG_FLUSH_COUNT_LIMIT       64
#define LOG_FLUSH_INTERVAL_SEC      1
#define LOG_SEM_WAIT_TIMEOUT_MS     100

static std::unique_ptr<SharedRingBuffer> pLogMCacheMem = nullptr;

bool LogManager::mRunning = true;

static int WaitSemTimeout(sem_t* sem, int timeoutMs)
{
    if (sem == nullptr || sem == SEM_FAILED) {
        return -1;
    }

    if (timeoutMs <= 0) {
        return sem_trywait(sem);
    }

    struct timespec startTs = {};
    if (clock_gettime(CLOCK_MONOTONIC, &startTs) != 0) {
        return sem_trywait(sem);
    }

    while (true) {
        if (sem_trywait(sem) == 0) {
            return 0;
        }

        if (errno != EAGAIN && errno != EINTR) {
            return -1;
        }

        struct timespec nowTs = {};
        if (clock_gettime(CLOCK_MONOTONIC, &nowTs) != 0) {
            return -1;
        }

        long long elapsedMs = static_cast<long long>(nowTs.tv_sec - startTs.tv_sec) * 1000
                    + static_cast<long long>(nowTs.tv_nsec - startTs.tv_nsec) / 1000000;
        if (elapsedMs >= timeoutMs) {
            return -1;
        }

        struct timespec sleepTs = {};
        sleepTs.tv_nsec = 1000000;
        nanosleep(&sleepTs, nullptr);
    }
}

static uint64_t GetMonotonicTickSec()
{
    struct timespec ts {};
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }

    return static_cast<uint64_t>(ts.tv_sec);
}

LogManager::LogManager()
    : mLogLevelLimit(InternalDefs::LOG_LEVEL_BUTT)
    , mOutputMode(LOG_OUTPUT_FILE)
    , mLogFrameLength(DEFAULT_FRAME_LEN_LIMIT)
    , mLogFileNum(DEFAULT_LOG_FILE_NUM_LIMIT)
    , mLogFileCapacity(DEFAULT_LOG_FILE_MAX_SIZE)
    , mPendingFlushCount(0)
    , mLastFlushTickSec(GetMonotonicTickSec())
    , mLogFileName(DEFAULT_BASE_LOG_FILE_NAME)
    , mLogsFilePath(DEFAULT_DEBUG_ROOT_DIR + std::string("/") + DEFAULT_BASE_LOG_FILE_NAME)
    , mCurrentLogFile(DEFAULT_BASE_LOG_FILE_NAME)
    , mReadSem(sem_open(LOG_WRITE_SEMAPHORE_NAME, O_CREAT, 0600, 1))
    , mLogFileStream()
    , mLogFilePaths()
    , mLoadAttrMap()
{

    mLoadAttrMap.insert(std::make_pair("logging.output",        &LogManager::LoadAttrOutputMode));
    mLoadAttrMap.insert(std::make_pair("logging.level",         &LogManager::LoadAttrLevelLimit));
    mLoadAttrMap.insert(std::make_pair("logging.file_name",     &LogManager::LoadAttrFileName));
    mLoadAttrMap.insert(std::make_pair("logging.file_num",      &LogManager::LoadAttrFileNumLimit));
    mLoadAttrMap.insert(std::make_pair("logging.file_capacity", &LogManager::LoadAttrFileCapacityLimit));
    mLoadAttrMap.insert(std::make_pair("logging.file_path",     &LogManager::LoadAttrFilePath));
    mLoadAttrMap.insert(std::make_pair("logging.frame_length",  &LogManager::LoadAttrFrameLengthLimit));

    LoadLogCfgFile(LOG_CONFIGURE_FILE_PATH);
    if (access(mLogsFilePath.c_str(), F_OK) != 0) {
        int ret = mkdir(mLogsFilePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0) {
            SPR_LOGE("mkdir %s failed! (%s)\n", mLogsFilePath.c_str(), strerror(errno));
            mRunning = false;
        }
    }

    pLogMCacheMem.reset(new SharedRingBuffer(LOG_CACHE_MEMORY_PATH, LOG_CACHE_MEMORY_SIZE));
    mLogFilePaths = GetSortedLogFiles(mLogsFilePath, mLogFileName);
    EnvReady(SRV_NAME_LOG);

    // Dump log attrs for debug
    // DumpLogAttrs();
}

LogManager::~LogManager()
{
    if (mReadSem != SEM_FAILED && mReadSem != nullptr) {
        sem_close(mReadSem);
    }
}

int LogManager::EnvReady(const std::string& srvName)
{
    std::string node = DEFAULT_DEBUG_ROOT_DIR + std::string("/") + srvName;
    int fd = creat(node.c_str(), 0644);
    if (fd != -1) {
        close(fd);
    }

    return 0;
}

int LogManager::StopWork()
{
    mRunning = false;
    SPR_LOGD("Stop Work!\n");
    return 0;
}

int LogManager::DumpLogAttrs()
{
    SPR_LOGD("------------------------- Dump Log Attrs -------------------------\n");
    SPR_LOGD("- mOutputMode         = %d\n", mOutputMode);
    SPR_LOGD("- mLogLevelLimit      = %d\n", mLogLevelLimit);
    SPR_LOGD("- mLogFrameLength     = %uB\n", mLogFrameLength);
    SPR_LOGD("- mLogFileNum         = %u\n", mLogFileNum);
    SPR_LOGD("- mLogFileCapacity    = %uM\n", mLogFileCapacity / (1024 * 1024));
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
        mLogLevelLimit = InternalDefs::LOG_LEVEL_DEBUG;
    } else if (value == "info") {
        mLogLevelLimit = InternalDefs::LOG_LEVEL_INFO;
    } else if (value == "warn") {
        mLogLevelLimit = InternalDefs::LOG_LEVEL_WARN;
    } else if (value == "error") {
        mLogLevelLimit = InternalDefs::LOG_LEVEL_ERROR;
    } else {
        mLogLevelLimit = InternalDefs::LOG_LEVEL_BUTT;
    }
}

void LogManager::LoadAttrFrameLengthLimit(const std::string& value)
{
    int32_t frameLength = atoi(value.c_str());
    mLogFrameLength = (frameLength > static_cast<int32_t>(sizeof(int32_t)))
                    ? static_cast<uint32_t>(frameLength)
                    : DEFAULT_FRAME_LEN_LIMIT;
}

void LogManager::LoadAttrFileNumLimit(const std::string& value)
{
    int32_t fileNum = atoi(value.c_str());
    mLogFileNum = (fileNum > 0) ? static_cast<uint32_t>(fileNum) : DEFAULT_LOG_FILE_NUM_LIMIT;
}

void LogManager::LoadAttrFileCapacityLimit(const std::string& value)
{
    int32_t fileCapacityMb = atoi(value.c_str());
    mLogFileCapacity = (fileCapacityMb > 0)
                     ? static_cast<uint32_t>(fileCapacityMb) * 1024 * 1024
                     : DEFAULT_LOG_FILE_MAX_SIZE;
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
    if (!file) {
        SPR_LOGE("Open %s fail! \n", cfgPath.c_str());
        return -1;
    }

    SPR_LOGD("Load %s\n", cfgPath.c_str());
    std::string line;
    std::string buffer;
    while (std::getline(file, buffer)) {
        line += buffer + "\n";
    }

    std::istringstream iss(line);
    std::string keyValue;
    while (std::getline(iss, keyValue, '\n')) {
        size_t delimiter = keyValue.find('=');
        if (delimiter != std::string::npos) {
            std::string key = keyValue.substr(0, delimiter);
            std::string value = keyValue.substr(delimiter + 1);
            if (mLoadAttrMap.count(key) != 0) {
                (reinterpret_cast<LogManager*>(this)->*(mLoadAttrMap[key]))(value);
            }
        }
    }

    return 0;
}

int LogManager::OpenCurrentLogFile()
{
    if (mLogFileStream.is_open()) {
        return 0;
    }

    mLogFileStream.open(mLogsFilePath + '/' + mCurrentLogFile, std::ios_base::app | std::ios_base::out);
    if (!mLogFileStream.is_open()) {
        SPR_LOGE("Open %s failed!\n", mCurrentLogFile.c_str());
        return -1;
    }

    return 0;
}

int LogManager::UpdateSuffixOfAllFiles()
{
    while (mLogFilePaths.size() >= mLogFileNum) {
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
        std::string suffixTag = mLogFileName + ".";
        auto pos = oldPath.rfind(suffixTag);
        if (pos != std::string::npos) {
            suffix = oldPath.substr(pos + suffixTag.size());
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

int LogManager::FlushLogFileIfNecessary(bool force)
{
    if (!mLogFileStream.is_open()) {
        return 0;
    }

    if (!force && mPendingFlushCount == 0) {
        return 0;
    }

    uint64_t now = GetMonotonicTickSec();
    bool needFlush = force
                  || (mPendingFlushCount >= LOG_FLUSH_COUNT_LIMIT)
                  || (now >= mLastFlushTickSec + LOG_FLUSH_INTERVAL_SEC);
    if (!needFlush) {
        return 0;
    }

    mLogFileStream.flush();
    if (!mLogFileStream.good()) {
        SPR_LOGE("Flush %s failed!\n", mCurrentLogFile.c_str());
        return -1;
    }

    mPendingFlushCount = 0;
    mLastFlushTickSec = now;
    return 0;
}

// E.g: sparrow.log sparrow.log.1 sparrow.log.2 ...
int LogManager::RotateLogsIfNecessary(uint32_t logDataSize)
{
    if (OpenCurrentLogFile() != 0) {
        return -1;
    }

    std::streampos pos = mLogFileStream.tellp();
    uint32_t curFileSize = (pos >= 0) ? static_cast<uint32_t>(pos) : 0;
    if (curFileSize + logDataSize > mLogFileCapacity) {
        FlushLogFileIfNecessary(true);
        mLogFileStream.close();

        UpdateSuffixOfAllFiles();
        if (OpenCurrentLogFile() != 0) {
            return -1;
        }

        mPendingFlushCount = 0;
        mLastFlushTickSec = GetMonotonicTickSec();
    }

    return 0;
}

int LogManager::GetLevelFromLogStrs(const std::string& logData)
{
    int level = InternalDefs::LOG_LEVEL_BUTT;

    // 04-03 07:56:23.032  43930     DebugMsg D:
    char levelChar = 0;
    int rc = GetCharBeforeNthTarget(logData, ':', 3, levelChar);
    if (rc == 0) {
        if  (levelChar == 'D') {
            level = InternalDefs::LOG_LEVEL_DEBUG;
        } else if (levelChar == 'I') {
            level = InternalDefs::LOG_LEVEL_INFO;
        } else if (levelChar == 'W') {
            level = InternalDefs::LOG_LEVEL_WARN;
        } else if (levelChar == 'E') {
            level = InternalDefs::LOG_LEVEL_ERROR;
        } else {
            level = InternalDefs::LOG_LEVEL_BUTT;
        }
    }

    return level;
}

int LogManager::WriteToLogFile(const std::string& logData)
{
    if (logData.size() > mLogFrameLength) {
        SPR_LOGE("Out of length limit [%d %u]!\n", (int)logData.size(), mLogFrameLength);
        return -1;
    }

    if (OpenCurrentLogFile() != 0) {
        return -1;
    }

    mLogFileStream.write(logData.c_str(), logData.size());
    if (!mLogFileStream.good()) {
        SPR_LOGE("Write %s failed!\n", mCurrentLogFile.c_str());
        return -1;
    }

    mPendingFlushCount++;
    return FlushLogFileIfNecessary(false);
}

int LogManager::WriteLog(const std::string& logData, int level)
{
    if (mOutputMode == LOG_OUTPUT_STDOUT) {
        fputs(logData.c_str(), stdout);
        if (level <= InternalDefs::LOG_LEVEL_ERROR) {
            fflush(stdout);
        }
        return 0;
    }

    if (RotateLogsIfNecessary(logData.size()) != 0) {
        return -1;
    }

    return WriteToLogFile(logData);
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
        if (currentFile.rfind(fileNamePrefix, 0) == 0) {
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
    if (!pLogMCacheMem) {
        SPR_LOGE("pLogMCacheMem is nullptr!\n");
        return -1;
    }

    while (mRunning) {
        if (pLogMCacheMem->AvailData() < static_cast<int32_t>(sizeof(int32_t))) {
            FlushLogFileIfNecessary(false);
            usleep(10000);
            continue;
        }

        if (mReadSem != SEM_FAILED && mReadSem != nullptr) {
            if (WaitSemTimeout(mReadSem, LOG_SEM_WAIT_TIMEOUT_MS) != 0) {
                usleep(10000);
                continue;
            }
        }

        int32_t len = 0;
        int ret = pLogMCacheMem->Read(&len, sizeof(int32_t));
        if (ret != 0 || len < 0 || len > static_cast<int32_t>(mLogFrameLength)) {
            if (mReadSem != SEM_FAILED && mReadSem != nullptr) {
                sem_post(mReadSem);
            }

            SPR_LOGE("Read memory failed! len = %d, ret = %d\n", len, ret);
            usleep(10000);
            continue;
        }

        std::string value;
        value.resize(len);
        char* data = const_cast<char*>(value.c_str());
        ret = pLogMCacheMem->Read(data, len);
        if (mReadSem != SEM_FAILED && mReadSem != nullptr) {
            sem_post(mReadSem);
        }

        if (ret != 0) {
            SPR_LOGE("Read failed! len = %d\n", len);
        }

        // Write the log if level less than the limit
        int level = GetLevelFromLogStrs(value);
        if (level <= mLogLevelLimit) {
            WriteLog(value, level);
        }
    }

    FlushLogFileIfNecessary(true);
    return 0;
}
