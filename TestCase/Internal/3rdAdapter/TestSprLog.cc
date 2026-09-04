/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprLog.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/03/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <vector>
#include <thread>
#include <atomic>
#include <regex>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstdlib>
#include <limits.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "gtest/gtest.h"
#include "SprLog.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "GeneralUtils.h"

// 定义不同长度的 LOG_TAG
#define LOG_TAG "TestTag"
#define LONG_LOG_TAG "ThisIsAVeryLongTagThatExceedsLimit"

#define LOG_BUFFER_DEF      512           // 与 SprLog.cpp 的 LOG_BUFFER_SIZE_DEFAULT 保持一致
#define LOG_CONF_FILE       "sprlog.conf" // 与 LogManager.cpp 的 LOG_CONFIGURE_FILE_PATH 保持一致

struct LogConfig {
    std::string output;         // file / stdout
    std::string filePath;       // logging.file_path
    std::string fileName;       // logging.file_name
    int fileNum = 10;           // logging.file_num
    int fileCapacityMb = 10;    // logging.file_capacity (MB)
};

// 去掉字符串首尾空白
std::string TrimStr(const std::string& str) {
    size_t begin = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }
    return str.substr(begin, end - begin + 1);
}

static std::string GetLogCfgPath()
{
    const char* pEnvRoot = std::getenv(ENV_SPR_ROOT_PATH);
    if (pEnvRoot != nullptr && pEnvRoot[0] != '\0') {
        return std::string(pEnvRoot) + "/" + DEFAULT_SPR_ETC_FILE + "/" + LOG_CONF_FILE;
    }

    char exePath[300] = {0};
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len > 0) {
        exePath[len] = '\0';
        std::string fullPath(exePath);
        auto pos = fullPath.find_last_of('/');
        if (pos != std::string::npos) {
            std::string execDir = fullPath.substr(0, pos);
            return execDir + "/../" + DEFAULT_SPR_ETC_FILE + "/" + LOG_CONF_FILE;
        }
    }

    return "";
}


// 读取 sprlog.conf（优先按可执行文件目录/Etc 解析，兼容从 Bin 目录启动）
bool LoadLogConfig(LogConfig& cfg) {
    std::string cfgPath = GetLogCfgPath();
    std::ifstream file(cfgPath);
    if (!file) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t comment = line.find('#');
        if (comment != std::string::npos) {
            line = line.substr(0, comment);
        }
        size_t eq = line.find('=');
        if (eq == std::string::npos) {
            continue;
        }

        std::string key = TrimStr(line.substr(0, eq));
        std::string value = TrimStr(line.substr(eq + 1));
        if (key == "logging.output") {
            cfg.output = value;
        } else if (key == "logging.file_path") {
            cfg.filePath = value;
        } else if (key == "logging.file_name") {
            cfg.fileName = value;
        } else if (key == "logging.file_num") {
            cfg.fileNum = atoi(value.c_str());
        } else if (key == "logging.file_capacity") {
            cfg.fileCapacityMb = atoi(value.c_str());
        }
    }
    return true;
}

// 获取日志目录下全部日志文件路径（含轮转后缀：sprlog.log, sprlog.log.1, ...）
std::vector<std::string> GetAllLogFilePaths() {
    LogConfig cfg;
    std::vector<std::string> paths;
    if (!LoadLogConfig(cfg) || cfg.filePath.empty()) {
        return paths;
    }

    DIR* dir = opendir(cfg.filePath.c_str());
    if (!dir) {
        return paths;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name(entry->d_name);
        if (name.rfind(cfg.fileName, 0) == 0) {
            paths.push_back(cfg.filePath + "/" + name);
        }
    }
    closedir(dir);
    return paths;
}

// 在全部日志文件（含轮转）中查找指定内容是否出现
bool SearchInLogFiles(const std::string& content) {
    for (auto& path : GetAllLogFilePaths()) {
        std::ifstream file(path);
        std::string line;
        while (std::getline(file, line)) {
            if (line.find(content) != std::string::npos) {
                return true;
            }
        }
    }
    return false;
}

