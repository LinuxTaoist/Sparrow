/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : LogSink.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Log output sink.
 *  @date       : 2026/09/20
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <cerrno>
#include <cstdio>
#include <limits>
#include <unistd.h>
#include <sys/stat.h>
#include "CoreTypeDefs.h"
#include "LogConfigKeys.h"
#include "LogSink.h"

namespace {

uint64_t GetMonotonicTickMs() {
    struct timespec ts = {};
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }

    return static_cast<uint64_t>(ts.tv_sec) * 1000
         + static_cast<uint64_t>(ts.tv_nsec) / 1000000;
}

bool MakeDirectory(const std::string& path) {
    if (path.empty() || access(path.c_str(), F_OK) == 0) {
        return !path.empty();
    }

    const size_t separator = path.find_last_of('/');
    if (separator != std::string::npos && separator > 0
        && !MakeDirectory(path.substr(0, separator))) {
        return false;
    }

    return mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0
        || errno == EEXIST;
}

bool EndsWith(const std::string& value, const std::string& suffix) {
    return value.size() >= suffix.size()
        && value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string GetPackExtension(const std::string& packType) {
    if (packType == "tar.gz") {
        return ".tar.gz";
    }
    if (packType == "zip") {
        return ".zip";
    }
    return std::string();
}

std::string NormalizePackBaseName(const std::string& packName,
                                  const std::string& extension) {
    if (extension.empty()) {
        return packName;
    }
    if (EndsWith(packName, extension)) {
        return packName.substr(0, packName.size() - extension.size());
    }
    return packName;
}

} // namespace

LogSink::LogSink(const LogConfiger::LogModuleAttrs& attrs)
    : mEnabled(true)
    , mPackEnabled(false)
    , mLevel(InternalDefs::LOG_LEVEL_DEBUG)
    , mMaxFileCount(10)
    , mPackFileCount(10)
    , mFileLimit(10 * 1024 * 1024)
    , mPackTotalSizeLimit(0)
    , mFlushThreshold(64)
    , mFlushInterval(1000)
    , mPendingFlushes(0)
    , mLastFlushMs(GetMonotonicTickMs())
    , mOutput(LOG_CONFIG_VALUE_FILE)
    , mFile(LOG_CONFIG_DEFAULT_FILE_NAME)
    , mCurrentFile(LOG_CONFIG_DEFAULT_FILE_NAME)
    , mPackFile(LOG_CONFIG_DEFAULT_PACK_NAME)
    , mPackDirectory(LOG_CONFIG_DEFAULT_PACK_DIR)
    , mDirectory(LOG_CONFIG_DEFAULT_LOG_DIR) {
    LoadConfig(attrs);
    if (mOutput != LOG_CONFIG_VALUE_FILE) {
        return;
    }

    MakeDirectory(mDirectory);
    if (mPackEnabled) {
        MakeDirectory(mPackDirectory);
    }
    UpdateCurrentFileName();
    UpdatePackFileName();

    LogPackerConfig config = {
        mPackEnabled,
        mMaxFileCount,
        mPackFileCount,
        mPackTotalSizeLimit,
        mDirectory,
        mPackDirectory,
        mFile,
        mPackFile,
        mPackType,
        mPackCommand,
        mPackCommandOverride
    };
    mPacker.SetConfig(config);
    mPacker.SetCurrentRawFile(mCurrentFile);
    mPacker.ScanFiles();
    mCurrentFile = mPacker.GetCurrentRawFile();
}

LogSink::~LogSink() {
    Flush(true);
}

