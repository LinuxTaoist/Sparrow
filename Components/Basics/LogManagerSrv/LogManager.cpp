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
#include <vector>
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
#include <cstdlib>
#include <limits>
#include <tuple>
#include "SharedRingBuffer.h"
#include "CommonMacros.h"
#include "GeneralUtils.h"
#include "LogConfigKeys.h"
#include "LogManager.h"

using namespace std;
using namespace GeneralUtils;

#define SPR_LOGI(fmt, args...) printf("%s %6d %-12s I: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "LOGM", __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %6d %-12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "LOGM", __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %6d %-12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "LOGM", __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %6d %-12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "LOGM", __LINE__, ##args)

#define LOG_CONFIGURE_FILE_PATH     "sprlog.conf"
#define LOG_WRITE_SEMAPHORE_NAME    "/SprLogSem"
#define LOG_SEM_WAIT_TIMEOUT_MS     100

bool LogManager::mRunning = true;

static int32_t WaitSemTimeout(sem_t* sem, int32_t timeoutMs) {
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

LogManager::LogManager()
    : mCache()
    , mConfiger()
    , mFrameLength(1024) {
    mReadSem = sem_open(LOG_WRITE_SEMAPHORE_NAME, O_CREAT, 0600, 1);
    LoadConfig(GetConfigPath());
    mCache.reset(new SharedRingBuffer(LOG_CACHE_MEMORY_PATH, LOG_CACHE_MEMORY_SIZE));
    EnvReady(SRV_NAME_LOG);
}

LogManager::~LogManager() {
    if (mReadSem != SEM_FAILED && mReadSem != nullptr) {
        sem_close(mReadSem);
    }
}

int32_t LogManager::EnvReady(const std::string& srvName) {
    std::string node = DEFAULT_DEBUG_ROOT_DIR + std::string("/") + srvName;
    int32_t fd = creat(node.c_str(), 0644);
    if (fd != -1) {
        close(fd);
    }

    return 0;
}

int32_t LogManager::StopWork() {
    mRunning = false;
    SPR_LOGD("Stop Work!\n");
    return 0;
}

int32_t LogManager::LoadConfig(const std::string& path) {
    int32_t ret = mConfiger.Load(path);
    const LogConfiger::LogModules modules = mConfiger.GetLogModules();
    auto moduleIt = modules.find(LOG_CONFIG_MODULE_DEFAULT);
    if (moduleIt == modules.end()) {
        return -1;
    }

    const LogConfiger::LogModuleAttrs& attrs = moduleIt->second;
    auto frameLength = attrs.find(LOG_CONFIG_KEY_FRAME_LENGTH_BYTES);
    if (frameLength != attrs.end()) {
        errno = 0;
        char* end = nullptr;
        const unsigned long value = std::strtoul(frameLength->second.c_str(), &end, 10);
        if (errno == 0 && end != frameLength->second.c_str() && *end == '\0'
            && value >= sizeof(int32_t) && value <= std::numeric_limits<uint32_t>::max()) {
            mFrameLength = static_cast<uint32_t>(value);
        }
    }

    mSinks.clear();
    for (const auto& module : modules) {
        mSinks.emplace(std::piecewise_construct,
                       std::forward_as_tuple(module.first),
                       std::forward_as_tuple(module.second));
    }
    return ret;
}

int32_t LogManager::Write(const std::string& moduleName, const std::string& data, int32_t level) {
    auto sink = mSinks.find(moduleName);
    if (sink == mSinks.end()) {
        sink = mSinks.find(LOG_CONFIG_MODULE_DEFAULT);
    }
    return sink->second.Write(data, level);
}

int32_t LogManager::Flush(bool force) {
    int32_t ret = 0;
    for (auto& sink : mSinks) {
        if (sink.second.Flush(force) != 0) {
            ret = -1;
        }
    }
    return ret;
}

int32_t LogManager::TryReadRecord(std::string& moduleName,
                                  std::string& data,
                                  int32_t& level) {
    if (mCache->AvailData() < static_cast<int32_t>(sizeof(uint32_t))) {
        return -1;
    }

    if (mReadSem != SEM_FAILED &&
        mReadSem != nullptr    &&
        WaitSemTimeout(mReadSem, LOG_SEM_WAIT_TIMEOUT_MS) != 0) {
        return -1;
    }

    // Scan the byte stream for the record magic. The outer frame length is
    // intentionally ignored here: it is only a fast framing hint and may be
    // corrupted. Producers publish a complete frame in one ring-buffer write,
    // so a valid candidate has its complete header available when found.
    const uint32_t magic = LOG_RECORD_MAGIC;
    uint32_t window = 0;
    int32_t scanCount = mCache->AvailData();
    while (scanCount-- > 0) {
        uint8_t byte = 0;
        if (mCache->Read(&byte, sizeof(byte)) != 0) {
            break;
        }
        window = (window >> 8) | (static_cast<uint32_t>(byte) << 24);
        if (window != magic || mCache->AvailData()
                < static_cast<int32_t>(sizeof(InternalDefs::SLogRecordHeader) - sizeof(uint32_t))) {
            continue;
        }

        InternalDefs::SLogRecordHeader header = {};
        memcpy(&header, &magic, sizeof(magic));
        if (mCache->Read(reinterpret_cast<uint8_t*>(&header) + sizeof(magic),
                 sizeof(header) - sizeof(magic)) != 0) {
            break;
        }

        if (header.version != LOG_RECORD_VERSION
            || header.headerSize < sizeof(InternalDefs::SLogRecordHeader)
            || header.payloadLength > mFrameLength
            || mCache->AvailData() < static_cast<int32_t>(header.payloadLength)) {
            window = 0;
            continue;
        }

        std::vector<char> payload(header.payloadLength);
        if (mCache->Read(payload.data(), header.payloadLength) != 0) {
            break;
        }
        moduleName.assign(header.moduleName,
                   strnlen(header.moduleName,
                       LOG_RECORD_MODULE_NAME_MAX_LENGTH));
        data.assign(payload.data(), header.payloadLength);
        level = header.level;
        if (mReadSem != SEM_FAILED && mReadSem != nullptr) {
            sem_post(mReadSem);
        }
        return 0;
    }

    if (mReadSem != SEM_FAILED && mReadSem != nullptr) {
        sem_post(mReadSem);
    }
    return -1;
}

std::string LogManager::GetConfigPath() {
    const char* pEnvRoot = std::getenv(ENV_SPR_ROOT_PATH);
    if (pEnvRoot != nullptr && pEnvRoot[0] != '\0') {
        return std::string(pEnvRoot) + "/" + DEFAULT_SPR_ETC_FILE + "/" + LOG_CONFIGURE_FILE_PATH;
    }

    char exePath[300] = {0};
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len > 0) {
        exePath[len] = '\0';
        std::string fullPath(exePath);
        std::string::size_type pos = fullPath.find_last_of('/');
        if (pos != std::string::npos) {
            std::string execDir = fullPath.substr(0, pos);
            return execDir + "/../" + DEFAULT_SPR_ETC_FILE + "/" + LOG_CONFIGURE_FILE_PATH;
        }
    }

    return "";
}

int32_t LogManager::MainLoop() {
    if (!mCache) {
        SPR_LOGE("Log cache is unavailable!\n");
        return -1;
    }

    while (mRunning) {
        if (mCache->AvailData() < static_cast<int32_t>(sizeof(int32_t))) {
            Flush(false);
            usleep(10000);
            continue;
        }

        std::string value;
        std::string moduleName;
        int32_t level = InternalDefs::LOG_LEVEL_BUTT;
        int32_t ret = TryReadRecord(moduleName, value, level);
        if (ret != 0) {
            usleep(10000);
            continue;
        }

        Write(moduleName, value, level);
    }

    Flush(true);
    return 0;
}
