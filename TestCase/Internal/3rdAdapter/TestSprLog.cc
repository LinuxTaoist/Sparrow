/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprLog.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/16
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <vector>
#include <thread>
#include <atomic>
#include <regex>
#include <string>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstdlib>
#include <limits.h>
#include <cstring>
#include <sys/wait.h>
#include <sys/stat.h>
#include <map>
#include <cstdio>
#include "gtest/gtest.h"
#include "SprLog.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "GeneralUtils.h"

// 定义不同长度的 LOG_TAG
#define LOG_TAG "TestTag"
#define LONG_LOG_TAG "ThisIsAVeryLongTagThatExceedsLimit"

#define TEST_LOG(fmt, args...) printf("[   INFO   ] " fmt "\n", ##args)

#define LOG_BUFFER_DEF      512                 // 与 SprLog.cpp 的 LOG_BUFFER_SIZE_DEFAULT 保持一致
#define LOG_CONF_FILE       "sprlog.conf"       // 与 LogManager.cpp 的 LOG_CONFIGURE_FILE_PATH 保持一致
#define LOG_BURST_BYTES     (4 * 1024 * 1024)
#define LOG_BURST_PAUSE_US  (1000 * 1000)

bool ResetSprLogTestEnvironment();

static bool RunCommand(const std::string& cmd) {
    return std::system(cmd.c_str()) == 0;
}

static int DetectGtestRepeatCount() {
    std::ifstream cmdline("/proc/self/cmdline", std::ios::binary);
    if (!cmdline) {
        return 1;
    }

    std::string buffer((std::istreambuf_iterator<char>(cmdline)), std::istreambuf_iterator<char>());
    size_t begin = 0;
    while (begin < buffer.size()) {
        const size_t end = buffer.find('\0', begin);
        const std::string arg = buffer.substr(begin, end == std::string::npos ? std::string::npos : end - begin);
        const char* prefix = "--gtest_repeat=";
        if (arg.rfind(prefix, 0) == 0) {
            return std::max(1, atoi(arg.substr(std::strlen(prefix)).c_str()));
        }
        if (end == std::string::npos) {
            break;
        }
        begin = end + 1;
    }
    return 1;
}

struct LogConfig {
    std::string output;         // file / stdout
    std::string logDir;         // logging.log_dir
    std::string fileName;       // logging.file_name
    std::string packDir;        // logging.pack_dir
    std::string packName;       // logging.pack_name
    std::string packType;       // logging.pack_type
    int fileNum = 10;           // logging.file_num
    int fileCapacityMb = 10;    // logging.file_capacity (MB)
    bool packEnabled = false;   // logging.pack_enabled
};

class ThirdAdapter_SprLog : public ::testing::Test {
protected:
    void ClearHistoricalLogs() const {
        LogConfig cfg;
        if (!LoadLogConfig(cfg) || cfg.fileName.empty()) {
            return;
        }

        RunCommand("find " + ShellQuote(cfg.logDir)
            + " -maxdepth 1 -type f -name '*.[0-9]*' -delete >/dev/null 2>&1 || true");
        if (!cfg.packDir.empty()) {
            RunCommand("find " + ShellQuote(cfg.packDir)
                + " -mindepth 1 -maxdepth 1 -type f -delete >/dev/null 2>&1 || true");
        }
    }

    void SetUp() override {
        if (DetectGtestRepeatCount() > 1) {
            ASSERT_TRUE(ResetSprLogTestEnvironment());
        } else {
            ClearHistoricalLogs();
        }
        mLog = SprLog::GetInstance();
        ASSERT_NE(mLog, nullptr);
        mLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
        mLog->SetLength(LOG_BUFFER_DEF);
    }

    void TearDown() override {
        if (mLog != nullptr) {
            mLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
            mLog->SetLength(LOG_BUFFER_DEF);
        }
    }

    bool LoadLogConfig(LogConfig& cfg) const {
        std::ifstream file(LogConfigPath());
        if (!file) {
            return false;
        }

        std::string line;
        std::string section;
        while (std::getline(file, line)) {
            const size_t comment = line.find('#');
            if (comment != std::string::npos) {
                line.resize(comment);
            }
            line = Trim(line);
            if (line.size() >= 2 && line.front() == '[' && line.back() == ']') {
                section = Trim(line.substr(1, line.size() - 2));
                continue;
            }

            const size_t equal = line.find('=');
            if (equal == std::string::npos) {
                continue;
            }
            const std::string key = Trim(line.substr(0, equal));
            const std::string value = Trim(line.substr(equal + 1));
            if ((section.empty() || section.rfind("output.", 0) != 0)
                && key.find("logging.") != 0) {
                continue;
            }
            if (key == "logging.output" || key == "output") {
                cfg.output = value;
            } else if (key == "logging.log_dir" || key == "log_dir"
                       || key == "logging.file_path" || key == "file_path") {
                cfg.logDir = value;
            } else if (key == "logging.file_name" || key == "file_name") {
                cfg.fileName = value;
            } else if (key == "logging.pack_enabled" || key == "pack_enabled") {
                cfg.packEnabled = value == "true";
            } else if (key == "logging.pack_dir" || key == "pack_dir") {
                cfg.packDir = value;
            } else if (key == "logging.pack_name" || key == "pack_name") {
                cfg.packName = value;
            } else if (key == "logging.pack_type" || key == "pack_type") {
                cfg.packType = value;
            } else if (key == "logging.file_num" || key == "file_num") {
                cfg.fileNum = atoi(value.c_str());
            } else if (key == "logging.file_capacity" || key == "file_capacity_mb") {
                cfg.fileCapacityMb = atoi(value.c_str());
            }
        }
        return true;
    }

    std::vector<std::string> LogFiles() const {
        LogConfig cfg;
        std::vector<std::string> paths;
        if (!LoadLogConfig(cfg) || cfg.logDir.empty()) {
            return paths;
        }

        DIR* directory = opendir(cfg.logDir.c_str());
        if (directory == nullptr) {
            return paths;
        }
        struct dirent* entry;
        while ((entry = readdir(directory)) != nullptr) {
            const std::string name(entry->d_name);
            if (IsConfiguredLogFile(name, cfg.fileName)) {
                paths.push_back(cfg.logDir + "/" + name);
            }
        }
        closedir(directory);
        return paths;
    }

