/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : LogSink.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Log output sink.
 *  @date       : 2026/09/20
 *
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <utility>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "CoreTypeDefs.h"
#include "LogConfigKeys.h"
#include "LogSink.h"

namespace {

uint64_t GetMonotonicTickMs() {
    struct timespec ts {};
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

} // namespace

LogSink::LogSink(const LogConfiger::LogModuleAttrs& attrs)
    : mEnabled(true)
    , mLevel(InternalDefs::LOG_LEVEL_DEBUG)
    , mMaxFileCount(10)
    , mFileLimit(10 * 1024 * 1024)
    , mFlushThreshold(64)
    , mFlushInterval(1000)
    , mPendingFlushes(0)
    , mLastFlushMs(GetMonotonicTickMs())
    , mOutput(LOG_CONFIG_VALUE_FILE)
    , mFile(LOG_CONFIG_DEFAULT_FILE_NAME)
    , mCurrentFile(LOG_CONFIG_DEFAULT_FILE_NAME)
    , mDirectory(LOG_CONFIG_DEFAULT_FILE_PATH) {
    LoadConfig(attrs);
    if (mOutput == LOG_CONFIG_VALUE_FILE) {
        MakeDirectory(mDirectory);
        UpdateCurrentFileName();
        mFiles = ListFiles(mDirectory, mFile);
    }
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
    const auto filePath = attrs.find(LOG_CONFIG_KEY_FILE_PATH);
    if (filePath != attrs.end() && !filePath->second.empty()) {
        mDirectory = filePath->second;
    }
    return 0;
}

int32_t LogSink::UpdateCurrentFileName() {
    LogFileName fileName(mFile, mFileNameFormat);
    return fileName.Build(mCurrentFile);
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

    Flush(true);
    mStream.close();
    ShiftFiles();
    UpdateCurrentFileName();
    mFiles.insert(mDirectory + '/' + mCurrentFile);
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

int32_t LogSink::ShiftFiles() {
    while (mFiles.size() >= mMaxFileCount) {
        auto oldest = mFiles.end();
        uint32_t oldestSuffix = 0;
        for (auto it = mFiles.begin(); it != mFiles.end(); ++it) {
            const size_t suffixPosition = it->rfind('.');
            if (suffixPosition == std::string::npos) {
                continue;
            }

            const std::string suffix = it->substr(suffixPosition + 1);
            char* end = nullptr;
            const unsigned long value = std::strtoul(suffix.c_str(), &end, 10);
            if (end != suffix.c_str() && *end == '\0'
                && (oldest == mFiles.end() || value > oldestSuffix)) {
                oldest = it;
                oldestSuffix = static_cast<uint32_t>(value);
            }
        }

        if (oldest == mFiles.end()) {
            oldest = mFiles.begin();
        }
        remove(oldest->c_str());
        mFiles.erase(oldest);
    }

    const std::string currentPath = mDirectory + '/' + mCurrentFile;
    std::vector<std::string> files;
    const auto current = mFiles.find(currentPath);
    if (current != mFiles.end()) {
        files.push_back(*current);
    }
    for (const auto& path : mFiles) {
        if (path != currentPath) {
            files.push_back(path);
        }
    }

    std::vector<std::string> temporaryFiles;
    temporaryFiles.reserve(files.size());
    for (size_t index = 0; index < files.size(); ++index) {
        const std::string temporaryPath = files[index] + ".sprlog_rotating_"
                                         + std::to_string(getpid()) + "_"
                                         + std::to_string(index);
        if (rename(files[index].c_str(), temporaryPath.c_str()) == 0) {
            temporaryFiles.push_back(temporaryPath);
        }
    }

    std::set<std::string> paths;
    for (size_t index = 0; index < temporaryFiles.size(); ++index) {
        const std::string newPath = currentPath + "." + std::to_string(index + 1);
        if (rename(temporaryFiles[index].c_str(), newPath.c_str()) == 0) {
            paths.insert(newPath);
        }
    }

    mFiles = std::move(paths);
    return 0;
}

std::set<std::string> LogSink::ListFiles(const std::string& path,
                                         const std::string& name) {
    std::set<std::string> paths;
    const size_t extensionPosition = name.rfind('.');
    const std::string baseName = extensionPosition == std::string::npos
                               ? name : name.substr(0, extensionPosition);
    const std::string extension = extensionPosition == std::string::npos
                                ? std::string() : name.substr(extensionPosition);
    DIR* directory = opendir(path.c_str());
    if (directory == nullptr) {
        paths.insert(path + '/' + name);
        return paths;
    }

    std::string latestFile;
    struct dirent* entry = nullptr;
    while ((entry = readdir(directory)) != nullptr) {
        const std::string currentFile(entry->d_name);
        if (currentFile == "." || currentFile == ".."
            || currentFile.rfind(baseName, 0) != 0
            || (currentFile.size() > baseName.size()
                && currentFile[baseName.size()] != '.'
                && currentFile[baseName.size()] != '_'
                && currentFile[baseName.size()] != '-')) {
            continue;
        }

        const size_t currentExtension = extension.empty()
                                      ? currentFile.size()
                                      : currentFile.find(extension, baseName.size());
        if (currentExtension == std::string::npos
            || (!extension.empty()
                && currentExtension + extension.size() < currentFile.size()
                && currentFile[currentExtension + extension.size()] != '.')) {
            continue;
        }

        struct stat fileStat {};
        if (stat((path + '/' + currentFile).c_str(), &fileStat) == 0
            && S_ISREG(fileStat.st_mode)) {
            const size_t suffixPosition = currentFile.rfind('.');
            const std::string suffix = suffixPosition == std::string::npos
                                     ? std::string()
                                     : currentFile.substr(suffixPosition + 1);
            char* end = nullptr;
            std::strtoul(suffix.c_str(), &end, 10);
            const bool isRotation = !suffix.empty() && end != suffix.c_str()
                                  && *end == '\0';
            paths.insert(path + '/' + currentFile);
            if (!isRotation && currentFile == name) {
                latestFile = currentFile;
            } else if (!isRotation && latestFile != name
                       && (latestFile.empty() || currentFile > latestFile)) {
                latestFile = currentFile;
            }
        }
    }

    closedir(directory);
    if (!latestFile.empty()) {
        mCurrentFile = latestFile;
    }

    // Keep the selected active file in the managed set. For a dynamic name,
    // this also removes the static placeholder that would otherwise be
    // inserted when the directory is empty.
    paths.erase(path + '/' + name);
    paths.insert(path + '/' + mCurrentFile);
    return paths;
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