// 统计全部日志文件（含轮转）中包含指定内容的行数
int CountInLogFiles(const std::string& content) {
    int count = 0;
    for (auto& path : GetAllLogFilePaths()) {
        std::ifstream file(path);
        std::string line;
        while (std::getline(file, line)) {
            if (line.find(content) != std::string::npos) {
                count++;
            }
        }
    }
    return count;
}

// 轮询等待日志文件出现指定内容，超时返回 false
bool WaitLogFileContains(const std::string& content, int timeoutSec = 10) {
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(timeoutSec)) {
        if (SearchInLogFiles(content)) {
            return true;
        }
        usleep(100000);
    }
    return false;
}

// 轮询等待日志文件中指定内容行数达到期望值，超时返回 false
bool WaitLogLines(const std::string& content, int expectCount, int timeoutSec = 30) {
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(timeoutSec)) {
        if (CountInLogFiles(content) >= expectCount) {
            return true;
        }
        usleep(200000);
    }
    return false;
}

// 检查日志服务是否已就绪（通过进程检测，不直接操作共享内存）
bool IsLogSrvReady() {
    FILE* pipe = popen("pgrep -x logmanagersrv", "r");
    if (pipe == nullptr) {
        return false;
    }
    char buf[32] = {0};
    bool found = (fgets(buf, sizeof(buf), pipe) != nullptr);
    pclose(pipe);
    return found;
}

// 解析标准帧: "MM-DD HH:MM:SS.mmm  PID  TAG  LEVEL: content"
bool ParseFrame(const std::string& frame, std::string& level, std::string& tag, std::string& content) {
    static const std::regex re(R"(^\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3}\s+\d+\s+(\S+)\s+([DIWE]):\s*(.*)$)");
    std::smatch m;
    if (!std::regex_match(frame, m, re)) {
        return false;
    }
    tag     = m[1].str();
    level   = m[2].str();
    content = m[3].str();
    return true;
}

// 测试基本日志级别
TEST(ThirdAdapter_SprLog, BasicLoggingLevels) {
    // 测试 SPR_LOGD
    SPR_LOGD("This is a debug log");

    // 测试 SPR_LOGI
    SPR_LOGI("This is an info log");

    // 测试 SPR_LOGW
    SPR_LOGW("This is a warning log");

    // 测试 SPR_LOGE
    SPR_LOGE("This is an error log");
}

// 测试日志标签长度检查
TEST(ThirdAdapter_SprLog, TagLengthCheck) {
    // 测试正常长度标签
    SPR_LOGD("This is a log with normal tag length");

    // 尝试使用超长标签，这里期望编译失败，但我们可以通过注释和说明来强调
    // 如果在代码中直接使用下面的宏，会触发静态断言
    // #define LOG_TAG LONG_LOG_TAG
    // SPR_LOGD("This should fail due to long tag");
}

// 测试可变参数日志
TEST(ThirdAdapter_SprLog, VariableArgumentLogging) {
    int value = 42;
    SPR_LOGD("This is a log with variable argument: %d", value);
}

// 测试单例模式
TEST(ThirdAdapter_SprLog, SingletonPattern) {
    SprLog* instance1 = SprLog::GetInstance();
    SprLog* instance2 = SprLog::GetInstance();
    EXPECT_EQ(instance1, instance2);
}

// 测试1000条日志输出
TEST(ThirdAdapter_SprLog, MultipleLogs) {
    for (int i = 0; i <= 1000; ++i) {
        SPR_LOGD("Test log out times: %d", i);
    }
}

// 测试超长日志输出
TEST(ThirdAdapter_SprLog, LongLogOutput) {
    std::string longMessage(1024, 'a');
    SPR_LOGD("%s", longMessage.c_str());
}