int32_t LogSink::LoadConfig(const LogConfiger::LogModuleAttrs& attrs) {
    const auto enabled = attrs.find(LOG_CONFIG_KEY_ENABLED);
    if (enabled != attrs.end()) {
        mEnabled = enabled->second != LOG_CONFIG_VALUE_FALSE;
    }

    const auto level = attrs.find(LOG_CONFIG_KEY_LEVEL);
    if (level != attrs.end()) {
        if (level->second == LOG_CONFIG_VALUE_ERROR) {
            mLevel = InternalDefs::LOG_LEVEL_ERROR;
        } else if (level->second == LOG_CONFIG_VALUE_WARN) {
            mLevel = InternalDefs::LOG_LEVEL_WARN;
        } else if (level->second == LOG_CONFIG_VALUE_INFO) {
            mLevel = InternalDefs::LOG_LEVEL_INFO;
        }
    }

    const auto output = attrs.find(LOG_CONFIG_KEY_OUTPUT);
    if (output != attrs.end()) {
        mOutput = output->second;
    }

    mMaxFileCount = ReadUint32(attrs, LOG_CONFIG_KEY_FILE_NUM, mMaxFileCount);
    const uint32_t fileCapacityMb = ReadUint32(attrs, LOG_CONFIG_KEY_FILE_CAPACITY_MB, 10);
    if (fileCapacityMb <= std::numeric_limits<uint32_t>::max() / (1024 * 1024)) {
        mFileLimit = fileCapacityMb * 1024 * 1024;
    }
    mFlushThreshold = ReadUint32(attrs, LOG_CONFIG_KEY_FLUSH_COUNT, mFlushThreshold);
    mFlushInterval = ReadUint32(attrs, LOG_CONFIG_KEY_FLUSH_INTERVAL_MS, mFlushInterval);

    const auto fileName = attrs.find(LOG_CONFIG_KEY_FILE_NAME);
    if (fileName != attrs.end() && !fileName->second.empty()) {
        mFile = fileName->second;
    }
    const auto fileNameFormat = attrs.find(LOG_CONFIG_KEY_FILE_NAME_FORMAT);
    if (fileNameFormat != attrs.end()) {
        mFileNameFormat = fileNameFormat->second;
    }
    const auto logDir = attrs.find(LOG_CONFIG_KEY_LOG_DIR);
    if (logDir != attrs.end() && !logDir->second.empty()) {
        mDirectory = logDir->second;
    }

    const auto packEnabled = attrs.find(LOG_CONFIG_KEY_PACK_ENABLED);
    if (packEnabled != attrs.end()) {
        mPackEnabled = packEnabled->second != LOG_CONFIG_VALUE_FALSE;
    }
    const auto packName = attrs.find(LOG_CONFIG_KEY_PACK_NAME);
    if (packName != attrs.end() && !packName->second.empty()) {
        mPackFile = packName->second;
    }
    const auto packDir = attrs.find(LOG_CONFIG_KEY_PACK_DIR);
    if (packDir != attrs.end() && !packDir->second.empty()) {
        mPackDirectory = packDir->second;
    }
    const auto packNameFormat = attrs.find(LOG_CONFIG_KEY_PACK_NAME_FORMAT);
    if (packNameFormat != attrs.end()) {
        mPackFileNameFormat = packNameFormat->second;
    }
    const auto packType = attrs.find(LOG_CONFIG_KEY_PACK_TYPE);
    if (packType != attrs.end()) {
        mPackType = packType->second;
    }
    const auto packCommand = attrs.find(LOG_CONFIG_KEY_PACK_COMMAND);
    if (packCommand != attrs.end()) {
        mPackCommand = packCommand->second;
    }
    const auto packCommandOverride = attrs.find(LOG_CONFIG_KEY_PACK_COMMAND_OVERRIDE);
    if (packCommandOverride != attrs.end()) {
        mPackCommandOverride = packCommandOverride->second;
    }
    if (mPackCommandOverride.empty()) {
        mPackCommandOverride = mPackCommand;
    }
    if (mPackDirectory.empty()) {
        mPackDirectory = mDirectory;
    }

    mPackFileCount = ReadUint32(attrs, LOG_CONFIG_KEY_PACK_FILE_NUM, mPackFileCount);
    const uint32_t packTotalSizeMb = ReadUint32AllowZero(attrs,
                                                         LOG_CONFIG_KEY_PACK_TOTAL_SIZE_MB,
                                                         0);
    mPackTotalSizeLimit = static_cast<uint64_t>(packTotalSizeMb) * 1024ULL * 1024ULL;
    return 0;
}

int32_t LogSink::UpdateCurrentFileName() {
    LogFileName fileName(mFile, mFileNameFormat);
    return fileName.Build(mCurrentFile);
}

int32_t LogSink::UpdatePackFileName() {
    std::string currentPackFile;
    const std::string extension = mPackCommandOverride.empty()
                                ? GetPackExtension(mPackType) : std::string();
    const std::string placeholderExt = "__packext__";
    const std::string logicalPackName = extension.empty()
                                      ? mPackFile
                                      : NormalizePackBaseName(mPackFile, extension);
    LogFileName fileName(extension.empty()
                       ? logicalPackName
                       : logicalPackName + "." + placeholderExt,
                         mPackFileNameFormat);
    int32_t ret = fileName.Build(currentPackFile);
    if (ret == 0) {
        if (!extension.empty()) {
            const std::string placeholderSuffix = "." + placeholderExt;
            if (EndsWith(currentPackFile, placeholderSuffix)) {
                currentPackFile = currentPackFile.substr(
                    0, currentPackFile.size() - placeholderSuffix.size()) + extension;
            }
        }
        mPacker.SetCurrentPackFile(currentPackFile);
    }
    return ret;
}

