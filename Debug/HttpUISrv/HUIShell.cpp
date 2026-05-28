/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HUIShell.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Remote shell command execution implementation
 *  @date       : 2026/05/28
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <pwd.h>
#include <sstream>
#include <mutex>
#include "HUIShell.h"

namespace HUIShell {

static std::string g_currentDir = "";
static std::string g_hostname = "";
static std::string g_username = "";
static std::mutex g_session_mutex;

int InitSession()
{
    std::lock_guard<std::mutex> lock(g_session_mutex);

    // Get current directory
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        g_currentDir = "/";
    } else {
        g_currentDir = cwd;
    }

    // Get hostname
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        g_hostname = hostname;
    } else {
        g_hostname = "unknown";
    }

    // Get username
    struct passwd* pwd = getpwuid(getuid());
    if (pwd != nullptr) {
        g_username = pwd->pw_name;
    } else {
        g_username = "user";
    }

    return 0;
}

std::string GetCurrentDir()
{
    std::lock_guard<std::mutex> lock(g_session_mutex);
    return g_currentDir;
}

std::string GetPrompt()
{
    std::lock_guard<std::mutex> lock(g_session_mutex);
    return g_username + "@" + g_hostname + ":" + g_currentDir + "$ ";
}

int ExecuteCommand(const std::string& cmd, std::string& output, int timeoutMs)
{
    if (cmd.empty()) {
        output = "";
        return -1;
    }

    // Build full command with current directory context
    std::string fullCmd;
    {
        std::lock_guard<std::mutex> lock(g_session_mutex);
        fullCmd = "cd '" + g_currentDir + "' && " + cmd;
    }

    // Use popen for command execution
    FILE* fp = popen(fullCmd.c_str(), "r");
    if (fp == nullptr) {
        output = "";
        return -1;
    }

    output.clear();
    const int bufSize = 4096;
    char buf[bufSize];
    while (fgets(buf, bufSize, fp) != nullptr) {
        output += buf;
    }

    int status = pclose(fp);
    if (status == -1) {
        return -1;
    }

    // Check if command was cd - if so, update current directory
    std::string trimCmd = cmd;
    // Trim leading/trailing whitespace
    size_t start = trimCmd.find_first_not_of(" \t\n\r");
    size_t end;
    if (start != std::string::npos) {
        trimCmd = trimCmd.substr(start);
        end = trimCmd.find_last_not_of(" \t\n\r");
        trimCmd = trimCmd.substr(0, end + 1);
    }

    if (trimCmd.length() >= 2 && trimCmd.substr(0, 2) == "cd") {
        // Extract directory from cd command
        std::string dir = trimCmd.substr(2);
        // Trim whitespace
        start = dir.find_first_not_of(" \t");
        if (start != std::string::npos) {
            dir = dir.substr(start);
            end = dir.find_last_not_of(" \t\n\r");
            dir = dir.substr(0, end + 1);

            // Remove quotes if present
            if (dir.length() >= 2 &&
                ((dir.front() == '\'' && dir.back() == '\'') ||
                 (dir.front() == '"' && dir.back() == '"'))) {
                dir = dir.substr(1, dir.length() - 2);
            }

            // Try to change directory and verify
            if (!dir.empty()) {
                char resolvedPath[4096];
                char oldCwd[4096];
                if (getcwd(oldCwd, sizeof(oldCwd)) == nullptr) {
                    return 0;
                }

                if (chdir(dir.c_str()) == 0) {
                    if (getcwd(resolvedPath, sizeof(resolvedPath)) != nullptr) {
                        std::lock_guard<std::mutex> lock(g_session_mutex);
                        g_currentDir = resolvedPath;
                    }
                } else {
                    int rc = chdir(oldCwd);  // Restore old directory on failure
                    (void)rc;  // Suppress unused variable warning
                    output = "cd: " + dir + ": No such file or directory\n";
                }
            }
        }
    }

    return 0;
}

int GetDeviceProfile(std::string& profile)
{
    std::ostringstream oss;
    std::string hostname, uptime, kernel;

    // Get hostname
    ExecuteCommand("hostname -f", hostname);
    if (!hostname.empty() && hostname.back() == '\n') {
        hostname.pop_back();
    }

    // Get uptime
    ExecuteCommand("uptime -p", uptime);
    if (!uptime.empty() && uptime.back() == '\n') {
        uptime.pop_back();
    }

    // Get kernel version
    ExecuteCommand("uname -r", kernel);
    if (!kernel.empty() && kernel.back() == '\n') {
        kernel.pop_back();
    }

    oss << "{"
        << "\"hostname\":\"" << (hostname.empty() ? "-" : hostname) << "\","
        << "\"uptime\":\"" << (uptime.empty() ? "-" : uptime) << "\","
        << "\"kernel\":\"" << (kernel.empty() ? "-" : kernel) << "\""
        << "}";

    profile = oss.str();
    return 0;
}

int GetResourceUsage(std::string& resources)
{
    std::ostringstream oss;
    std::string meminfo, df;

    // Get memory info
    ExecuteCommand("free -h | grep Mem | awk '{print $2, $3, $4}'", meminfo);
    if (!meminfo.empty() && meminfo.back() == '\n') {
        meminfo.pop_back();
    }

    // Get disk usage
    ExecuteCommand("df -h / | awk 'NR==2 {print $2, $3, $4, $5}'", df);
    if (!df.empty() && df.back() == '\n') {
        df.pop_back();
    }

    oss << "{"
        << "\"memory\":\"" << (meminfo.empty() ? "-" : meminfo) << "\","
        << "\"disk\":\"" << (df.empty() ? "-" : df) << "\""
        << "}";

    resources = oss.str();
    return 0;
}

}  // namespace HUIShell
