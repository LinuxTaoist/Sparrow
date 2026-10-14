/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : TestInternalEnvironment.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Internal gtest environment.
 *  @date       : 2026/10/14
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include "CommonMacros.h"
#include "gtest/gtest.h"

namespace {

constexpr const char* kLogConfigFile = "sprlog.conf";
constexpr const char* kInitConfigFile = "init.conf";
constexpr const char* kServiceManagerName = "servicemanagersrv";
constexpr const char* kLogManagerName = "logmanagersrv";
constexpr const char* kRuntimeLogPath = "/tmp/sprlog_test_runtime.log";
constexpr const char* kBinderReadyPath = "/tmp/bindermanagersrv";
constexpr const char* kConfigReadyPath = "/tmp/configmanagersrv";
constexpr const char* kTestLogFileName = "test_internal.log";
constexpr const char* kTestPackFileName = "test_internal";

struct EnvironmentState {
    bool smWasRunning = false;
    bool hasRootEnv = false;
    std::string rootEnv;
    std::string root;
    std::string etc;
    std::string logCfg;
    std::string initCfg;
    std::string logBak;
    std::string initBak;
    std::string logDir;
    std::string logDirBase;
    std::string packDir;
};

EnvironmentState environmentState;

std::string ShellQuote(const std::string& text) {
    std::string out("'");
    for (char c : text) {
        out += (c == '\'') ? "'\\''" : std::string(1, c);
    }
    return out + "'";
}

std::string Trim(const std::string& text) {
    const size_t begin = text.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return std::string();
    }
    const size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(begin, end - begin + 1);
}

std::string ExePath() {
    char buf[512] = {0};
    const ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len <= 0) {
        return std::string();
    }
    buf[len] = '\0';
    return buf;
}

std::string DirPath(const std::string& path) {
    const size_t slash = path.rfind('/');
    return slash == std::string::npos ? std::string() : path.substr(0, slash);
}

std::string ExeDirPath() {
    return DirPath(ExePath());
}

std::string RootPath() {
    const char* env = std::getenv(ENV_SPR_ROOT_PATH);
    if (env != nullptr && env[0] != '\0') {
        return env;
    }

    return DirPath(ExeDirPath());
}

bool RunCommand(const std::string& cmd) {
    return std::system(cmd.c_str()) == 0;
}

bool IsProcessRunning(const std::string& processName) {
    const std::string cmd = "pgrep -f " + ShellQuote("(^|/)" + processName + "($| )");
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe == nullptr) {
        return false;
    }
    char buf[32] = {0};
    const bool ok = fgets(buf, sizeof(buf), pipe) != nullptr;
    pclose(pipe);
    return ok;
}

bool WaitForProcessState(const std::string& processName, bool running, int timeoutSec) {
    const auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(timeoutSec)) {
        if (IsProcessRunning(processName) == running) {
            return true;
        }
        usleep(200000);
    }
    return false;
}

bool WaitForProcessStates(const std::vector<std::string>& names, bool running, int timeoutSec) {
    const auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(timeoutSec)) {
        bool matched = true;
        for (const auto& name : names) {
            if (IsProcessRunning(name) != running) {
                matched = false;
                break;
            }
        }
        if (matched) {
            return true;
        }
        usleep(200000);
    }
    return false;
}

bool WaitForProcessesExit(const std::vector<std::string>& names, int timeoutSec) {
    return WaitForProcessStates(names, false, timeoutSec);
}

bool WaitForFile(const std::string& path, int timeoutSec) {
    const auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(timeoutSec)) {
        if (access(path.c_str(), F_OK) == 0) {
            return true;
        }
        usleep(100000);
    }
    return false;
}

std::vector<std::string> ReadInitServiceNames(const std::string& configPath) {
    std::vector<std::string> names;
    std::ifstream file(configPath);
    std::string line;
    while (std::getline(file, line)) {
        line = Trim(line.substr(0, line.find('#')));
        if (!line.empty()) {
            names.push_back(Trim(line.substr(0, line.find_first_of(" \t"))));
        }
    }
    return names;
}