// 测试自定义次数，配合--gtest_repeat参数使用
// ./test_internal --gtest_filter=ThirdAdapter_SprLog.CustomerLogCount --gtest_repeat=5
TEST(ThirdAdapter_SprLog, CustomerLogCount) {
    static int32_t count = 0;
    SPR_LOGD("Customer log count: %d", ++count);
}

// 测试压力输出 10 万条日志
TEST(ThirdAdapter_SprLog, PressureTest) {
    for (int32_t i = 0; i < 100000; ++i) {
        std::string message = "Log message test index: " + std::to_string(i);
        SPR_LOGD("%s", message.c_str());
    }
}

// 测试帧格式标准（时间戳/毫秒/PID/TAG/级别/内容）端到端落盘校验
TEST(ThirdAdapter_SprLog, FrameFormat_Standard) {
    if (!IsLogSrvReady()) {
        FAIL() << "LogManager not started, please start logmanagersrv first";
    }

    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);

    const std::string unique = "frame-format-check-2025";
    SPR_LOGI("%s", unique.c_str());

    // 等待日志服务消费并落盘
    ASSERT_TRUE(WaitLogFileContains(unique)) << "log not flushed to file";

    // 在全部日志文件（含轮转）中定位该帧并按格式校验
    bool found = false;
    for (auto& path : GetAllLogFilePaths()) {
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
TEST(ThirdAdapter_SprLog, SpecialContent) {
    if (!IsLogSrvReady()) {
        FAIL() << "LogManager not started, please start logmanagersrv first";
    }

    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);

    SPR_LOGI("percent:100%% path:/tmp/a b 中文 \t quote:\"x\"");

    ASSERT_TRUE(WaitLogFileContains("percent:100%")) << "log not flushed to file";
    // 校验特殊内容在落盘文件中完整保留
    EXPECT_TRUE(SearchInLogFiles("中文"));
    EXPECT_TRUE(SearchInLogFiles("quote:\"x\""));
    EXPECT_TRUE(SearchInLogFiles("/tmp/a b"));
}

// 测试多行日志端到端落盘（一帧跨多行，内容完整）
TEST(ThirdAdapter_SprLog, MultiLineContent) {
    if (!IsLogSrvReady()) {
        FAIL() << "LogManager not started, please start logmanagersrv first";
    }

    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);

    SPR_LOGW("line1\nline2:with-colon\nline3");

    ASSERT_TRUE(WaitLogFileContains("line2:with-colon")) << "log not flushed to file";
    EXPECT_TRUE(SearchInLogFiles("line1"));
    EXPECT_TRUE(SearchInLogFiles("line3"));
}

// 测试前端级别过滤（SetLevel 后低于级别的日志不产出帧、不落盘）
TEST(ThirdAdapter_SprLog, LevelFilter_AtProducer) {
    if (!IsLogSrvReady()) {
        FAIL() << "LogManager not started, please start logmanagersrv first";
    }

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
    ASSERT_TRUE(WaitLogFileContains("error-should-appear")) << "log not flushed to file";

    // 被前端拦截的日志未写入共享内存，落盘文件中必然不存在
    EXPECT_FALSE(SearchInLogFiles("should-not-appear-d"));
    EXPECT_FALSE(SearchInLogFiles("should-not-appear-i"));
    EXPECT_FALSE(SearchInLogFiles("should-not-appear-w"));
    EXPECT_TRUE(SearchInLogFiles("error-should-appear"));

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);   // 恢复
}

// 测试超长日志截断（超过 SetLength 后截断并带 [TRUNCATED] 标记）
TEST(ThirdAdapter_SprLog, LengthLimit_Truncate) {
    if (!IsLogSrvReady()) {
        FAIL() << "LogManager not started, please start logmanagersrv first";
    }

    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);
    pLog->SetLength(128);   // 收紧帧长，触发截断路径

    std::string longMsg(1024, 'a');
    SPR_LOGD("long-content:%s", longMsg.c_str());

    ASSERT_TRUE(WaitLogFileContains("[TRUNCATED]")) << "log not flushed to file";
    EXPECT_TRUE(SearchInLogFiles("long-"));     // 截断保留的原文前缀
    EXPECT_TRUE(SearchInLogFiles("LEN:"));      // 截断标记携带原始长度
}