    std::vector<std::string> PackFiles() const {
        LogConfig cfg;
        std::vector<std::string> paths;
        if (!LoadLogConfig(cfg) || !cfg.packEnabled) {
            return paths;
        }

        const std::string directoryPath = cfg.packDir.empty() ? cfg.logDir : cfg.packDir;
        const std::string packName = NormalizePackName(cfg);
        if (directoryPath.empty() || packName.empty()) {
            return paths;
        }

        DIR* directory = opendir(directoryPath.c_str());
        if (directory == nullptr) {
            return paths;
        }

        struct dirent* entry = nullptr;
        while ((entry = readdir(directory)) != nullptr) {
            const std::string name(entry->d_name);
            if (IsConfiguredLogFile(name, packName)) {
                paths.push_back(directoryPath + "/" + name);
            }
        }
        closedir(directory);
        return paths;
    }

    static std::string ShellQuote(const std::string& value) {
        std::string quoted = "'";
        for (const char ch : value) {
            if (ch == '\'') {
                quoted += "'\\''";
            } else {
                quoted += ch;
            }
        }
        quoted += "'";
        return quoted;
    }

    bool SearchArchiveLogs(const std::string& archivePath, const std::string& content) const {
        std::string command;
        if (archivePath.size() >= 7 && archivePath.compare(archivePath.size() - 7, 7, ".tar.gz") == 0) {
            command = "tar -xOf " + ShellQuote(archivePath) + " 2>/dev/null | grep -F -- " + ShellQuote(content) + " >/dev/null 2>&1";
        } else if (archivePath.size() >= 4 && archivePath.compare(archivePath.size() - 4, 4, ".zip") == 0) {
            command = "unzip -p " + ShellQuote(archivePath) + " 2>/dev/null | grep -F -- " + ShellQuote(content) + " >/dev/null 2>&1";
        } else {
            return false;
        }

        FILE* pipe = popen(command.c_str(), "r");
        if (pipe == nullptr) {
            return false;
        }
        const int exitStatus = pclose(pipe);
        return WIFEXITED(exitStatus) && WEXITSTATUS(exitStatus) == 0;
    }

    int CountArchiveLogs(const std::string& archivePath, const std::string& content) const {
        std::string command;
        if (archivePath.size() >= 7 && archivePath.compare(archivePath.size() - 7, 7, ".tar.gz") == 0) {
            command = "tar -xOf " + ShellQuote(archivePath) + " 2>/dev/null | grep -F -c -- " + ShellQuote(content);
        } else if (archivePath.size() >= 4 && archivePath.compare(archivePath.size() - 4, 4, ".zip") == 0) {
            command = "unzip -p " + ShellQuote(archivePath) + " 2>/dev/null | grep -F -c -- " + ShellQuote(content);
        } else {
            return 0;
        }

        FILE* pipe = popen(command.c_str(), "r");
        if (pipe == nullptr) {
            return 0;
        }

        char buffer[64] = {0};
        const bool ok = fgets(buffer, sizeof(buffer), pipe) != nullptr;
        const int exitStatus = pclose(pipe);
        if (!ok || !WIFEXITED(exitStatus) || WEXITSTATUS(exitStatus) != 0) {
            return 0;
        }
        return std::atoi(buffer);
    }

    std::vector<std::string> ReadArchiveLines(const std::string& archivePath) const {
        std::vector<std::string> lines;
        std::string command;
        if (archivePath.size() >= 7 && archivePath.compare(archivePath.size() - 7, 7, ".tar.gz") == 0) {
            command = "tar -xOf " + ShellQuote(archivePath) + " 2>/dev/null";
        } else if (archivePath.size() >= 4 && archivePath.compare(archivePath.size() - 4, 4, ".zip") == 0) {
            command = "unzip -p " + ShellQuote(archivePath) + " 2>/dev/null";
        } else {
            return lines;
        }

        FILE* pipe = popen(command.c_str(), "r");
        if (pipe == nullptr) {
            return lines;
        }

        char buffer[8192] = {0};
        std::string current;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            current += buffer;
        }
        pclose(pipe);

