/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : LogPacker.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Log history rotator and packer.
 *  @date       : 2026/10/13
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __LOG_PACKER_H__
#define __LOG_PACKER_H__

#include <vector>
#include <set>
#include <string>
#include <stdint.h>

struct LogPackerConfig {
    bool packEnabled;
    uint32_t rawFileCount;
    uint32_t packFileCount;
    uint64_t packTotalSizeLimit;
    std::string rawDirectory;
    std::string packDirectory;
    std::string rawFileName;
    std::string packFileName;
    std::string packType;
    std::string packCommand;
    std::string packCommandOverride;
};

class LogPacker {
public:
    LogPacker();
    explicit LogPacker(const LogPackerConfig& config);
    ~LogPacker() = default;

    int32_t SetConfig(const LogPackerConfig& config);
    int32_t ScanFiles();
    int32_t RotateRawFile();
    int32_t PackFilesIfNeeded();
    int32_t TrimRawFiles();
    int32_t TrimPackFiles();

    void SetCurrentRawFile(const std::string& fileName);
    void SetCurrentPackFile(const std::string& fileName);
    const std::string& GetCurrentRawFile() const;
    const std::string& GetCurrentPackFile() const;

private:
    int32_t ScanRawFiles();
    int32_t ScanPackFiles();
    std::string BuildCommand(const std::string& templateCommand,
                             const std::vector<std::string>& srcPaths,
                             const std::string& dstPath) const;
    int32_t ShiftIndexedFiles(std::set<std::string>& files,
                              const std::string& directory,
                              bool insertIndexBeforeExtension,
                              uint32_t step);

private:
    bool mPackEnabled;
    uint32_t mRawFileCount;
    uint32_t mPackFileCount;
    uint64_t mPackTotalSizeLimit;
    std::string mRawDirectory;
    std::string mPackDirectory;
    std::string mRawFileName;
    std::string mCurrentRawFile;
    std::string mPackFileName;
    std::string mCurrentPackFile;
    std::string mPackType;
    std::string mPackCommand;
    std::string mPackCommandOverride;
    std::set<std::string> mRawFiles;
    std::set<std::string> mPackFiles;
};

#endif // __LOG_PACKER_H__