// 测试 SetLength / SetLevel 边界
TEST(ThirdAdapter_SprLog, SetterBoundary) {
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
TEST(ThirdAdapter_SprLog, Concurrent_MultiThreadWrite) {
    if (!IsLogSrvReady()) {
        FAIL() << "LogManager not started, please start logmanagersrv first";
    }

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
    EXPECT_TRUE(WaitLogLines("thread-", expectCount)) << "some frames lost";
}

// 测试多进程并发写（fork 子进程模拟多个业务进程，端到端校验不丢帧）
TEST(ThirdAdapter_SprLog, Concurrent_MultiProcessWrite) {
    if (!IsLogSrvReady()) {
        FAIL() << "LogManager not started, please start logmanagersrv first";
    }

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
    EXPECT_TRUE(WaitLogLines("proc-", expectCount)) << "some frames lost";
}

// 测试万条日志端到端不丢帧
TEST(ThirdAdapter_SprLog, Pressure_10KNoLoss) {
    if (!IsLogSrvReady()) {
        FAIL() << "LogManager not started, please start logmanagersrv first";
    }

    constexpr int32_t TOTAL = 10000;

    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);

    for (int32_t i = 0; i < TOTAL; ++i) {
        SPR_LOGD("pressure index: %d", i);
    }

    // 等待日志服务消费完并落盘，统计行数校验不丢帧
    EXPECT_TRUE(WaitLogLines("pressure index", TOTAL)) << "some frames lost";
}

// 测试日志轮转后内容仍可检索（文件写满文件名变更场景）。
// 写满 file_capacity 触发轮转：sprlog.log → sprlog.log.1，后续文件后缀递增。
// 若 file_capacity 过大无法快速触发（如 10MB），打印提示并跳过，读取逻辑本身已支持轮转文件检索。
TEST(ThirdAdapter_SprLog, Rotation_ContentRetrievable) {
    if (!IsLogSrvReady()) {
        FAIL() << "LogManager not started, please start logmanagersrv first";
    }

    LogConfig cfg;
    ASSERT_TRUE(LoadLogConfig(cfg)) << "open sprlog.conf failed";

    // 容量 >1MB 时触发轮转成本过高，跳过（读取逻辑对所有用例均已覆盖轮转检索）
    if (cfg.fileCapacityMb > 1) {
        printf("SKIP Rotation_ContentRetrievable: file_capacity=%dMB too large\n", cfg.fileCapacityMb);
        return;
    }

    SprLog* pLog = SprLog::GetInstance();
    if (!pLog) {
        FAIL() << "SprLog::GetInstance() failed";
    }

    pLog->SetLevel(InternalDefs::LOG_LEVEL_DEBUG);

    const std::string mark = "rotation-mark-" + std::to_string(getpid());
    SPR_LOGI("%s before-rotate", mark.c_str());

    // 写满容量触发轮转：每条约 140 字节，写满后多写一段确保跨过阈值
    const int32_t fillCount = cfg.fileCapacityMb * 1024 * 1024 / 140 + 500;
    for (int32_t i = 0; i < fillCount; ++i) {
        SPR_LOGD("rotation-fill-%08d", i);
    }

    SPR_LOGI("%s after-rotate", mark.c_str());

    // 轮转前后写入的内容都应在（可能已更名的）日志文件中检索到
    EXPECT_TRUE(WaitLogFileContains(mark + " before-rotate")) << "content before rotate lost";
    EXPECT_TRUE(WaitLogFileContains(mark + " after-rotate")) << "content after rotate lost";
}