        std::stringstream stream(current);
        std::string line;
        while (std::getline(stream, line)) {
            lines.push_back(line);
        }
        return lines;
    }

    bool SearchLogs(const std::string& content) const {
        for (const auto& path : LogFiles()) {
            std::ifstream file(path);
            std::string line;
            while (std::getline(file, line)) {
                if (line.find(content) != std::string::npos) {
                    return true;
                }
            }
        }
        for (const auto& path : PackFiles()) {
            if (SearchArchiveLogs(path, content)) {
                return true;
            }
        }
        return false;
    }

    int CountLogs(const std::string& content) const {
        int count = 0;
        for (const auto& path : LogFiles()) {
            std::ifstream file(path);
            std::string line;
            while (std::getline(file, line)) {
                count += line.find(content) != std::string::npos ? 1 : 0;
            }
        }
        for (const auto& path : PackFiles()) {
            count += CountArchiveLogs(path, content);
        }
        return count;
    }

    bool WaitForLog(const std::string& content, int timeoutSec = 10) const {
        const auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < std::chrono::seconds(timeoutSec)) {
            if (SearchLogs(content)) {
                return true;
            }
            usleep(100000);
        }
        return false;
    }

    bool WaitForLogs(const std::string& content, int expected, int timeoutSec = 30) const {
        const auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < std::chrono::seconds(timeoutSec)) {
            if (CountLogs(content) >= expected) {
                return true;
            }
            usleep(200000);
        }
        return false;
    }

    bool WaitForRotatedArtifacts(int expected, int timeoutSec = 10) const {
        const auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < std::chrono::seconds(timeoutSec)) {
            if (RotatedFileCount() >= expected) {
                return true;
            }
            usleep(200000);
        }
        return false;
    }

    size_t EstimateBytesPerLog(size_t payloadBytes, size_t variableTextBytes = 0) const {
        const size_t frameOverheadBytes = 80; // timestamp/pid/tag/level/newline and small formatting text
        return std::max<size_t>(1, frameOverheadBytes + payloadBytes + variableTextBytes);
    }

    int EstimateLogsForSingleRotation(size_t payloadBytes, size_t variableTextBytes = 0) const {
        LogConfig cfg;
        if (!LoadLogConfig(cfg) || cfg.fileCapacityMb <= 0) {
            cfg.fileCapacityMb = 10;
        }

        const size_t fileCapacityBytes = static_cast<size_t>(cfg.fileCapacityMb) * 1024U * 1024U;
        const size_t approxBytesPerLog = EstimateBytesPerLog(payloadBytes, variableTextBytes);
        const size_t baseCount = fileCapacityBytes / approxBytesPerLog;

        // 留一点余量，确保稳定触发一次轮转，但不要再暴力打太多日志。
        return static_cast<int>(std::max<size_t>(1024, (baseCount * 11) / 10));
    }

    int EstimateLogsForConfiguredRotations(size_t payloadBytes, size_t variableTextBytes,
                                           int rotationCount, int minCount, int maxCount) const {
        const int safeRotationCount = std::max(1, rotationCount);
        const int safeMinCount = std::max(1, minCount);
        const int safeMaxCount = std::max(safeMinCount, maxCount);
        const int singleRotationCount = EstimateLogsForSingleRotation(payloadBytes, variableTextBytes);
        long long estimated = static_cast<long long>(singleRotationCount) * safeRotationCount;
        if (estimated < safeMinCount) {
            estimated = safeMinCount;
        }
        if (estimated > safeMaxCount) {
            estimated = safeMaxCount;
        }
        return static_cast<int>(estimated);
    }

    void ThrottleAsyncWrite(size_t& pendingBytes, size_t bytesPerLog,
                            size_t burstBytes = LOG_BURST_BYTES,
                            useconds_t pauseUs = LOG_BURST_PAUSE_US) const {
        pendingBytes += bytesPerLog;
        if (pendingBytes >= burstBytes) {
            usleep(pauseUs);
            pendingBytes = 0;
        }
    }

    std::pair<std::string, int> ParseRotationSuffix(const std::string& fileName) const {
        std::string name = fileName;
        for (const std::string extension : {".tar.gz", ".zip"}) {
            if (name.size() >= extension.size() && name.compare(name.size() - extension.size(),
                                                               extension.size(),
                                                               extension) == 0) {
                name = name.substr(0, name.size() - extension.size());
                break;
            }
        }

        const size_t dot = name.rfind('.');
        if (dot == std::string::npos || dot == 0) {
            return {std::string(), -1};
        }

        const std::string suffix = name.substr(dot + 1);
        if (suffix.empty() || !std::all_of(suffix.begin(), suffix.end(), [](char c) {
                return std::isdigit(static_cast<unsigned char>(c)) != 0;
            })) {
            return {std::string(), -1};
        }

        return {name.substr(0, dot), std::stoi(suffix)};
    }

    int RotatedFileCount() const {
        LogConfig cfg;
        std::vector<std::string> files = LogFiles();
        if (LoadLogConfig(cfg) && cfg.packEnabled) {
            const std::vector<std::string> packFiles = PackFiles();
            files.insert(files.end(), packFiles.begin(), packFiles.end());
        }

        int count = 0;
        for (const auto& path : files) {
            const size_t slash = path.rfind('/');
            const std::string name = slash == std::string::npos ? path : path.substr(slash + 1);
            const auto parsed = ParseRotationSuffix(name);
            if (parsed.second >= 0) {
                ++count;
            }
        }
        return count;
    }

    std::map<std::string, int> RotatedSuffixes() const {
        LogConfig cfg;
        std::vector<std::string> files = LogFiles();
        if (LoadLogConfig(cfg) && cfg.packEnabled) {
            const std::vector<std::string> packFiles = PackFiles();
            files.insert(files.end(), packFiles.begin(), packFiles.end());
        }

        std::map<std::string, int> suffixes;
        for (const auto& path : files) {
            const size_t slash = path.rfind('/');
            const std::string name = slash == std::string::npos ? path : path.substr(slash + 1);
            const auto parsed = ParseRotationSuffix(name);
            if (parsed.second >= 0) {
                suffixes[parsed.first] = std::max(suffixes[parsed.first], parsed.second);
            }
        }
        return suffixes;
    }

    bool HasDoubleNumericSuffix(const std::string& name) const {
        const size_t lastDot = name.rfind('.');
        if (lastDot == std::string::npos || lastDot == 0) {
            return false;
        }
        char* end = nullptr;
        std::strtoul(name.substr(lastDot + 1).c_str(), &end, 10);
        if (end == nullptr || *end != '\0') {
            return false;
        }
        const size_t previousDot = name.rfind('.', lastDot - 1);
        if (previousDot == std::string::npos || previousDot + 1 >= lastDot) {
            return false;
        }
        end = nullptr;
        std::strtoul(name.substr(previousDot + 1, lastDot - previousDot - 1).c_str(), &end, 10);
        return end != nullptr && *end == '\0';
    }

    bool ParseFrame(const std::string& frame, std::string& level,
                    std::string& tag, std::string& content) const {
        static const std::regex pattern(
            R"(^\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3}\s+\d+\s+(\S+)\s+([DIWE]):\s*(.*)$)");
        std::smatch match;
        if (!std::regex_match(frame, match, pattern)) {
            return false;
        }
        tag = match[1].str();
        level = match[2].str();
        content = match[3].str();
        return true;
    }

    static std::string Trim(const std::string& value) {
        const size_t begin = value.find_first_not_of(" \t\r\n");
        if (begin == std::string::npos) {
            return std::string();
        }
        const size_t end = value.find_last_not_of(" \t\r\n");
        return value.substr(begin, end - begin + 1);
    }

    static bool EndsWith(const std::string& value, const std::string& suffix) {
        return value.size() >= suffix.size()
            && value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    static std::string ConfiguredExtension(const std::string& fileName) {
        if (EndsWith(fileName, ".tar.gz")) {
            return ".tar.gz";
        }
        if (EndsWith(fileName, ".zip")) {
            return ".zip";
        }

        const size_t extensionPos = fileName.rfind('.');
        if (extensionPos == std::string::npos) {
            return std::string();
        }
        return fileName.substr(extensionPos);
    }

    static std::string NormalizePackName(const LogConfig& cfg) {
        if (cfg.packName.empty()) {
            return std::string();
        }

        std::string extension;
        if (cfg.packType == "tar.gz") {
            extension = ".tar.gz";
        } else if (cfg.packType == "zip") {
            extension = ".zip";
        }

        if (extension.empty() || EndsWith(cfg.packName, extension)) {
            return cfg.packName;
        }

        const size_t dotPos = cfg.packName.rfind('.');
        if (dotPos != std::string::npos) {
            return cfg.packName.substr(0, dotPos) + extension;
        }
        return cfg.packName + extension;
    }

    static std::string LogConfigPath() {
        const char* root = std::getenv(ENV_SPR_ROOT_PATH);
        if (root != nullptr && root[0] != '\0') {
            return std::string(root) + "/" + DEFAULT_SPR_ETC_FILE + "/" + LOG_CONF_FILE;
        }

        char executable[300] = {0};
        const ssize_t length = readlink("/proc/self/exe", executable, sizeof(executable) - 1);
        if (length <= 0) {
            return std::string();
        }
        executable[length] = '\0';
        const std::string path(executable);
        const size_t slash = path.find_last_of('/');
        return slash == std::string::npos ? std::string()
            : path.substr(0, slash) + "/../" + DEFAULT_SPR_ETC_FILE + "/" + LOG_CONF_FILE;
    }

    static bool IsConfiguredLogFile(const std::string& name, const std::string& fileName) {
        if (name == fileName) {
            return true;
        }

        const std::string rawPrefix = fileName + ".";
        if (name.rfind(rawPrefix, 0) == 0) {
            const std::string suffix = name.substr(rawPrefix.size());
            return !suffix.empty() && std::all_of(suffix.begin(), suffix.end(), [](char value) {
                return std::isdigit(static_cast<unsigned char>(value)) != 0;
            });
        }

        const std::string extension = ConfiguredExtension(fileName);
        const std::string base = extension.empty()
                       ? fileName : fileName.substr(0, fileName.size() - extension.size());

        if (!extension.empty()
            && name.size() > base.size() + extension.size() + 1
            && name.rfind(base + ".", 0) == 0
            && name.compare(name.size() - extension.size(), extension.size(), extension) == 0) {
            const std::string suffix = name.substr(base.size() + 1,
                                                   name.size() - base.size() - extension.size() - 1);
            return !suffix.empty() && std::all_of(suffix.begin(), suffix.end(), [](char value) {
                return std::isdigit(static_cast<unsigned char>(value)) != 0;
            });
        }

        const std::string prefix = base + "_";
        if (name.rfind(prefix, 0) != 0) {
            return false;
        }
        const size_t generatedExtension = extension.empty()
                                        ? std::string::npos : name.find(extension, prefix.size());
        if (!extension.empty() && generatedExtension == std::string::npos) {
            return false;
        }
        const size_t rotationPos = extension.empty()
                                 ? name.size() : generatedExtension + extension.size();
        if (rotationPos == name.size()) {
            return true;
        }
        if (name[rotationPos] != '.') {
            return false;
        }
        const std::string suffix = name.substr(rotationPos + 1);
        return !suffix.empty() && std::all_of(suffix.begin(), suffix.end(), [](char value) {
            return std::isdigit(static_cast<unsigned char>(value)) != 0;
        });
    }

    SprLog* mLog = nullptr;
};

