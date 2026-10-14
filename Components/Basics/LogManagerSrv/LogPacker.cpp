/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : LogPacker.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Log history rotator and packer.
 *  @date       : 2026/10/13
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "GeneralUtils.h"
#include "LogPacker.h"

namespace {

#define LOG_ROTATING_MARKER ".sprlog_rotating_"
#define LOG_PACKING_MARKER ".sprlog_packing_"

struct IndexedFile {
    std::string path;
    std::string logicalName;
    uint32_t index;
};

std::string JoinPath(const std::string& dir, const std::string& name) {
    return dir + '/' + name;
}

std::string GetFileName(const std::string& path) {
    const size_t slash = path.rfind('/');
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

void RemoveIfExists(const std::string& path) {
    if (!path.empty()) {
        remove(path.c_str());
    }
}

bool EndsWith(const std::string& value, const std::string& suffix) {
    return value.size() >= suffix.size()
        && value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string GetArchiveExtension(const std::string& name) {
    if (EndsWith(name, ".tar.gz")) {
        return ".tar.gz";
    }

    const size_t dotPos = name.rfind('.');
    if (dotPos == std::string::npos || dotPos == 0 || dotPos + 1 >= name.size()) {
        return std::string();
    }
    return name.substr(dotPos);
}

bool StartsWithSeparator(const std::string& value, size_t index) {
    if (index >= value.size()) {
        return false;
    }
    return value[index] == '.' || value[index] == '_' || value[index] == '-';
}

bool ParseTrailingIndex(const std::string& value, uint32_t& index, size_t& dotPos) {
    dotPos = value.rfind('.');
    if (dotPos == std::string::npos || dotPos + 1 >= value.size()) {
        return false;
    }

    errno = 0;
    char* end = nullptr;
    const unsigned long parsed = std::strtoul(value.c_str() + dotPos + 1, &end, 10);
    if (errno != 0 || end == value.c_str() + dotPos + 1 || *end != '\0'
        || parsed > std::numeric_limits<uint32_t>::max()) {
        return false;
    }

    index = static_cast<uint32_t>(parsed);
    return true;
}

bool MatchLogicalName(const std::string& actualName, const std::string& configuredName) {
    const std::string configuredExt = GetArchiveExtension(configuredName);
    const std::string configuredBase = configuredExt.empty()
                                     ? configuredName
                                     : configuredName.substr(0, configuredName.size() - configuredExt.size());
    if (!configuredExt.empty()) {
        if (actualName.size() <= configuredExt.size()
            || actualName.compare(actualName.size() - configuredExt.size(),
                                  configuredExt.size(), configuredExt) != 0) {
            return false;
        }
    }

    std::string actualBase = configuredExt.empty()
                           ? actualName
                           : actualName.substr(0, actualName.size() - configuredExt.size());
    if (actualBase.rfind(configuredBase, 0) != 0) {
        return false;
    }
    return actualBase.size() == configuredBase.size()
        || StartsWithSeparator(actualBase, configuredBase.size());
}

bool ParseRawFile(const std::string& name,
                  const std::string& configuredName,
                  std::string& logicalName,
                  uint32_t& index,
                  bool& rotated) {
    rotated = false;
    logicalName = name;
    size_t dotPos = std::string::npos;
    if (ParseTrailingIndex(name, index, dotPos)) {
        rotated = true;
        logicalName = name.substr(0, dotPos);
    }
    return MatchLogicalName(logicalName, configuredName);
}

bool ParsePackFile(const std::string& name,
                   const std::string& configuredName,
                   std::string& logicalName,
                   uint32_t& index) {
    const std::string configuredExt = GetArchiveExtension(configuredName);
    if (configuredExt.empty()) {
        return false;
    }
    if (name.size() <= configuredExt.size()
        || name.compare(name.size() - configuredExt.size(), configuredExt.size(), configuredExt) != 0) {
        return false;
    }

    const std::string body = name.substr(0, name.size() - configuredExt.size());
    size_t dotPos = std::string::npos;
    if (!ParseTrailingIndex(body, index, dotPos)) {
        return false;
    }

    logicalName = body.substr(0, dotPos) + configuredExt;
    return MatchLogicalName(logicalName, configuredName);
}

bool FileStat(const std::string& path, struct stat& fileStat) {
    return stat(path.c_str(), &fileStat) == 0 && S_ISREG(fileStat.st_mode);
}

uint64_t GetFileSize(const std::string& path) {
    struct stat fileStat = {};
    return FileStat(path, fileStat) ? static_cast<uint64_t>(fileStat.st_size) : 0;
}

std::string ReplaceAll(std::string value,
                       const std::string& from,
                       const std::string& to) {
    if (from.empty()) {
        return value;
    }

    size_t pos = 0;
    while ((pos = value.find(from, pos)) != std::string::npos) {
        value.replace(pos, from.size(), to);
        pos += to.size();
    }
    return value;
}

std::string ShellQuote(const std::string& value) {
    std::string quoted = "'";
    for (size_t index = 0; index < value.size(); ++index) {
        if (value[index] == '\'') {
            quoted += "'\\''";
            continue;
        }
        quoted += value[index];
    }
    quoted += "'";
    return quoted;
}

std::string JoinQuotedValues(const std::vector<std::string>& values) {
    std::string joined;
    for (size_t index = 0; index < values.size(); ++index) {
        if (!joined.empty()) {
            joined += ' ';
        }
        joined += ShellQuote(values[index]);
    }
    return joined;
}

std::string BuildIndexedFileName(const std::string& logicalName,
                                 uint32_t index,
                                 bool insertIndexBeforeExtension) {
    if (!insertIndexBeforeExtension) {
        return logicalName + "." + std::to_string(index);
    }

    const std::string extension = GetArchiveExtension(logicalName);
    if (extension.empty()) {
        return logicalName + "." + std::to_string(index);
    }
    const size_t dotPos = logicalName.size() - extension.size();
    return logicalName.substr(0, dotPos) + "." + std::to_string(index)
        + extension;
}

bool GetIndexedFile(const std::string& path,
                    bool insertIndexBeforeExtension,
                    IndexedFile& file) {
    const std::string name = GetFileName(path);
    uint32_t index = 0;
    size_t dotPos = std::string::npos;
    if (!insertIndexBeforeExtension) {
        if (!ParseTrailingIndex(name, index, dotPos)) {
            return false;
        }
        file.logicalName = name.substr(0, dotPos);
    } else {
        const std::string extension = GetArchiveExtension(name);
        if (extension.empty()) {
            return false;
        }
        const size_t extPos = name.size() - extension.size();
        const std::string body = name.substr(0, extPos);
        if (!ParseTrailingIndex(body, index, dotPos)) {
            return false;
        }
        file.logicalName = body.substr(0, dotPos) + extension;
    }

    file.path = path;
    file.index = index;
    return true;
}

std::vector<IndexedFile> CollectIndexedFiles(const std::set<std::string>& files,
                                             bool insertIndexBeforeExtension) {
    std::vector<IndexedFile> indexedFiles;
    indexedFiles.reserve(files.size());
    for (std::set<std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
        IndexedFile file;
        if (GetIndexedFile(*it, insertIndexBeforeExtension, file)) {
            indexedFiles.push_back(file);
        }
    }
    return indexedFiles;
}

std::vector<std::string> BuildPackTypeCommands(const std::string& packType) {
    if (packType == "tar.gz") {
        return {
            "tar -czf \"{dst}\" -C \"{dir}\" {name_list}",
            "busybox tar -czf \"{dst}\" -C \"{dir}\" {name_list}"
        };
    }
    if (packType == "zip") {
        return {
            "zip -j \"{dst}\" {src_list}"
        };
    }
    return std::vector<std::string>();
}

} // namespace

LogPacker::LogPacker()
    : mPackEnabled(false)
    , mRawFileCount(10)
    , mPackFileCount(10)
    , mPackTotalSizeLimit(0) {
}

LogPacker::LogPacker(const LogPackerConfig& config)
    : mPackEnabled(false)
    , mRawFileCount(10)
    , mPackFileCount(10)
    , mPackTotalSizeLimit(0) {
    SetConfig(config);
}

int32_t LogPacker::SetConfig(const LogPackerConfig& config) {
    mPackEnabled = config.packEnabled;
    mRawFileCount = config.rawFileCount;
    mPackFileCount = config.packFileCount;
    mPackTotalSizeLimit = config.packTotalSizeLimit;
    mRawDirectory = config.rawDirectory;
    mPackDirectory = config.packDirectory.empty() ? config.rawDirectory : config.packDirectory;
    mRawFileName = config.rawFileName;
    mPackFileName = config.packFileName;
    mPackType = config.packType;
    mPackCommand = config.packCommand;
    mPackCommandOverride = config.packCommandOverride;
    if (mPackCommandOverride.empty()) {
        const std::string extension = GetArchiveExtension(mPackFileName);
        if (mPackType == "tar.gz" && extension != ".tar.gz") {
            if (!extension.empty()) {
                mPackFileName = mPackFileName.substr(0, mPackFileName.size() - extension.size());
            }
            mPackFileName += ".tar.gz";
        } else if (mPackType == "zip" && extension != ".zip") {
            if (!extension.empty()) {
                mPackFileName = mPackFileName.substr(0, mPackFileName.size() - extension.size());
            }
            mPackFileName += ".zip";
        }
    }
    return 0;
}

void LogPacker::SetCurrentRawFile(const std::string& fileName) {
    mCurrentRawFile = fileName;
}

void LogPacker::SetCurrentPackFile(const std::string& fileName) {
    mCurrentPackFile = fileName;
}

const std::string& LogPacker::GetCurrentRawFile() const {
    return mCurrentRawFile;
}

const std::string& LogPacker::GetCurrentPackFile() const {
    return mCurrentPackFile;
}

int32_t LogPacker::ScanFiles() {
    mRawFiles.clear();
    mPackFiles.clear();

    ScanRawFiles();
    if (mPackEnabled) {
        ScanPackFiles();
    }
    return 0;
}

int32_t LogPacker::ScanRawFiles() {
    DIR* directory = opendir(mRawDirectory.c_str());
    if (directory == nullptr) {
        return 0;
    }

    std::string latestRawFile;
    struct dirent* entry = nullptr;
    while ((entry = readdir(directory)) != nullptr) {
        std::string fileName(entry->d_name);
        if (fileName == "." || fileName == "..") {
            continue;
        }

        std::string fullPath = JoinPath(mRawDirectory, fileName);
        struct stat fileStat = {};
        if (!FileStat(fullPath, fileStat)) {
            continue;
        }

        if (fileName.find(LOG_ROTATING_MARKER) != std::string::npos
            || fileName.find(LOG_PACKING_MARKER) != std::string::npos) {
            RemoveIfExists(fullPath);
            continue;
        }

        std::string logicalName;
        uint32_t index = 0;
        bool rotated = false;
        if (!ParseRawFile(fileName, mRawFileName, logicalName, index, rotated)) {
            continue;
        }

        if (rotated) {
            mRawFiles.insert(fullPath);
            continue;
        }
        if (logicalName == mRawFileName || (latestRawFile != mRawFileName && logicalName > latestRawFile)) {
            latestRawFile = logicalName;
        }
    }
    closedir(directory);

    if (!latestRawFile.empty()) {
        mCurrentRawFile = latestRawFile;
    }
    return 0;
}

int32_t LogPacker::ScanPackFiles() {
    DIR* directory = opendir(mPackDirectory.c_str());
    if (directory == nullptr) {
        return 0;
    }

    std::string latestPackFile;
    struct dirent* entry = nullptr;
    while ((entry = readdir(directory)) != nullptr) {
        std::string fileName(entry->d_name);
        if (fileName == "." || fileName == "..") {
            continue;
        }

        std::string fullPath = JoinPath(mPackDirectory, fileName);
        struct stat fileStat = {};
        if (!FileStat(fullPath, fileStat)) {
            continue;
        }

        if (fileName.find(LOG_ROTATING_MARKER) != std::string::npos
            || fileName.find(LOG_PACKING_MARKER) != std::string::npos) {
            RemoveIfExists(fullPath);
            continue;
        }

        std::string logicalName;
        uint32_t index = 0;
        if (!ParsePackFile(fileName, mPackFileName, logicalName, index)) {
            continue;
        }

        mPackFiles.insert(fullPath);
        if (latestPackFile.empty() || logicalName > latestPackFile) {
            latestPackFile = logicalName;
        }
    }
    closedir(directory);

    if (!latestPackFile.empty()) {
        mCurrentPackFile = latestPackFile;
    }
    return 0;
}

int32_t LogPacker::RotateRawFile() {
    if (ShiftIndexedFiles(mRawFiles, mRawDirectory, false, 1) != 0) {
        return -1;
    }

    std::string currentPath = JoinPath(mRawDirectory, mCurrentRawFile);
    std::string rotatedPath = JoinPath(mRawDirectory,
                                       BuildIndexedFileName(mCurrentRawFile, 1, false));
    if (rename(currentPath.c_str(), rotatedPath.c_str()) != 0) {
        return -1;
    }

    mRawFiles.insert(rotatedPath);
    return 0;
}

int32_t LogPacker::PackFilesIfNeeded() {
    if (!mPackEnabled || mRawFileCount == 0) {
        return 0;
    }

    while (mRawFiles.size() >= mRawFileCount) {
        std::vector<IndexedFile> rawFiles = CollectIndexedFiles(mRawFiles, false);
        if (rawFiles.size() < mRawFileCount) {
            break;
        }

        std::sort(rawFiles.begin(), rawFiles.end(), [](const IndexedFile& left, const IndexedFile& right) {
            if (left.index != right.index) {
                return left.index < right.index;
            }
            return left.path < right.path;
        });
        rawFiles.resize(mRawFileCount);

        std::vector<std::string> sourcePaths;
        sourcePaths.reserve(rawFiles.size());
        for (size_t index = 0; index < rawFiles.size(); ++index) {
            sourcePaths.push_back(rawFiles[index].path);
        }

        std::string tempPath = JoinPath(mPackDirectory,
            GetFileName(rawFiles.front().path) + LOG_PACKING_MARKER
                             + std::to_string(getpid()));
        RemoveIfExists(tempPath);

        bool isPacked = false;
        if (!mPackCommandOverride.empty() || !mPackCommand.empty()) {
            std::string command = BuildCommand(!mPackCommandOverride.empty()
                                             ? mPackCommandOverride : mPackCommand,
                                             sourcePaths,
                                             tempPath);
            int ret = GeneralUtils::SystemCmd("%s", command.c_str());
            isPacked = ret == 0 && GetFileSize(tempPath) > 0;
        } else if (!mPackType.empty()) {
            std::vector<std::string> commands = BuildPackTypeCommands(mPackType);
            for (size_t cmdIndex = 0; cmdIndex < commands.size(); ++cmdIndex) {
                std::string command = BuildCommand(commands[cmdIndex], sourcePaths, tempPath);
                int ret = GeneralUtils::SystemCmd("%s", command.c_str());
                if (ret == 0 && GetFileSize(tempPath) > 0) {
                    isPacked = true;
                    break;
                }
                RemoveIfExists(tempPath);
            }
        }

        if (!isPacked) {
            RemoveIfExists(tempPath);
            break;
        }

        ShiftIndexedFiles(mPackFiles, mPackDirectory, true, 1);
        std::string finalPath = JoinPath(mPackDirectory,
            BuildIndexedFileName(mCurrentPackFile, 1, true));
        RemoveIfExists(finalPath);
        if (rename(tempPath.c_str(), finalPath.c_str()) != 0) {
            RemoveIfExists(tempPath);
            break;
        }

        for (size_t index = 0; index < rawFiles.size(); ++index) {
            RemoveIfExists(rawFiles[index].path);
            mRawFiles.erase(rawFiles[index].path);
        }
        mPackFiles.insert(finalPath);
        TrimPackFiles();
    }
    return 0;
}

int32_t LogPacker::TrimRawFiles() {
    std::vector<IndexedFile> rawFiles = CollectIndexedFiles(mRawFiles, false);
    std::sort(rawFiles.begin(), rawFiles.end(), [](const IndexedFile& left, const IndexedFile& right) {
        if (left.index != right.index) {
            return left.index > right.index;
        }
        return left.path > right.path;
    });

    while (rawFiles.size() > mRawFileCount) {
        RemoveIfExists(rawFiles.front().path);
        mRawFiles.erase(rawFiles.front().path);
        rawFiles.erase(rawFiles.begin());
    }
    return 0;
}

int32_t LogPacker::TrimPackFiles() {
    std::vector<IndexedFile> packFiles = CollectIndexedFiles(mPackFiles, true);
    std::sort(packFiles.begin(), packFiles.end(), [](const IndexedFile& left, const IndexedFile& right) {
        if (left.index != right.index) {
            return left.index > right.index;
        }
        return left.path > right.path;
    });

    while (packFiles.size() > mPackFileCount) {
        RemoveIfExists(packFiles.front().path);
        mPackFiles.erase(packFiles.front().path);
        packFiles.erase(packFiles.begin());
    }

    if (mPackTotalSizeLimit == 0) {
        return 0;
    }

    uint64_t totalSize = 0;
    for (size_t index = 0; index < packFiles.size(); ++index) {
        totalSize += GetFileSize(packFiles[index].path);
    }
    while (!packFiles.empty() && totalSize > mPackTotalSizeLimit) {
        totalSize -= GetFileSize(packFiles.front().path);
        RemoveIfExists(packFiles.front().path);
        mPackFiles.erase(packFiles.front().path);
        packFiles.erase(packFiles.begin());
    }
    return 0;
}

std::string LogPacker::BuildCommand(const std::string& templateCommand,
                                    const std::vector<std::string>& srcPaths,
                                    const std::string& dstPath) const {
    std::vector<std::string> sourceNames;
    sourceNames.reserve(srcPaths.size());
    for (size_t index = 0; index < srcPaths.size(); ++index) {
        sourceNames.push_back(GetFileName(srcPaths[index]));
    }

    const std::string firstSource = srcPaths.empty() ? std::string() : srcPaths.front();
    const std::string firstName = sourceNames.empty() ? std::string() : sourceNames.front();
    std::string command = ReplaceAll(templateCommand, "{src}", firstSource);
    command = ReplaceAll(command, "{src_list}", JoinQuotedValues(srcPaths));
    command = ReplaceAll(command, "{dst}", dstPath);
    command = ReplaceAll(command, "{dir}", mRawDirectory);
    command = ReplaceAll(command, "{name}", firstName);
    command = ReplaceAll(command, "{name_list}", JoinQuotedValues(sourceNames));
    return command;
}

int32_t LogPacker::ShiftIndexedFiles(std::set<std::string>& files,
                                     const std::string& directory,
                                     bool insertIndexBeforeExtension,
                                     uint32_t step) {
    if (files.empty() || step == 0) {
        return 0;
    }

    std::vector<IndexedFile> indexedFiles = CollectIndexedFiles(files, insertIndexBeforeExtension);
    std::vector<IndexedFile> stagedFiles;
    stagedFiles.reserve(indexedFiles.size());
    for (size_t index = 0; index < indexedFiles.size(); ++index) {
        std::string tempPath = indexedFiles[index].path + LOG_ROTATING_MARKER
                             + std::to_string(getpid()) + "_"
                             + std::to_string(index);
        RemoveIfExists(tempPath);
        if (rename(indexedFiles[index].path.c_str(), tempPath.c_str()) != 0) {
            continue;
        }

        indexedFiles[index].path = tempPath;
        stagedFiles.push_back(indexedFiles[index]);
    }

    std::set<std::string> shiftedFiles;
    for (size_t index = 0; index < stagedFiles.size(); ++index) {
        std::string newPath = JoinPath(directory,
            BuildIndexedFileName(stagedFiles[index].logicalName,
                                 stagedFiles[index].index + step,
                                 insertIndexBeforeExtension));
        RemoveIfExists(newPath);
        if (rename(stagedFiles[index].path.c_str(), newPath.c_str()) == 0) {
            shiftedFiles.insert(newPath);
        } else {
            RemoveIfExists(stagedFiles[index].path);
        }
    }

    files = shiftedFiles;
    return 0;
}