bool BackupFile(const std::string& sourcePath, const std::string& backupPath) {
    if (!std::ifstream(sourcePath).good()) {
        return false;
    }
    std::remove(backupPath.c_str());
    return std::rename(sourcePath.c_str(), backupPath.c_str()) == 0;
}

bool WriteTextFile(const std::string& path, const std::string& text) {
    std::ofstream file(path, std::ios::trunc);
    return file && (file << text, file.good());
}

bool CopyTextFile(const std::string& sourcePath, const std::string& targetPath) {
    std::ifstream in(sourcePath, std::ios::binary);
    std::ofstream out(targetPath, std::ios::binary | std::ios::trunc);
    return in && out && (out << in.rdbuf(), out.good());
}

bool WriteSprLogConfig() {
    const std::string logText =
        "[output.default]\n"
        "frame_length_bytes = 1024\n"
        "enabled = true\n"
        "output = file\n"
        "level = debug\n"
        "log_dir = " + environmentState.logDir + "\n"
        "file_name = " + std::string(kTestLogFileName) + "\n"
        "file_name_format = BN_ST.FX\n"
        "file_num = 3\n"
        "file_capacity_mb = 1\n"
        "flush_count = 32\n"
        "flush_interval_ms = 200\n"
        "pack_enabled = true\n"
        "pack_name = " + std::string(kTestPackFileName) + "\n"
        "pack_name_format = BN_ST.FX\n"
        "pack_dir = " + environmentState.packDir + "\n"
        "pack_type = tar.gz\n"
        "pack_file_num = 10\n"
        "pack_total_size_mb = 100\n";
    return WriteTextFile(environmentState.logCfg, logText);
}

bool StartServiceManager() {
    if (environmentState.root.empty()) {
        return false;
    }
    return RunCommand("cd " + ShellQuote(ExeDirPath())
        + " && ENV_SPR_ROOT_PATH=" + ShellQuote(environmentState.root)
        + " ./servicemanagersrv >" + ShellQuote(kRuntimeLogPath) + " 2>&1 &");
}

bool ResetSprLogTestEnvironmentImpl() {
    if (environmentState.root.empty() || environmentState.logCfg.empty() || environmentState.initCfg.empty()) {
        return false;
    }

    environmentState.logDir = environmentState.logDirBase;
    environmentState.packDir = environmentState.logDir + "/pack";

    RunCommand("killall -10 servicemanagersrv >/dev/null 2>&1 || true");
    if (!WaitForProcessesExit(ReadInitServiceNames(environmentState.initCfg), 20)) {
        return false;
    }

    if (!RunCommand("rm -rf " + ShellQuote(environmentState.logDir)
        + " >/dev/null 2>&1 || true")
        || !RunCommand("mkdir -p " + ShellQuote(environmentState.logDir))
        || !RunCommand("mkdir -p " + ShellQuote(environmentState.packDir))) {
        return false;
    }

    if (!WriteSprLogConfig()) {
        return false;
    }

    const std::vector<std::string> serviceNames = ReadInitServiceNames(environmentState.initCfg);
    return StartServiceManager()
        && WaitForProcessState(kServiceManagerName, true, 20)
        && WaitForProcessState(kLogManagerName, true, 20)
        && WaitForProcessStates(serviceNames, true, 20)
        && WaitForFile(kBinderReadyPath, 5)
        && WaitForFile(kConfigReadyPath, 5)
        && (usleep(500000), true);
}