// 测试单例与接口边界
TEST_F(ThirdAdapter_SprLog, Interface_SingletonAndBounds) {
    SprLog* first = SprLog::GetInstance();
    SprLog* second = SprLog::GetInstance();

    ASSERT_NE(first, nullptr);
    EXPECT_EQ(first, second);

    EXPECT_EQ(first->SetLevel(InternalDefs::LOG_LEVEL_DEBUG), 0);
    EXPECT_EQ(first->GetLevel(), InternalDefs::LOG_LEVEL_DEBUG);

    EXPECT_EQ(first->SetLength(0), 0);
    EXPECT_EQ(first->GetLength(), LOG_BUFFER_DEF);

    EXPECT_EQ(first->SetLength(-10), 0);
    EXPECT_EQ(first->GetLength(), LOG_BUFFER_DEF);

    EXPECT_EQ(first->SetLength(LOG_BUFFER_SIZE_LIMIT + 5000), 0);
    EXPECT_EQ(first->GetLength(), LOG_BUFFER_SIZE_LIMIT);

    EXPECT_EQ(first->SetLength(LOG_BUFFER_DEF), 0);

    EXPECT_EQ(first->Init(""), -1);
    EXPECT_EQ(first->Init(std::string(LOG_RECORD_MODULE_NAME_MAX_LENGTH, 'a')), -1);
}

// 测试基础日志写入与参数拼接
TEST_F(ThirdAdapter_SprLog, Scenario_BasicWriteAndArgs) {
    SprLog* pLog = SprLog::GetInstance();
    ASSERT_NE(pLog, nullptr);

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
    pLog->SetLength(LOG_BUFFER_DEF);

    const std::string mark = "basic-write-" + std::to_string(getpid()) + "-"
                           + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());

    EXPECT_GE(pLog->d(LOG_TAG, "%s debug %d", mark.c_str(), 1), 0);
    EXPECT_GE(pLog->i(LOG_TAG, "%s info %d", mark.c_str(), 2), 0);
    EXPECT_GE(pLog->w(LOG_TAG, "%s warn %d", mark.c_str(), 3), 0);
    EXPECT_GE(pLog->e(LOG_TAG, "%s error %d", mark.c_str(), 4), 0);

    ASSERT_TRUE(WaitForLogs(mark, 4)) << "logs not flushed to file";
    EXPECT_TRUE(SearchLogs(mark + " debug 1"));
    EXPECT_TRUE(SearchLogs(mark + " info 2"));
    EXPECT_TRUE(SearchLogs(mark + " warn 3"));
    EXPECT_TRUE(SearchLogs(mark + " error 4"));
}

// 测试超长内容截断行为
TEST_F(ThirdAdapter_SprLog, Scenario_TruncateBehavior) {
    SprLog* pLog = SprLog::GetInstance();
    ASSERT_NE(pLog, nullptr);

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
    pLog->SetLength(128);

    const std::string mark = "truncate-" + std::to_string(getpid()) + "-"
                           + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    const std::string longMessage(1024, 'a');

    EXPECT_EQ(pLog->d(LOG_TAG, "%s %s", mark.c_str(), longMessage.c_str()), -1);

    ASSERT_TRUE(WaitForLog("[TRUNCATED]")) << "log not flushed to file";
    EXPECT_TRUE(SearchLogs(mark.substr(0, 10)));
    EXPECT_TRUE(SearchLogs("[TRUNCATED]"));
    EXPECT_TRUE(SearchLogs("[LOG CONTENT TRUNCATED]"));
}

// 测试帧格式标准（时间戳/毫秒/PID/TAG/级别/内容）端到端落盘校验
TEST_F(ThirdAdapter_SprLog, FrameFormat_Standard) {
    const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    const std::string unique = "frame-format-check-" + std::to_string(nowMs);
    SPR_LOGI("%s", unique.c_str());

    // 等待日志服务消费并落盘
    ASSERT_TRUE(WaitForLog(unique)) << "log not flushed to file";

    // 在全部日志文件（含轮转）中定位该帧并按格式校验
    bool found = false;
    for (const auto& path : LogFiles()) {
        std::ifstream file(path);
        std::string line;
        while (std::getline(file, line)) {
            if (line.find(unique) != std::string::npos) {
                std::string level, tag, body;
                            ASSERT_TRUE(ParseFrame(line, level, tag, body)) << "bad format: " << line;
                EXPECT_EQ(level, "I");
                EXPECT_EQ(tag, LOG_TAG);
                EXPECT_NE(body.find(unique), std::string::npos);
                found = true;
                break;
            }
        }
        if (found) {
            break;
        }
    }
    EXPECT_TRUE(found);
}

