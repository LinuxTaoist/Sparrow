/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprLog.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/02
 *
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <memory>
#include <vector>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <sys/time.h>
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "SharedRingBuffer.h"
#include "SprLog.h"

using namespace InternalDefs;

#define PID_PRINT_WIDTH_LIMIT       6
#define LOG_BUFFER_SIZE_DEFAULT     512
#define SEMAPHORE_NAME              "/SprLogSem"
#define LOG_SEM_WAIT_TIMEOUT_MS     1000

static std::unique_ptr<SharedRingBuffer> pLogSCacheMem = nullptr;

static int32_t WaitSemTimeout(sem_t* sem, int32_t timeoutMs)
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
        sleepTs.tv_nsec = 1000000;  // 1ms
        nanosleep(&sleepTs, nullptr);
    }
}

SprLog::SprLog()
    : mWriteSem(SEM_FAILED)
    , mLevel(LOG_LEVEL_BUTT)
    , mLength(LOG_BUFFER_SIZE_DEFAULT)
{
    mWriteSem = sem_open(SEMAPHORE_NAME, O_CREAT, 0600, 1);
    if (SEM_FAILED == mWriteSem) {
        perror("sem_open failed");
    }

    // Retry: wait for the master (LogManagerSrv) to create /tmp/SprLogShm.
    // On cold boot the tmpfs is empty; without a retry the slave constructor
    // fails permanently because open(path, O_RDWR) returns ENOENT.
    const int MAX_RETRY_CNT = 50;   // 50 × 100 ms = 5 s
    for (int i = 0; i < MAX_RETRY_CNT; i++) {
        pLogSCacheMem.reset(new SharedRingBuffer(LOG_CACHE_MEMORY_PATH));
        if (pLogSCacheMem && pLogSCacheMem->IsEnabled()) {
            break;
        }
        pLogSCacheMem.reset();
        usleep(100000); // 100 ms
    }

    // Last attempt — if still failing, accept the disabled buffer rather than
    // blocking the process forever. Log output degrades gracefully.
    if (!pLogSCacheMem || !pLogSCacheMem->IsEnabled()) {
        pLogSCacheMem.reset(new SharedRingBuffer(LOG_CACHE_MEMORY_PATH));
    }
}

SprLog::~SprLog()
{
    // Refer comment in SprLog::GetInstance()
    // if (SEM_FAILED != mWriteSem) {
    //     sem_close(mWriteSem);
    //     sem_unlink(SEMAPHORE_NAME);
    //     mWriteSem = SEM_FAILED;
    // }
}

SprLog* SprLog::GetInstance()
{
    // never delete this instance
    static SprLog instance;
    return &instance;
}

int32_t SprLog::SetLevel(int32_t level)
{
    mLevel = level;
    return 0;
}

int32_t SprLog::Init(const std::string& moduleName)
{
    if (moduleName.empty() || moduleName.size() >= LOG_RECORD_MODULE_NAME_MAX_LENGTH) {
        return -1;
    }

    mModuleName = moduleName;
    return 0;
}

int32_t SprLog::GetLevel()
{
    return mLevel;
}

int32_t SprLog::SetLength(int32_t length)
{
    if (length <= 0) {
        mLength = LOG_BUFFER_SIZE_DEFAULT;
    } else {
        mLength = std::min(length, static_cast<int32_t>(LOG_BUFFER_SIZE_LIMIT));
    }

    return 0;
}

int32_t SprLog::GetLength()
{
    return mLength;
}

int32_t SprLog::d(const std::string& tag, const char* format, ...)
{
    if (mLevel < LOG_LEVEL_DEBUG) {
        return 0;
    }

    va_list args;
    va_start(args, format);
    int32_t result = LogImpl(LOG_LEVEL_DEBUG, tag, format, args);
    va_end(args);

    return result;
}

int32_t SprLog::i(const std::string& tag, const char* format, ...)
{
    if (mLevel < LOG_LEVEL_INFO) {
        return 0;
    }

    va_list args;
    va_start(args, format);
    int32_t result = LogImpl(LOG_LEVEL_INFO, tag, format, args);
    va_end(args);

    return result;
}

int32_t SprLog::w(const std::string& tag, const char* format, ...)
{
    if (mLevel < LOG_LEVEL_WARN) {
        return 0;
    }

    va_list args;
    va_start(args, format);
    int32_t result = LogImpl(LOG_LEVEL_WARN, tag, format, args);
    va_end(args);

    return result;
}

int32_t SprLog::e(const std::string& tag, const char* format, ...)
{
    if (mLevel < LOG_LEVEL_ERROR) {
        return 0;
    }

    va_list args;
    va_start(args, format);
    int32_t result = LogImpl(LOG_LEVEL_ERROR, tag, format, args);
    va_end(args);

    return result;
}

static uint64_t GetCurrentTimestampMs()
{
    struct timeval tv = {};
    if (gettimeofday(&tv, nullptr) != 0) {
        return 0;
    }

    return static_cast<uint64_t>(tv.tv_sec) * 1000U
         + static_cast<uint64_t>(tv.tv_usec / 1000);
}

