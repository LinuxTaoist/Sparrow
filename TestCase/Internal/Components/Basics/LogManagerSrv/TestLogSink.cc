#include <string>
#include <fstream>
#include <dirent.h>
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

} // namespace