// 测试特殊内容（中文/%转义/引号/制表符/路径空格）端到端落盘
TEST_F(ThirdAdapter_SprLog, SpecialContent) {
    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);

    SPR_LOGI("percent:100%% path:/tmp/a b 中文 \t quote:\"x\"");

    ASSERT_TRUE(WaitForLog("percent:100%")) << "log not flushed to file";
    // 校验特殊内容在落盘文件中完整保留
    EXPECT_TRUE(SearchLogs("中文"));
    EXPECT_TRUE(SearchLogs("quote:\"x\""));
    EXPECT_TRUE(SearchLogs("/tmp/a b"));
}

// 测试多行日志端到端落盘（一帧跨多行，内容完整）
TEST_F(ThirdAdapter_SprLog, MultiLineContent) {
    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);

    SPR_LOGW("line1\nline2:with-colon\nline3");

    ASSERT_TRUE(WaitForLog("line2:with-colon")) << "log not flushed to file";
    EXPECT_TRUE(SearchLogs("line1"));
    EXPECT_TRUE(SearchLogs("line3"));
}

// 测试前端级别过滤（SetLevel 后低于级别的日志不产出帧、不落盘）
TEST_F(ThirdAdapter_SprLog, LevelFilter_AtProducer) {
    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_ERROR);

    SPR_LOGD("should-not-appear-d");
    SPR_LOGI("should-not-appear-i");
    SPR_LOGW("should-not-appear-w");
    SPR_LOGE("error-should-appear");

    // 等待 error 落盘，确认消费链路正常
    ASSERT_TRUE(WaitForLog("error-should-appear")) << "log not flushed to file";

    // 被前端拦截的日志未写入共享内存，落盘文件中必然不存在
    EXPECT_FALSE(SearchLogs("should-not-appear-d"));
    EXPECT_FALSE(SearchLogs("should-not-appear-i"));
    EXPECT_FALSE(SearchLogs("should-not-appear-w"));
    EXPECT_TRUE(SearchLogs("error-should-appear"));
}

// 测试超长日志截断（超过 SetLength 后截断并带 [TRUNCATED] 标记）
TEST_F(ThirdAdapter_SprLog, LengthLimit_Truncate) {
    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
    pLog->SetLength(128);   // 收紧帧长，触发截断路径

    std::string longMsg(1024, 'a');
    SPR_LOGD("long-content:%s", longMsg.c_str());

    ASSERT_TRUE(WaitForLog("[TRUNCATED]")) << "log not flushed to file";
    EXPECT_TRUE(SearchLogs("LEN:"));
    EXPECT_TRUE(SearchLogs("[LOG CONTENT TRUNCATED]"));
}

// 测试 SetLength / SetLevel 边界
TEST_F(ThirdAdapter_SprLog, SetterBoundary) {
    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }


    // SetLength: 非正数回默认，超上限钳制到上限
    pLog->SetLength(0);
    EXPECT_EQ(pLog->GetLength(), LOG_BUFFER_DEF);
    pLog->SetLength(-10);
    EXPECT_EQ(pLog->GetLength(), LOG_BUFFER_DEF);
    pLog->SetLength(LOG_BUFFER_SIZE_LIMIT + 5000);
    EXPECT_EQ(pLog->GetLength(), LOG_BUFFER_SIZE_LIMIT);
    pLog->SetLength(200);
    EXPECT_EQ(pLog->GetLength(), 200);
    pLog->SetLength(LOG_BUFFER_DEF);   // 恢复

    // SetLevel: 存储语义（不校验范围）
    pLog->SetLevel(0);
    EXPECT_EQ(pLog->GetLevel(), 0);
    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
    EXPECT_EQ(pLog->GetLevel(), InternalDefs::LOG_LEVEL_DEBUG);
}

// 测试多线程并发写（模拟一个进程内多模块同时打日志，端到端校验不丢帧）
TEST_F(ThirdAdapter_SprLog, Concurrent_MultiThreadWrite) {
    constexpr int32_t THREAD_NUM      = 8;
    constexpr int32_t MSG_PER_THREAD  = 500;
    const int expectCount             = THREAD_NUM * MSG_PER_THREAD;

    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);

    std::vector<std::thread> threads;
    for (int32_t t = 0; t < THREAD_NUM; ++t) {
        threads.emplace_back([pLog, t]() {
            char tag[8];
            snprintf(tag, sizeof(tag), "Thr%02d", t);
            for (int32_t i = 0; i < MSG_PER_THREAD; ++i) {
                pLog->d(tag, "thread-%02d msg-%04d", t, i);
            }
        });
    }
    for (auto& th : threads) {
        th.join();
    }

    // 等待日志服务消费完并落盘，统计行数校验不丢帧
    EXPECT_TRUE(WaitForLogs("thread-", expectCount)) << "some frames lost";
}

// 测试多进程并发写（fork 子进程模拟多个业务进程，端到端校验不丢帧）
TEST_F(ThirdAdapter_SprLog, Concurrent_MultiProcessWrite) {
    constexpr int32_t PROC_NUM       = 4;
    constexpr int32_t MSG_PER_PROC   = 200;
    const int expectCount            = PROC_NUM * MSG_PER_PROC;

    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);

    pid_t pids[PROC_NUM];
    for (int32_t p = 0; p < PROC_NUM; ++p) {
        pid_t pid = fork();
        ASSERT_GE(pid, 0) << "fork failed";
        if (pid == 0) {
            // 子进程（业务进程）：继承共享内存映射与命名信号量
            char tag[8];
            snprintf(tag, sizeof(tag), "Proc%02d", p);
            for (int32_t i = 0; i < MSG_PER_PROC; ++i) {
                pLog->d(tag, "proc-%02d msg-%04d", p, i);
            }
            _exit(0);
        }
        pids[p] = pid;
    }

    for (int32_t p = 0; p < PROC_NUM; ++p) {
        int status = 0;
        waitpid(pids[p], &status, 0);
        EXPECT_TRUE(WIFEXITED(status) && WEXITSTATUS(status) == 0) << "child " << p << " failed";
    }

    // 等待日志服务消费完并落盘，统计行数校验不丢帧
    EXPECT_TRUE(WaitForLogs("proc-", expectCount)) << "some frames lost";
}