bool PrepareSprLogTestEnvironment() {
    const char* env = std::getenv(ENV_SPR_ROOT_PATH);
    if (env != nullptr && env[0] != '\0') {
        environmentState.hasRootEnv = true;
        environmentState.rootEnv = env;
    }

    environmentState.root = RootPath();
    if (environmentState.root.empty()) {
        return false;
    }
    environmentState.etc = ExeDirPath() + "/../Etc";
    environmentState.logCfg = environmentState.etc + "/" + kLogConfigFile;
    environmentState.initCfg = environmentState.etc + "/" + kInitConfigFile;
    environmentState.logBak = environmentState.logCfg + ".bak.test_internal." + std::to_string(getpid());
    environmentState.initBak = environmentState.initCfg + ".bak.test_internal." + std::to_string(getpid());
    environmentState.logDirBase = "/tmp/sprlog/test_internal";
    environmentState.logDir = environmentState.logDirBase;
    environmentState.packDir = environmentState.logDir + "/pack";

    if (!RunCommand("mkdir -p " + ShellQuote(environmentState.etc))) {
        return false;
    }

    if (std::ifstream(environmentState.logCfg).good()
        && !BackupFile(environmentState.logCfg, environmentState.logBak)) {
        return false;
    }
    if (std::ifstream(environmentState.initCfg).good()
        && !BackupFile(environmentState.initCfg, environmentState.initBak)) {
        return false;
    }

    if (!RunCommand("rm -rf " + ShellQuote(environmentState.logDir)
        + " >/dev/null 2>&1 || true")
        || !RunCommand("mkdir -p " + ShellQuote(environmentState.logDir))
        || !RunCommand("mkdir -p " + ShellQuote(environmentState.packDir))) {
        return false;
    }

    if (!WriteSprLogConfig()) {
        return false;
    }

    if (!CopyTextFile(environmentState.initBak, environmentState.initCfg)) {
        return false;
    }

    setenv(ENV_SPR_ROOT_PATH, environmentState.root.c_str(), 1);

    environmentState.smWasRunning = IsProcessRunning(kServiceManagerName);
    if (environmentState.smWasRunning || IsProcessRunning(kServiceManagerName)) {
        RunCommand("killall -10 servicemanagersrv >/dev/null 2>&1 || true");
        if (!WaitForProcessesExit(ReadInitServiceNames(environmentState.initCfg), 20)) {
            return false;
        }
    }

    const std::vector<std::string> serviceNames = ReadInitServiceNames(environmentState.initCfg);
    return StartServiceManager()
        && WaitForProcessState(kServiceManagerName, true, 20)
        && WaitForProcessState(kLogManagerName, true, 20)
        && WaitForProcessStates(serviceNames, true, 20)
        && WaitForFile(kBinderReadyPath, 5)
        && WaitForFile(kConfigReadyPath, 5)
        && (usleep(500000), true);
}

bool RestoreSprLogTestEnvironment() {
    RunCommand("killall -10 servicemanagersrv >/dev/null 2>&1 || true");
    WaitForProcessesExit(ReadInitServiceNames(environmentState.initCfg), 20);

    if (!environmentState.logBak.empty() && std::ifstream(environmentState.logBak).good()) {
        std::remove(environmentState.logCfg.c_str());
        std::rename(environmentState.logBak.c_str(), environmentState.logCfg.c_str());
    } else {
        std::remove(environmentState.logCfg.c_str());
    }

    if (!environmentState.initBak.empty() && std::ifstream(environmentState.initBak).good()) {
        std::remove(environmentState.initCfg.c_str());
        std::rename(environmentState.initBak.c_str(), environmentState.initCfg.c_str());
    } else {
        std::remove(environmentState.initCfg.c_str());
    }

    std::remove(kRuntimeLogPath);

    if (environmentState.hasRootEnv) {
        setenv(ENV_SPR_ROOT_PATH, environmentState.rootEnv.c_str(), 1);
    } else {
        unsetenv(ENV_SPR_ROOT_PATH);
    }

    if (environmentState.smWasRunning) {
        return StartServiceManager()
            && WaitForProcessState(kServiceManagerName, true, 20)
            && WaitForProcessState(kLogManagerName, true, 20);
    }
    return true;
}

class TestInternalEnvironment : public ::testing::Environment {
public:
    void SetUp() override { ASSERT_TRUE(PrepareSprLogTestEnvironment()); }
    void TearDown() override { ASSERT_TRUE(RestoreSprLogTestEnvironment()); }
};

::testing::Environment* const gTestInternalEnvironment =
    ::testing::AddGlobalTestEnvironment(new TestInternalEnvironment());

} // namespace

bool ResetSprLogTestEnvironment() {
    return ResetSprLogTestEnvironmentImpl();
}