static const std::string& GetDefaultModuleName()
{
    static const std::string moduleName = []() {
        char path[256] = {0};
        ssize_t length = readlink("/proc/self/exe", path, sizeof(path) - 1);
        if (length <= 0) {
            return std::string("unknown");
        }

        path[length] = '\0';
        const char* name = strrchr(path, '/');
        return std::string(name == nullptr ? path : name + 1);
    }();

    return moduleName;
}

static std::string GetCurrentTimestampText()
{
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) != 0) {
        perror("gettimeofday");
        return "null";
    }

    struct tm local_tm;
    localtime_r(&tv.tv_sec, &local_tm);

    char buffer[30] = {0};
    strftime(buffer, sizeof(buffer), "%m-%d %H:%M:%S.", &local_tm);

    // millisecond
    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "%.3ld", tv.tv_usec / 1000);
    return buffer;
}

// 04-03 07:56:23.032  43930     DebugMsg D:
static int FormatLog(std::string& log, int32_t level, const std::string& tag, const char* buffer)
{
    __pid_t pid = getpid();
    std::ostringstream oss;
    std::string timestamp = GetCurrentTimestampText();
    const char* levelText = (level == LOG_LEVEL_DEBUG) ? "D"
                          : (level == LOG_LEVEL_INFO)  ? "I"
                          : (level == LOG_LEVEL_WARN)  ? "W"
                          : (level == LOG_LEVEL_ERROR) ? "E"
                          : "U";

    oss << timestamp;
    oss << " " << std::right << std::setw(PID_PRINT_WIDTH_LIMIT) << pid;
    oss << " " << std::left << std::setw(TAG_PRINT_WIDTH_LIMIT) << tag;
    oss << " " << levelText;
    oss << ": " << buffer;

    bool hasNewline = std::any_of(buffer, buffer + strlen(buffer), [](char c){ return c == '\n'; });
    if (!hasNewline) {
        oss << "\n";
    }

    log += oss.str();
    return 0;
}

int32_t SprLog::LogImpl(int32_t level, const std::string& tag, const char* format, va_list args)
{
    std::vector<char> buffer(mLength, 0);
    int32_t result = vsnprintf(buffer.data(), buffer.size(), format, args);

    if (result < 0 || result >= (int32_t)buffer.size() || result > LOG_BUFFER_SIZE_LIMIT) {
        char prefix[11] = {0};
        memcpy(prefix, buffer.data(), 10);
        std::fill(buffer.begin(), buffer.end(), 0);
        snprintf(buffer.data(), buffer.size(),
            "%s...... [TRUNCATED] LEN:%d >= LIMIT:%zu [LOG CONTENT TRUNCATED]",
            prefix, result, buffer.size());
        result = -1;
    }

    std::string log;
    FormatLog(log, level, tag, buffer.data());
    if (mWriteSem == SEM_FAILED) {
        fputs(log.c_str(), stdout);
        return result;
    }

    if (WaitSemTimeout(mWriteSem, LOG_SEM_WAIT_TIMEOUT_MS) != 0) {
        fputs(log.c_str(), stdout);
        return result;
    }
    LogsToMemory(level, tag, log.c_str(), (int32_t)log.length());
    sem_post(mWriteSem);

    return result;
}


int32_t SprLog::LogsToMemory(int32_t level, const std::string& tag, const char* logs, int32_t len)
{
    if (!pLogSCacheMem || !logs || len <= 0) {
        return -1;
    }

    SLogRecordHeader header = {};
    header.magic = LOG_RECORD_MAGIC;
    header.version = LOG_RECORD_VERSION;
    header.headerSize = sizeof(SLogRecordHeader);
    header.payloadLength = static_cast<uint32_t>(len);
    header.timestampMs = GetCurrentTimestampMs();
    header.pid = static_cast<uint32_t>(getpid());
    header.level = static_cast<uint8_t>(level);
    header.category = LOG_CATEGORY_NORMAL;
    const std::string& moduleName = mModuleName.empty() ? GetDefaultModuleName() : mModuleName;
    strncpy(header.moduleName, moduleName.c_str(), sizeof(header.moduleName) - 1);
    strncpy(header.tag, tag.c_str(), sizeof(header.tag) - 1);

    const size_t recordLength = sizeof(header) + static_cast<size_t>(len);
    const int32_t frameLength = static_cast<int32_t>(recordLength);
    std::vector<unsigned char> frame(sizeof(frameLength) + recordLength, 0);
    memcpy(frame.data(), &frameLength, sizeof(frameLength));
    memcpy(frame.data() + sizeof(frameLength), &header, sizeof(header));
    memcpy(frame.data() + sizeof(frameLength) + sizeof(header), logs, len);
    return pLogSCacheMem->Write(frame.data(), static_cast<int32_t>(frame.size()));
}