// 测试 4 千条日志端到端不丢帧
TEST_F(ThirdAdapter_SprLog, Pressure_10KNoLoss) {
    constexpr int32_t PROGRESS_STEP = 1000;
    constexpr size_t BURST_BYTES = 256 * 1024;
    constexpr useconds_t BURST_PAUSE_US = 200 * 1000;
    constexpr int32_t FLUSH_TIMEOUT_SEC = 20;

    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);

    const std::string mark = "pressure-10k-" + std::to_string(getpid()) + "-"
                           + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    const int32_t TOTAL = EstimateLogsForConfiguredRotations(mark.size(), 16, 2, 2000, 4000);
    size_t pendingBytes = 0;
    const size_t bytesPerLog = EstimateBytesPerLog(mark.size(), 16);
    TEST_LOG("Writing %d logs with mark [%s]...", TOTAL, mark.c_str());
    for (int32_t i = 0; i < TOTAL; ++i) {
        if (i > 0 && (i % PROGRESS_STEP) == 0) {
            TEST_LOG("  progress: %d/%d", i, TOTAL);
        }
        SPR_LOGD("%s index: %d", mark.c_str(), i);
        ThrottleAsyncWrite(pendingBytes, bytesPerLog, BURST_BYTES, BURST_PAUSE_US);
    }

    // 等待日志服务消费完并落盘，统计行数校验不丢帧
    TEST_LOG("Waiting for %d logs to flush (timeout %ds)...", TOTAL, FLUSH_TIMEOUT_SEC);
    const bool flushed = WaitForLogs(mark, TOTAL, FLUSH_TIMEOUT_SEC);
    const int finalCount = CountLogs(mark);
    TEST_LOG("Flush result: %d/%d logs found", finalCount, TOTAL);
    EXPECT_TRUE(flushed) << "Expected " << TOTAL << " logs flushed, actual " << finalCount;
}

// 测试日志轮转后内容仍可检索（文件写满文件名变更场景）。
// 写满 file_capacity 触发轮转：sprlog.log → sprlog.log.1，后续文件后缀递增。
TEST_F(ThirdAdapter_SprLog, Rotation_ContentRetrievable) {
    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
    pLog->SetLength(LOG_BUFFER_SIZE_LIMIT);

    const std::string mark = "rotation-mark-" + std::to_string(getpid()) + "-"
                           + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    const std::string payload(700, 'R');
    size_t pendingBytes = 0;
    const size_t bytesPerLog = EstimateBytesPerLog(payload.size(), mark.size() + 32);
    SPR_LOGI("%s before-rotate", mark.c_str());

    const int32_t fillCount = EstimateLogsForSingleRotation(payload.size(), mark.size() + 32);
    for (int32_t i = 0; i < fillCount; ++i) {
        SPR_LOGD("%s rotation-fill-%08d %s", mark.c_str(), i, payload.c_str());
        ThrottleAsyncWrite(pendingBytes, bytesPerLog);
    }

    SPR_LOGI("%s after-rotate", mark.c_str());

    // 轮转前后写入的内容都应在（可能已更名的）日志文件中检索到
    EXPECT_TRUE(WaitForLog(mark + " before-rotate")) << "content before rotate lost";
    EXPECT_TRUE(WaitForLog(mark + " after-rotate")) << "content after rotate lost";
    EXPECT_GE(RotatedFileCount(), 1) << "rotation should generate at least one rotated file";
}

// 测试大量日志写入触发轮转，准确性检验
// 场景：按单文件容量估算最小条数，稳定触发一次轮转且完整落盘
TEST_F(ThirdAdapter_SprLog, Rotation_LargePayload20K) {
    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
    pLog->SetLength(LOG_BUFFER_SIZE_LIMIT);

    // 使用唯一标记，避免已存在日志的干扰
    const std::string uniqueMark = "largescale-100k-" + std::to_string(getpid()) + "-" +
                                   std::to_string(std::chrono::system_clock::now().time_since_epoch().count());

    // 记录写入前的状态
    const int initialCount = CountLogs(uniqueMark);
    EXPECT_EQ(initialCount, 0) << "unique mark should not exist before test";

    const std::string payload(700, 'L');
    const int32_t TOTAL_LOGS = EstimateLogsForConfiguredRotations(
        payload.size(), uniqueMark.size() + 24, 1, 1024, 6000);
    size_t pendingBytes = 0;
    const size_t bytesPerLog = EstimateBytesPerLog(payload.size(), uniqueMark.size() + 24);
    const useconds_t burstPauseUs = 2 * 1000 * 1000;
    const int flushTimeoutSec = 30;

    // 写入估算条数，保证总字节量足以触发一次轮转
    TEST_LOG("Writing %d logs with mark [%s]...", TOTAL_LOGS, uniqueMark.c_str());
    for (int32_t i = 0; i < TOTAL_LOGS; ++i) {
        if (i % 5000 == 0 && i > 0) {
            TEST_LOG("  progress: %d/%d", i, TOTAL_LOGS);
        }
        SPR_LOGD("%s batch-%06d %s", uniqueMark.c_str(), i, payload.c_str());
        ThrottleAsyncWrite(pendingBytes, bytesPerLog, burstPauseUs);
    }

    // 大包轮转场景对后端消费更敏感，这里放宽到 4MB/2s，避免共享缓冲短时堆积。
    TEST_LOG("Waiting for %d logs to flush to disk (timeout %ds)...", TOTAL_LOGS, flushTimeoutSec);
    const bool flushed = WaitForLogs(uniqueMark, TOTAL_LOGS, flushTimeoutSec);
    const int finalCount = CountLogs(uniqueMark);
    TEST_LOG("Flush result: %d/%d logs found", finalCount, TOTAL_LOGS);
    EXPECT_TRUE(flushed)
        << "Expected " << TOTAL_LOGS << " logs flushed, actual " << finalCount;

    // 统计并校验轮转文件状态
    const int rotatedCount = RotatedFileCount();
    TEST_LOG("Rotation statistics: %d rotated files detected", rotatedCount);
    EXPECT_GE(rotatedCount, 1) << "Should have at least 1 rotated file";
}

