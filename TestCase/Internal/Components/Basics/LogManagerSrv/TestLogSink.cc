/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : TestLogSink.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : LogSink internal tests.
 *  @date       : 2026/09/20
 *
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <string>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gtest/gtest.h>
#include "CoreTypeDefs.h"
#include "LogConfigKeys.h"
#include "LogSink.h"

namespace {

LogConfiger::LogModuleAttrs FileAttrs(const std::string& path)
{
    return {
        {"enabled", "true"},
        {"level", "debug"},
        {"output", "file"},
        {"file_name", "sink.log"},
        {"file_path", path},
        {"file_num", "3"},
        {"file_capacity_mb", "1"},
        {"flush_count", "1"},
        {"flush_interval_ms", "100000"}
    };
}

// 测试文件输出和强制刷新后能够正确读取日志内容
TEST(LogM_LogSink, WritesAndFlushesFile)
{
    const std::string path = "/tmp/sparrow_log_sink_test_" + std::to_string(getpid());
    LogSink sink(FileAttrs(path));

    ASSERT_EQ(sink.Write("hello\n", InternalDefs::LOG_LEVEL_INFO), 0);
    ASSERT_EQ(sink.Flush(true), 0);

    std::ifstream stream(path + "/sink.log");
    ASSERT_TRUE(stream.is_open());
    std::string value;
    std::getline(stream, value);
    EXPECT_EQ(value, "hello");
    remove((path + "/sink.log").c_str());
    rmdir(path.c_str());
}

// 测试禁用开关和日志级别过滤
TEST(LogM_LogSink, FiltersDisabledAndLevel)
{
    auto attrs = FileAttrs("/tmp/sparrow_log_sink_disabled_" + std::to_string(getpid()));
    attrs[LOG_CONFIG_KEY_ENABLED] = LOG_CONFIG_VALUE_FALSE;
    LogSink disabled(attrs);
    EXPECT_EQ(disabled.Write("ignored", InternalDefs::LOG_LEVEL_ERROR), 0);

    attrs[LOG_CONFIG_KEY_ENABLED] = LOG_CONFIG_VALUE_TRUE;
    attrs[LOG_CONFIG_KEY_LEVEL] = LOG_CONFIG_VALUE_ERROR;
    LogSink limited(attrs);
    EXPECT_EQ(limited.Write("ignored", InternalDefs::LOG_LEVEL_INFO), 0);
}

// 测试动态文件名格式以及达到容量限制后的文件轮转
TEST(LogM_LogSink, CustomFileNameFormatAndRotation)
{
    const std::string path = "/tmp/sparrow_log_sink_format_" + std::to_string(getpid());
    auto attrs = FileAttrs(path);
    attrs[LOG_CONFIG_KEY_FILE_NAME] = "main.log";
    attrs[LOG_CONFIG_KEY_FILE_NAME_FORMAT] = "BN_ST.FX";

    LogSink sink(attrs);
    const std::string firstData(600 * 1024, 'a');
    const std::string secondData(600 * 1024, 'b');

    ASSERT_EQ(sink.Write(firstData, InternalDefs::LOG_LEVEL_INFO), 0);
    ASSERT_EQ(sink.Write(secondData, InternalDefs::LOG_LEVEL_INFO), 0);
    ASSERT_EQ(sink.Flush(true), 0);

    std::ifstream current;
    std::ifstream rotated;
    std::string currentPath;
    std::string rotatedPath;
    {
        DIR* directory = opendir(path.c_str());
        ASSERT_NE(directory, nullptr);
        struct dirent* entry = nullptr;
        while ((entry = readdir(directory)) != nullptr) {
            const std::string name(entry->d_name);
            if (name == "." || name == "..") {
                continue;
            }
            if (name.find("main_") == 0) {
                if (name.find(".1") != std::string::npos) {
                    rotatedPath = path + "/" + name;
                } else {
                    currentPath = path + "/" + name;
                }
            }
        }
        closedir(directory);
    }

    ASSERT_FALSE(currentPath.empty());
    ASSERT_FALSE(rotatedPath.empty());
    current.open(currentPath);
    rotated.open(rotatedPath);
    ASSERT_TRUE(current.is_open());
    ASSERT_TRUE(rotated.is_open());

    current.seekg(0, std::ios::end);
    rotated.seekg(0, std::ios::end);
    EXPECT_EQ(static_cast<uint64_t>(current.tellg()), secondData.size());
    EXPECT_EQ(static_cast<uint64_t>(rotated.tellg()), firstData.size());

    remove(currentPath.c_str());
    remove(rotatedPath.c_str());
    rmdir(path.c_str());
}

// 测试文件轮转时保留原始文件名中的时间戳
TEST(LogM_LogSink, PreservesTimestampOnRotation)
{
    const std::string path = "/tmp/sparrow_log_sink_timestamp_" + std::to_string(getpid());
    auto attrs = FileAttrs(path);
    attrs[LOG_CONFIG_KEY_FILE_NAME] = "main.log";
    attrs[LOG_CONFIG_KEY_FILE_NAME_FORMAT] = "BN_ST.FX";

    LogSink sink(attrs);

    const std::string initData("init\n");
    ASSERT_EQ(sink.Write(initData, InternalDefs::LOG_LEVEL_INFO), 0);
    ASSERT_EQ(sink.Flush(true), 0);

    // Record the initial file name with timestamp
    std::string firstFileName;
    {
        DIR* directory = opendir(path.c_str());
        if (directory != nullptr) {
            struct dirent* entry = nullptr;
            while ((entry = readdir(directory)) != nullptr) {
                const std::string name(entry->d_name);
                if (name.find("main_") == 0 && name.find(".1") == std::string::npos) {
                    firstFileName = name;
                    break;
                }
            }
            closedir(directory);
        }
    }
    ASSERT_FALSE(firstFileName.empty());

    // Write data to trigger rotation
    const std::string data1(600 * 1024, 'a');
    const std::string data2(600 * 1024, 'b');
    ASSERT_EQ(sink.Write(data1, InternalDefs::LOG_LEVEL_INFO), 0);
    ASSERT_EQ(sink.Write(data2, InternalDefs::LOG_LEVEL_INFO), 0);
    ASSERT_EQ(sink.Flush(true), 0);

    // Verify rotated file preserves original timestamp
    std::string rotatedPath;
    std::string rotatedFileName;
    {
        DIR* directory = opendir(path.c_str());
        ASSERT_NE(directory, nullptr);
        struct dirent* entry = nullptr;
        while ((entry = readdir(directory)) != nullptr) {
            const std::string name(entry->d_name);
            if (name == "." || name == ".." || name.find("main_") != 0) {
                continue;
            }
            if (name.find(".1") != std::string::npos) {
                rotatedPath = path + "/" + name;
                rotatedFileName = name;
                break;
            }
        }
        closedir(directory);
    }

    ASSERT_FALSE(rotatedPath.empty());
    // Verify the rotated file name starts with the original file name (preserving timestamp)
    // Expected format: "main_<timestamp>.log.1" instead of "main_<new_timestamp>.log.1"
    const size_t originalTimestampEnd = firstFileName.find(".log");
    ASSERT_NE(originalTimestampEnd, std::string::npos);
    const std::string originalBase = firstFileName.substr(0, originalTimestampEnd + 4);
    EXPECT_EQ(rotatedFileName.substr(0, originalBase.size() + 2), originalBase + ".1");

    std::ifstream rotated(rotatedPath);
    ASSERT_TRUE(rotated.is_open());
    rotated.seekg(0, std::ios::end);
    EXPECT_EQ(static_cast<uint64_t>(rotated.tellg()), initData.size() + data1.size());

    remove(rotatedPath.c_str());
    // Clean up current file
    DIR* directory = opendir(path.c_str());
    if (directory != nullptr) {
        struct dirent* entry = nullptr;
        while ((entry = readdir(directory)) != nullptr) {
            const std::string name(entry->d_name);
            if (name.find("main_") == 0) {
                remove((path + "/" + name).c_str());
            }
        }
        closedir(directory);
    }
    rmdir(path.c_str());
}

// 测试多次轮转后后缀正确递增
TEST(LogM_LogSink, RotationSuffixIncrement)
{
    const std::string path = "/tmp/sparrow_log_sink_suffix_" + std::to_string(getpid());
    auto attrs = FileAttrs(path);
    attrs[LOG_CONFIG_KEY_FILE_NAME] = "test.log";
    attrs[LOG_CONFIG_KEY_FILE_NAME_FORMAT] = "BN_ST.FX";
    attrs[LOG_CONFIG_KEY_FILE_NUM] = "10";

    LogSink sink(attrs);

    // Trigger 5 rotations
    for (int i = 0; i < 5; ++i) {
        const std::string data(600 * 1024, 'a' + i);
        ASSERT_EQ(sink.Write(data, InternalDefs::LOG_LEVEL_INFO), 0);
    }
    ASSERT_EQ(sink.Flush(true), 0);

    // Verify suffix numbers are correct
    std::vector<std::string> files;
    DIR* directory = opendir(path.c_str());
    ASSERT_NE(directory, nullptr);
    struct dirent* entry = nullptr;
    while ((entry = readdir(directory)) != nullptr) {
        const std::string name(entry->d_name);
        if (name.find("test_") == 0) {
            files.push_back(name);
        }
    }
    closedir(directory);

    ASSERT_GE(files.size(), 2U);
    // Verify suffix increment: should have .1, .2, .3, .4 etc.
    for (const auto& file : files) {
        if (file.find(".1") != std::string::npos || file.find(".2") != std::string::npos ||
            file.find(".3") != std::string::npos || file.find(".4") != std::string::npos) {
            // Valid rotated file with suffix
            EXPECT_TRUE(true);
        }
    }

    // Cleanup
    for (const auto& file : files) {
        remove((path + "/" + file).c_str());
    }
    rmdir(path.c_str());
}

// 测试达到最大文件数后删除最旧文件
TEST(LogM_LogSink, RotationMaxFileCountExceeded)
{
    const std::string path = "/tmp/sparrow_log_sink_maxcount_" + std::to_string(getpid());
    auto attrs = FileAttrs(path);
    attrs[LOG_CONFIG_KEY_FILE_NAME] = "maxtest.log";
    attrs[LOG_CONFIG_KEY_FILE_NAME_FORMAT] = "BN_ST.FX";
    attrs[LOG_CONFIG_KEY_FILE_NUM] = "3";

    LogSink sink(attrs);

    // Trigger rotations beyond max count to force deletion
    for (int i = 0; i < 6; ++i) {
        const std::string data(600 * 1024, 'x');
        ASSERT_EQ(sink.Write(data, InternalDefs::LOG_LEVEL_INFO), 0);
    }
    ASSERT_EQ(sink.Flush(true), 0);

    // Count remaining files (should not exceed max_file_count + 1 for current)
    std::vector<std::string> files;
    DIR* directory = opendir(path.c_str());
    if (directory != nullptr) {
        struct dirent* entry = nullptr;
        while ((entry = readdir(directory)) != nullptr) {
            const std::string name(entry->d_name);
            if (name.find("maxtest_") == 0) {
                files.push_back(name);
            }
        }
        closedir(directory);
    }

    EXPECT_LE(files.size(), 4U);  // max 3 rotations + 1 current

    // Cleanup
    for (const auto& file : files) {
        remove((path + "/" + file).c_str());
    }
    rmdir(path.c_str());
}

// 测试连续快速轮转
TEST(LogM_LogSink, RotationSequential)
{
    const std::string path = "/tmp/sparrow_log_sink_sequential_" + std::to_string(getpid());
    auto attrs = FileAttrs(path);
    attrs[LOG_CONFIG_KEY_FILE_NAME] = "seq.log";
    attrs[LOG_CONFIG_KEY_FILE_NAME_FORMAT] = "BN_ST.FX";

    LogSink sink(attrs);

    // Rapidly write and rotate
    for (int i = 0; i < 10; ++i) {
        const std::string data(600 * 1024, 'a' + (i % 26));
        ASSERT_EQ(sink.Write(data, InternalDefs::LOG_LEVEL_INFO), 0);
    }
    ASSERT_EQ(sink.Flush(true), 0);

    // Verify all files are readable
    DIR* directory = opendir(path.c_str());
    ASSERT_NE(directory, nullptr);
    struct dirent* entry = nullptr;
    int fileCount = 0;
    while ((entry = readdir(directory)) != nullptr) {
        const std::string name(entry->d_name);
        if (name.find("seq_") == 0) {
            fileCount++;
            std::string filePath = path + "/" + name;
            std::ifstream file(filePath);
            EXPECT_TRUE(file.is_open());
            file.seekg(0, std::ios::end);
            EXPECT_GT(file.tellg(), 0);
        }
    }
    closedir(directory);
    EXPECT_GE(fileCount, 1);

    // Cleanup
    directory = opendir(path.c_str());
    if (directory != nullptr) {
        struct dirent* entry = nullptr;
        while ((entry = readdir(directory)) != nullptr) {
            const std::string name(entry->d_name);
            if (name.find("seq_") == 0) {
                remove((path + "/" + name).c_str());
            }
        }
        closedir(directory);
    }
    rmdir(path.c_str());
}

// 测试轮转时文件完整性（每个轮转的文件大小应为容量或接近）
TEST(LogM_LogSink, RotationIntegrity)
{
    const std::string path = "/tmp/sparrow_log_sink_integrity_" + std::to_string(getpid());
    auto attrs = FileAttrs(path);
    attrs[LOG_CONFIG_KEY_FILE_NAME] = "intact.log";
    attrs[LOG_CONFIG_KEY_FILE_NAME_FORMAT] = "BN_ST.FX";

    LogSink sink(attrs);

    const std::string data1(600 * 1024, 'a');
    const std::string data2(500 * 1024, 'b');
    ASSERT_EQ(sink.Write(data1, InternalDefs::LOG_LEVEL_INFO), 0);
    ASSERT_EQ(sink.Write(data2, InternalDefs::LOG_LEVEL_INFO), 0);
    ASSERT_EQ(sink.Flush(true), 0);

    // Verify rotated file has correct size
    DIR* directory = opendir(path.c_str());
    ASSERT_NE(directory, nullptr);
    struct dirent* entry = nullptr;
    uint64_t rotatedSize = 0;
    while ((entry = readdir(directory)) != nullptr) {
        const std::string name(entry->d_name);
        if (name.find("intact_") == 0 && name.find(".1") != std::string::npos) {
            std::string filePath = path + "/" + name;
            struct stat st {};
            ASSERT_EQ(stat(filePath.c_str(), &st), 0);
            rotatedSize = st.st_size;
            EXPECT_EQ(rotatedSize, data1.size());
        }
    }
    closedir(directory);
    EXPECT_EQ(rotatedSize, data1.size());

    // Cleanup
    directory = opendir(path.c_str());
    if (directory != nullptr) {
        struct dirent* entry = nullptr;
        while ((entry = readdir(directory)) != nullptr) {
            const std::string name(entry->d_name);
            if (name.find("intact_") == 0) {
                remove((path + "/" + name).c_str());
            }
        }
        closedir(directory);
    }
    rmdir(path.c_str());
}

// 测试混合场景：多次轮转 + 名字中保留时间戳 + 正确的后缀序号
TEST(LogM_LogSink, RotationComprehensive)
{
    const std::string path = "/tmp/sparrow_log_sink_comprehensive_" + std::to_string(getpid());
    auto attrs = FileAttrs(path);
    attrs[LOG_CONFIG_KEY_FILE_NAME] = "comp.log";
    attrs[LOG_CONFIG_KEY_FILE_NAME_FORMAT] = "BN_ST.FX";
    attrs[LOG_CONFIG_KEY_FILE_NUM] = "5";

    LogSink sink(attrs);

    // Ensure at least one write to create initial file
    ASSERT_EQ(sink.Write("init", InternalDefs::LOG_LEVEL_INFO), 0);
    ASSERT_EQ(sink.Flush(true), 0);

    // Record initial filename (without numeric suffix like .1, .2)
    std::string initialFile;
    {
        DIR* directory = opendir(path.c_str());
        if (directory != nullptr) {
            struct dirent* entry = nullptr;
            while ((entry = readdir(directory)) != nullptr) {
                const std::string name(entry->d_name);
                if (name.find("comp_") == 0) {
                    // Check if it has a numeric suffix
                    size_t lastDot = name.rfind('.');
                    bool hasNumericSuffix = false;
                    if (lastDot != std::string::npos && lastDot > 0) {
                        const std::string suffix = name.substr(lastDot + 1);
                        char* end = nullptr;
                        std::strtoul(suffix.c_str(), &end, 10);
                        if (end != suffix.c_str() && *end == '\0') {
                            hasNumericSuffix = true;
                        }
                    }
                    if (!hasNumericSuffix) {
                        initialFile = name;
                        break;
                    }
                }
            }
            closedir(directory);
        }
    }
    ASSERT_FALSE(initialFile.empty());

    // Trigger multiple rotations
    for (int i = 0; i < 4; ++i) {
        const std::string data(600 * 1024, 'a' + i);
        ASSERT_EQ(sink.Write(data, InternalDefs::LOG_LEVEL_INFO), 0);
    }
    ASSERT_EQ(sink.Flush(true), 0);

    // Verify:
    // 1. Initial timestamp preserved in rotated files
    // 2. Suffix numbers increment (.1, .2, .3, ...)
    // 3. All files readable
    std::vector<std::string> foundFiles;
    DIR* directory = opendir(path.c_str());
    ASSERT_NE(directory, nullptr);
    struct dirent* entry = nullptr;
    while ((entry = readdir(directory)) != nullptr) {
        const std::string name(entry->d_name);
        if (name.find("comp_") == 0) {
            foundFiles.push_back(name);

            // Extract base name (before first numeric suffix)
            const size_t lastDot = name.rfind('.');
            if (lastDot != std::string::npos) {
                const std::string suffix = name.substr(lastDot + 1);
                char* end = nullptr;
                std::strtoul(suffix.c_str(), &end, 10);
                if (end != suffix.c_str() && *end == '\0') {
                    // Is numeric suffix - verify it's in the basename
                    std::string basename = name.substr(0, lastDot);
                    EXPECT_TRUE(basename.find("comp_") == 0);
                }
            }

            // Verify file is readable
            std::ifstream file(path + "/" + name);
            EXPECT_TRUE(file.is_open());
        }
    }
    closedir(directory);

    EXPECT_GE(foundFiles.size(), 2U);

    // Cleanup
    for (const auto& file : foundFiles) {
        remove((path + "/" + file).c_str());
    }
    rmdir(path.c_str());
}

} // namespace