int32_t LogSink::Write(const std::string& logData, int32_t level) {
    if (!mEnabled || level > mLevel) {
        return 0;
    }

    if (mOutput == LOG_CONFIG_VALUE_STDOUT) {
        fputs(logData.c_str(), stdout);
        if (level <= InternalDefs::LOG_LEVEL_ERROR) {
            fflush(stdout);
        }
        return 0;
    }

    if (RotateIfNeeded(static_cast<uint32_t>(logData.size())) != 0) {
        return -1;
    }
    return WriteFile(logData);
}

int32_t LogSink::Flush(bool force) {
    if (!mStream.is_open()) {
        return 0;
    }
    if (!force && mPendingFlushes == 0) {
        return 0;
    }

    const uint64_t now = GetMonotonicTickMs();
    if (!force && mPendingFlushes < mFlushThreshold
        && now < mLastFlushMs + mFlushInterval) {
        return 0;
    }

    mStream.flush();
    if (!mStream.good()) {
        return -1;
    }
    mPendingFlushes = 0;
    mLastFlushMs = now;
    return 0;
}

int32_t LogSink::OpenFile() {
    if (mStream.is_open()) {
        return 0;
    }
    if (!MakeDirectory(mDirectory)) {
        return -1;
    }

    mStream.open(mDirectory + '/' + mCurrentFile, std::ios_base::app | std::ios_base::out);
    return mStream.is_open() ? 0 : -1;
}

int32_t LogSink::RotateIfNeeded(uint32_t dataSize) {
    if (OpenFile() != 0) {
        return -1;
    }

    const std::streampos position = mStream.tellp();
    const uint32_t currentFileSize = position >= 0 ? static_cast<uint32_t>(position) : 0;
    if (currentFileSize + dataSize <= mFileLimit) {
        return 0;
    }

    int32_t ret = Flush(true);
    if (ret != 0) {
        return -1;
    }
    mStream.close();

    mPacker.SetCurrentRawFile(mCurrentFile);
    ret = mPacker.RotateRawFile();
    if (ret != 0) {
        return -1;
    }
    if (mPackEnabled) {
        ret = UpdatePackFileName();
        if (ret != 0) {
            return -1;
        }
        mPacker.PackFilesIfNeeded();
        mPacker.TrimPackFiles();
    }
    mPacker.TrimRawFiles();

    ret = UpdateCurrentFileName();
    if (ret != 0) {
        return -1;
    }
    mPacker.SetCurrentRawFile(mCurrentFile);
    mPendingFlushes = 0;
    mLastFlushMs = GetMonotonicTickMs();
    return OpenFile();
}

int32_t LogSink::WriteFile(const std::string& data) {
    if (mStream.write(data.c_str(), data.size()).fail()) {
        return -1;
    }
    ++mPendingFlushes;
    return Flush(false);
}

uint32_t LogSink::ReadUint32(const LogConfiger::LogModuleAttrs& attrs,
                             const std::string& key,
                             uint32_t defaultValue) const {
    const auto attr = attrs.find(key);
    if (attr == attrs.end() || attr->second.empty()) {
        return defaultValue;
    }

    errno = 0;
    char* end = nullptr;
    const unsigned long value = std::strtoul(attr->second.c_str(), &end, 10);
    if (errno == ERANGE || end == attr->second.c_str() || *end != '\0'
        || value == 0 || value > std::numeric_limits<uint32_t>::max()) {
        return defaultValue;
    }

    return static_cast<uint32_t>(value);
}

uint32_t LogSink::ReadUint32AllowZero(const LogConfiger::LogModuleAttrs& attrs,
                                      const std::string& key,
                                      uint32_t defaultValue) const {
    const auto attr = attrs.find(key);
    if (attr == attrs.end() || attr->second.empty()) {
        return defaultValue;
    }

    errno = 0;
    char* end = nullptr;
    const unsigned long value = std::strtoul(attr->second.c_str(), &end, 10);
    if (errno == ERANGE || end == attr->second.c_str() || *end != '\0'
        || value > std::numeric_limits<uint32_t>::max()) {
        return defaultValue;
    }

    return static_cast<uint32_t>(value);
}