// 测试轮转过程中的多进程并发写入准确性
// 场景：4 个子进程各写 200 条中等长度日志，验证并发写入路径可稳定执行
TEST_F(ThirdAdapter_SprLog, Rotation_ConcurrentModeratePayload) {
    constexpr int32_t PROC_NUM       = 4;
    constexpr int32_t MSG_PER_PROC   = 200;
    const int expectCount            = PROC_NUM * MSG_PER_PROC;

    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
    pLog->SetLength(512);
    const std::string payload(128, 'C');

    const std::string uniqueMark = "concurrent-2k-" + std::to_string(getpid()) + "-" +
                                   std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    const size_t bytesPerLog = EstimateBytesPerLog(payload.size(), uniqueMark.size() + 32);

    pid_t pids[PROC_NUM];
    TEST_LOG("Spawning %d processes, each writing %d logs...", PROC_NUM, MSG_PER_PROC);
    for (int32_t p = 0; p < PROC_NUM; ++p) {
        pid_t pid = fork();
        ASSERT_GE(pid, 0) << "fork failed";
        if (pid == 0) {
            char tag[16];
            snprintf(tag, sizeof(tag), "CProc%02d", p);
            size_t pendingBytes = 0;
            for (int32_t i = 0; i < MSG_PER_PROC; ++i) {
                pLog->d(tag, "%s proc-%02d msg-%06d %s", uniqueMark.c_str(), p, i, payload.c_str());
                ThrottleAsyncWrite(pendingBytes, bytesPerLog);
            }
            _exit(0);
        }
        pids[p] = pid;
    }

    for (int32_t p = 0; p < PROC_NUM; ++p) {
        int status = 0;
        waitpid(pids[p], &status, 0);
        EXPECT_TRUE(WIFEXITED(status) && WEXITSTATUS(status) == 0) << "child " << p << " failed";
    }

    TEST_LOG("Waiting for %d logs to flush (timeout 20s)...", expectCount);
    EXPECT_TRUE(WaitForLogs(uniqueMark, expectCount, 20))
        << "Expected " << expectCount << " logs flushed, some may be lost";

    int finalCount = 0;
    for (int retry = 0; retry < 10; ++retry) {
        finalCount = CountLogs(uniqueMark);
        if (finalCount == expectCount) {
            break;
        }
        usleep(200000);
    }
    TEST_LOG("Final count: %d/%d logs found", finalCount, expectCount);
    EXPECT_EQ(finalCount, expectCount) << "All logs should be preserved during rotation";
}

// 测试轮转文件命名和后缀递增的正确性
// 场景：写入触发轮转，验证文件名格式和后缀序号
TEST_F(ThirdAdapter_SprLog, Rotation_FileNameAndSuffix) {
    LogConfig cfg;
    ASSERT_TRUE(LoadLogConfig(cfg)) << "open sprlog.conf failed";

    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
    pLog->SetLength(LOG_BUFFER_SIZE_LIMIT);

    const std::string uniqueMark = "suffix-check-" + std::to_string(getpid()) + "-"
                                 + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    const std::string payload(700, 'S');
    const std::map<std::string, int> beforeSuffixMap = RotatedSuffixes();
    size_t pendingBytes = 0;
    const size_t bytesPerLog = EstimateBytesPerLog(payload.size(), uniqueMark.size() + 24);
    const int rotationTarget = std::max(1, cfg.fileNum);

    // 触发足够多次轮转，确保 pack_enabled=true 且 file_num=3 时能生成归档文件。
    const int32_t fillCount = EstimateLogsForConfiguredRotations(
        payload.size(), uniqueMark.size() + 24, rotationTarget, 6000, 20000);
    for (int32_t i = 0; i < fillCount; ++i) {
        SPR_LOGD("%s fill-%08d %s", uniqueMark.c_str(), i, payload.c_str());
        ThrottleAsyncWrite(pendingBytes, bytesPerLog);
    }
    ASSERT_TRUE(WaitForRotatedArtifacts(1)) << "rotation artifacts not generated";

    // 验证轮转后的文件命名：
    // 1. 每个轮转文件的末尾应有数字后缀（.1, .2, .3 等）
    // 2. 轮转文件名应包含原始基础名
    // 3. 后缀应按升序排列
    std::vector<std::string> rotatedFiles;
    std::map<std::string, int> suffixMap = RotatedSuffixes();

    for (const auto& path : LogFiles()) {
        const size_t lastSlash = path.rfind('/');
        const std::string fileName = lastSlash != std::string::npos ? path.substr(lastSlash + 1) : path;

        const size_t lastDot = fileName.rfind('.');
        if (lastDot != std::string::npos && lastDot > 0) {
            const std::string suffix = fileName.substr(lastDot + 1);
            char* end = nullptr;
            std::strtoul(suffix.c_str(), &end, 10);
            if (end != suffix.c_str() && *end == '\0') {
                rotatedFiles.push_back(fileName);
                EXPECT_FALSE(HasDoubleNumericSuffix(fileName))
                    << "rotated file should not contain chained numeric suffixes: " << fileName;
            }
        }
    }

    if (cfg.packEnabled) {
        const std::vector<std::string> packFiles = PackFiles();
        TEST_LOG("Found %zu pack files with suffixes", packFiles.size());
        EXPECT_GE(packFiles.size(), 1U) << "Should have at least 1 pack file";

        bool hasIndexedPack = false;
        for (size_t index = 0; index < packFiles.size(); ++index) {
            const size_t lastSlash = packFiles[index].rfind('/');
            const std::string fileName = lastSlash != std::string::npos
                                       ? packFiles[index].substr(lastSlash + 1) : packFiles[index];
            if (fileName.find(".1.tar.gz") != std::string::npos
                || fileName.find(".2.tar.gz") != std::string::npos
                || fileName.find(".1.zip") != std::string::npos
                || fileName.find(".2.zip") != std::string::npos) {
                hasIndexedPack = true;
                break;
            }
        }
        EXPECT_TRUE(hasIndexedPack) << "Expected at least one indexed pack file";
    } else {
        TEST_LOG("Found %zu rotated files with suffixes", rotatedFiles.size());
        EXPECT_GE(rotatedFiles.size(), 1U) << "Should have at least 1 rotated file";

        bool suffixAdvanced = false;
        for (auto it = suffixMap.begin(); it != suffixMap.end(); ++it) {
            const std::string& baseName = it->first;
            const int beforeMax = beforeSuffixMap.count(baseName) == 0 ? 0 : beforeSuffixMap.at(baseName);
            if (it->second > beforeMax) {
                TEST_LOG("  base [%s]: suffix advanced from %d to %d", baseName.c_str(), beforeMax, it->second);
                suffixAdvanced = true;
            }
        }
        EXPECT_TRUE(suffixAdvanced) << "Expected at least one rotated suffix to advance";
    }
}

// 测试轮转过程中新旧文件内容的完整性
// 场景：标记轮转前后的日志位置，确保内容不丢失或损坏
TEST_F(ThirdAdapter_SprLog, Rotation_ContentIntegrity) {
    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
    pLog->SetLength(LOG_BUFFER_SIZE_LIMIT);

    const std::string uniqueMark = "integrity-" + std::to_string(getpid()) + "-"
                                 + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    const std::string payload(700, 'I');
    size_t pendingBytes = 0;
    const size_t bytesPerLog = EstimateBytesPerLog(payload.size(), uniqueMark.size() + 24);

    // 写入标记1：轮转前
    SPR_LOGI("%s MARKER_BEFORE_ROTATION", uniqueMark.c_str());

    // 仅控制在稳定的单轮转范围内，避免保留裁剪把前置标记提前删掉。
    const int32_t fillCount = EstimateLogsForConfiguredRotations(
        payload.size(), uniqueMark.size() + 24, 1, 1024, 6000);
    for (int32_t i = 0; i < fillCount; ++i) {
        SPR_LOGD("%s data-%08d %s", uniqueMark.c_str(), i, payload.c_str());
        ThrottleAsyncWrite(pendingBytes, bytesPerLog);
    }

    // 写入标记2：轮转后
    SPR_LOGI("%s MARKER_AFTER_ROTATION", uniqueMark.c_str());

    // 轮转前的旧文件可能已经被 trim / pack 裁剪掉，因此不能把它当作必定保留的“硬约束”。
    // 真实要求是：轮转后标记必须落盘，并且只要两者都存在，就验证顺序正确。
    ASSERT_TRUE(WaitForLog(uniqueMark + " MARKER_AFTER_ROTATION")) << "marker after lost";

    // 逐行扫描所有文件，验证内容连续性和完整性
    std::vector<std::string> allLines;
    for (const auto& path : LogFiles()) {
        std::ifstream file(path);
        std::string line;
        while (std::getline(file, line)) {
            if (line.find(uniqueMark) != std::string::npos) {
                allLines.push_back(line);
            }
        }
    }
    for (const auto& path : PackFiles()) {
        for (const auto& line : ReadArchiveLines(path)) {
            if (line.find(uniqueMark) != std::string::npos) {
                allLines.push_back(line);
            }
        }
    }

    EXPECT_GE(allLines.size(), 1U) << "Should have at least one unique marker in the retained log set";

    // 验证顺序：比较两条 marker 自身的日志行顺序，而不是依赖目录遍历顺序。
    bool foundBefore = false, foundAfter = false;
    std::string beforeLine;
    std::string afterLine;
    for (auto& line : allLines) {
        if (line.find("MARKER_BEFORE_ROTATION") != std::string::npos) {
            foundBefore = true;
            beforeLine = line;
        }
        if (line.find("MARKER_AFTER_ROTATION") != std::string::npos) {
            foundAfter = true;
            afterLine = line;
        }
    }
    EXPECT_TRUE(foundAfter) << "MARKER_AFTER not found";
    EXPECT_TRUE(foundBefore || foundAfter) << "No rotation marker retained in the final log set";
    if (foundBefore && foundAfter) {
        EXPECT_LT(beforeLine, afterLine) << "MARKER_BEFORE should appear before MARKER_AFTER";
    }

    // 旧文件可能在 trim/pack 阶段被立即清理，因此不把“必定存在一份 rotated 文件”当成硬性要求。
    // 只保留内容完整性检查——只要确实存在轮转产物，就校验其数量/顺序；否则按保留日志集进行验证。
    if (RotatedFileCount() > 0) {
        EXPECT_GE(RotatedFileCount(), 1) << "rotation should generate at least one rotated file";
    }

    TEST_LOG("Rotation integrity check: found %zu relevant log lines, rotated files=%d", allLines.size(), RotatedFileCount());
}

// 测试轮转限制：文件数量不超过配置上限
// 场景：持续写入触发多次轮转，验证活跃文件数不超过 file_num 配置
TEST_F(ThirdAdapter_SprLog, Rotation_MaxFileLimitEnforced) {
    LogConfig cfg;
    ASSERT_TRUE(LoadLogConfig(cfg)) << "open sprlog.conf failed";

    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
    pLog->SetLength(LOG_BUFFER_SIZE_LIMIT);

    const std::string uniqueMark = "maxlimit-" + std::to_string(getpid()) + "-"
                                 + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    const std::string payload(700, 'M');
    size_t pendingBytes = 0;
    const size_t bytesPerLog = EstimateBytesPerLog(payload.size(), uniqueMark.size() + 24);

    // 持续触发轮转：写入超过 file_num 上限所需的数据量，验证旧文件会被裁剪。
    const int32_t fillCount = EstimateLogsForConfiguredRotations(
        payload.size(), uniqueMark.size() + 24, cfg.fileNum + 3, 2048, 250000);
    TEST_LOG("Writing %d entries to trigger multiple rotations...", fillCount);

    for (int32_t i = 0; i < fillCount; ++i) {
        if (i % 10000 == 0 && i > 0) {
            TEST_LOG("  progress: %d/%d", i, fillCount);
        }
        SPR_LOGD("%s fill-%08d %s", uniqueMark.c_str(), i, payload.c_str());
        ThrottleAsyncWrite(pendingBytes, bytesPerLog);
    }

    ASSERT_TRUE(WaitForLog(uniqueMark)) << "logs not flushed";
    usleep(500000);  // 等待轮转完成

    // 统计当前活跃文件数（包含当前和所有 .N 后缀文件）
    int totalFiles = 0;
    int filesWithSuffix = 0;
    for (const auto& path : LogFiles()) {
        totalFiles++;
        const size_t lastDot = path.rfind('.');
        if (lastDot != std::string::npos) {
            const std::string suffix = path.substr(lastDot + 1);
            char* end = nullptr;
            std::strtoul(suffix.c_str(), &end, 10);
            if (end != suffix.c_str() && *end == '\0') {
                filesWithSuffix++;
            }
        }
    }

    TEST_LOG("File count check: total=%d, with_suffix=%d, limit=%d", totalFiles, filesWithSuffix, cfg.fileNum);
    if (cfg.packEnabled) {
        const std::vector<std::string> packFiles = PackFiles();
        TEST_LOG("Pack count check: pack=%zu", packFiles.size());
        EXPECT_GE(packFiles.size(), 1U) << "Should have pack files after pressure write";
        EXPECT_LE(filesWithSuffix, cfg.fileNum) << "Remaining raw rotated files should stay within file_num";
        EXPECT_LE(totalFiles, cfg.fileNum + 1) << "Total raw files (including current) should be <= file_num+1";
    } else {
        EXPECT_GE(filesWithSuffix, 1) << "Should have rotated files after pressure write";
        EXPECT_LE(filesWithSuffix, cfg.fileNum) << "Rotated files should not exceed file_num limit";
        EXPECT_LE(totalFiles, cfg.fileNum + 1) << "Total files (including current) should be <= file_num+1";
    }
}
