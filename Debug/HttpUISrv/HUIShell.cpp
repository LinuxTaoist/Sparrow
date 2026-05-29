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
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <pwd.h>
#include <stdint.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <algorithm>
#include <sstream>
#include <mutex>
#include "HUIShell.h"

namespace HUIShell {

namespace {

const char* PROMPT_MARKER = "__HUI_PROMPT__";
const int READ_SLICE_MS = 100;
const int SESSION_BOOT_MS = 3000;
const int INTERRUPT_WAIT_MS = 1500;

static std::string gCurrentDir = "";
static std::string gHostname = "";
static std::string gUsername = "";
static std::string gPrompt = "";
static std::string gRawBuffer = "";
static int gMasterFd = -1;
static pid_t gShellPid = -1;
static bool gWaitingForPrompt = false;
static std::mutex gSessionMutex;

void UpdatePromptCacheLocked(const std::string& prompt)
{
    gPrompt = prompt;

    size_t atPos = prompt.find('@');
    size_t colonPos = prompt.find(':', atPos == std::string::npos ? 0 : atPos);
    size_t signPos = prompt.find_last_of("#$");

    if (atPos != std::string::npos && colonPos != std::string::npos) {
        gUsername = prompt.substr(0, atPos);
        gHostname = prompt.substr(atPos + 1, colonPos - atPos - 1);
    }

    if (colonPos != std::string::npos && signPos != std::string::npos && signPos > colonPos) {
        gCurrentDir = prompt.substr(colonPos + 1, signPos - colonPos - 1);
    }
}

void ResetSessionLocked(bool killChild)
{
    if (gMasterFd >= 0) {
        close(gMasterFd);
        gMasterFd = -1;
    }

    if (gShellPid > 0) {
        if (killChild) {
            kill(gShellPid, SIGKILL);
        }
        waitpid(gShellPid, nullptr, WNOHANG);
        gShellPid = -1;
    }

    gPrompt.clear();
    gRawBuffer.clear();
    gWaitingForPrompt = false;
}

bool IsShellAliveLocked()
{
    if (gShellPid <= 0) {
        return false;
    }

    int status = 0;
    pid_t ret = waitpid(gShellPid, &status, WNOHANG);
    if (ret == 0) {
        return true;
    }

    ResetSessionLocked(false);
    return false;
}

bool WriteAllLocked(const std::string& data)
{
    const char* ptr = data.c_str();
    size_t remain = data.length();
    while (remain > 0) {
        ssize_t writeLen = write(gMasterFd, ptr, remain);
        if (writeLen > 0) {
            ptr += writeLen;
            remain -= static_cast<size_t>(writeLen);
            continue;
        }

        if (writeLen < 0 && errno == EINTR) {
            continue;
        }

        return false;
    }

    return true;
}

bool SelectReadableLocked(int waitMs)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(gMasterFd, &readfds);

    struct timeval tv;
    tv.tv_sec = waitMs / 1000;
    tv.tv_usec = (waitMs % 1000) * 1000;
    int ret = select(gMasterFd + 1, &readfds, nullptr, nullptr, &tv);
    if (ret < 0) {
        return errno == EINTR;
    }

    return ret > 0 && FD_ISSET(gMasterFd, &readfds);
}

bool DrainReadyOutputLocked()
{
    char buf[4096];
    while (true) {
        ssize_t readLen = read(gMasterFd, buf, sizeof(buf));
        if (readLen > 0) {
            gRawBuffer.append(buf, static_cast<size_t>(readLen));
            continue;
        }

        if (readLen == 0) {
            return false;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return true;
        }

        if (errno == EINTR) {
            continue;
        }

        return false;
    }
}

bool IsPartialPromptMarker(const std::string& text, size_t index)
{
    size_t remain = text.length() - index;
    size_t markerLen = strlen(PROMPT_MARKER);
    if (remain >= markerLen) {
        return false;
    }

    return text.compare(index, remain, PROMPT_MARKER, remain) == 0;
}

bool TryConsumePrompt(const std::string& text, size_t index, std::string& prompt, size_t& consumed)
{
    size_t markerLen = strlen(PROMPT_MARKER);
    if (text.length() - index < markerLen) {
        return false;
    }

    if (text.compare(index, markerLen, PROMPT_MARKER) != 0) {
        return false;
    }

    size_t pos = index + markerLen;
    while (pos + 1 < text.length()) {
        if ((text[pos] == '$' || text[pos] == '#') && text[pos + 1] == ' ') {
            prompt = text.substr(index + markerLen, pos + 2 - (index + markerLen));
            consumed = pos + 2 - index;
            return true;
        }
        ++pos;
    }

    return false;
}

bool TrySkipEscapeSequence(const std::string& text, size_t index, size_t& consumed)
{
    if (text[index] != 0x1b) {
        return false;
    }

    if (index + 1 >= text.length()) {
        return false;
    }

    if (text[index + 1] == '[') {
        size_t pos = index + 2;
        while (pos < text.length()) {
            unsigned char ch = static_cast<unsigned char>(text[pos]);
            if (ch >= '@' && ch <= '~') {
                consumed = pos + 1 - index;
                return true;
            }
            ++pos;
        }
        return false;
    }

    if (text[index + 1] == ']') {
        size_t pos = index + 2;
        while (pos < text.length()) {
            if (text[pos] == '\a') {
                consumed = pos + 1 - index;
                return true;
            }
            if (text[pos] == 0x1b && pos + 1 < text.length() && text[pos + 1] == '\\') {
                consumed = pos + 2 - index;
                return true;
            }
            ++pos;
        }
        return false;
    }

    consumed = 2;
    return true;
}

void ConsumeProcessedOutputLocked(std::string& output, bool& promptReady, std::string& promptAfter)
{
    output.clear();
    promptReady = false;
    promptAfter.clear();

    size_t i = 0;
    while (i < gRawBuffer.length()) {
        if (IsPartialPromptMarker(gRawBuffer, i)) {
            break;
        }

        std::string prompt;
        size_t consumed = 0;
        if (TryConsumePrompt(gRawBuffer, i, prompt, consumed)) {
            UpdatePromptCacheLocked(prompt);
            promptAfter = prompt;
            promptReady = true;
            gWaitingForPrompt = false;
            i += consumed;
            continue;
        }

        unsigned char ch = static_cast<unsigned char>(gRawBuffer[i]);
        if (ch == 0x1b) {
            if (!TrySkipEscapeSequence(gRawBuffer, i, consumed)) {
                break;
            }
            i += consumed;
            continue;
        }

        if (ch == '\r') {
            ++i;
            continue;
        }

        if (ch < 0x20 && ch != '\n' && ch != '\t') {
            ++i;
            continue;
        }

        output.push_back(static_cast<char>(ch));
        ++i;
    }

    gRawBuffer.erase(0, i);
}

bool ReadOutputChunkLocked(std::string& output, std::string& promptAfter, bool& promptReady, int waitMs)
{
    if (!gRawBuffer.empty()) {
        ConsumeProcessedOutputLocked(output, promptReady, promptAfter);
        if (!output.empty() || promptReady) {
            return true;
        }
    }

    if (waitMs > 0 && !SelectReadableLocked(waitMs)) {
        output.clear();
        promptAfter.clear();
        promptReady = false;
        return true;
    }

    if (!DrainReadyOutputLocked()) {
        return false;
    }

    ConsumeProcessedOutputLocked(output, promptReady, promptAfter);
    return true;
}

bool WaitForPromptLocked(std::string& output, std::string& promptAfter, int timeoutMs)
{
    output.clear();
    promptAfter.clear();
    int elapsedMs = 0;
    while (timeoutMs <= 0 || elapsedMs < timeoutMs) {
        std::string chunk;
        bool promptReady = false;
        std::string prompt;
        int waitMs = READ_SLICE_MS;
        if (timeoutMs > 0 && timeoutMs - elapsedMs < waitMs) {
            waitMs = timeoutMs - elapsedMs;
        }

        if (!ReadOutputChunkLocked(chunk, prompt, promptReady, waitMs)) {
            return false;
        }

        output += chunk;
        if (promptReady) {
            promptAfter = prompt;
            return true;
        }

        elapsedMs += waitMs;
    }

    return false;
}

int StartShellLocked()
{
    ResetSessionLocked(true);

    int masterFd = -1;
    pid_t pid = forkpty(&masterFd, nullptr, nullptr, nullptr);
    if (pid < 0) {
        return -1;
    }

    if (pid == 0) {
        setenv("TERM", "xterm-256color", 1);
        setenv("HISTFILE", "/dev/null", 1);
        execl("/bin/bash", "bash", "--noprofile", "--norc", "-i", static_cast<char*>(nullptr));
        _exit(127);
    }

    gMasterFd = masterFd;
    gShellPid = pid;

    int flags = fcntl(gMasterFd, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(gMasterFd, F_SETFL, flags | O_NONBLOCK);
    }

    std::string initCmd = "stty -echo\n";
    initCmd += "export PS1=''\n";
    initCmd += "export PROMPT_COMMAND='printf \"";
    initCmd += PROMPT_MARKER;
    initCmd += "%s@%s:%s$ \" \"$USER\" \"$HOSTNAME\" \"$PWD\"'\n";
    initCmd += "bind 'set enable-bracketed-paste off'\n";
    if (!WriteAllLocked(initCmd)) {
        ResetSessionLocked(true);
        return -1;
    }

    std::string output;
    std::string prompt;
    if (!WaitForPromptLocked(output, prompt, SESSION_BOOT_MS)) {
        ResetSessionLocked(true);
        return -1;
    }

    UpdatePromptCacheLocked(prompt);

    while (SelectReadableLocked(20)) {
        if (!DrainReadyOutputLocked()) {
            break;
        }
        std::string discard;
        bool promptReady = false;
        std::string promptAfter;
        ConsumeProcessedOutputLocked(discard, promptReady, promptAfter);
    }

    return 0;
}

int EnsureSessionLocked()
{
    if (gMasterFd >= 0 && IsShellAliveLocked()) {
        return 0;
    }

    return StartShellLocked();
}

}  // namespace

int InitSession()
{
    std::lock_guard<std::mutex> lock(gSessionMutex);

    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        gCurrentDir = "/";
    } else {
        gCurrentDir = cwd;
    }

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        gHostname = hostname;
    } else {
        gHostname = "unknown";
    }

    struct passwd* pwd = getpwuid(getuid());
    if (pwd != nullptr) {
        gUsername = pwd->pw_name;
    } else {
        gUsername = "user";
    }

    return EnsureSessionLocked();
}

std::string GetCurrentDir()
{
    std::lock_guard<std::mutex> lock(gSessionMutex);
    return gCurrentDir;
}

std::string GetPrompt()
{
    std::lock_guard<std::mutex> lock(gSessionMutex);
    if (EnsureSessionLocked() != 0) {
        return gUsername + "@" + gHostname + ":" + gCurrentDir + "$ ";
    }

    return gPrompt.empty() ? (gUsername + "@" + gHostname + ":" + gCurrentDir + "$ ") : gPrompt;
}

int SendCommand(const std::string& cmd, std::string& promptBefore)
{
    if (cmd.empty()) {
        return -1;
    }

    std::lock_guard<std::mutex> lock(gSessionMutex);
    if (EnsureSessionLocked() != 0) {
        return -1;
    }

    promptBefore = gPrompt.empty() ? (gUsername + "@" + gHostname + ":" + gCurrentDir + "$ ") : gPrompt;
    std::string request = cmd + "\n";
    if (!WriteAllLocked(request)) {
        ResetSessionLocked(true);
        return -1;
    }

    gWaitingForPrompt = true;
    return 0;
}

int ReadOutput(std::string& output, std::string& promptAfter, bool& promptReady, int waitMs)
{
    std::lock_guard<std::mutex> lock(gSessionMutex);
    if (EnsureSessionLocked() != 0) {
        return -1;
    }

    if (!ReadOutputChunkLocked(output, promptAfter, promptReady, waitMs)) {
        ResetSessionLocked(false);
        return -1;
    }

    return 0;
}

int Interrupt()
{
    std::lock_guard<std::mutex> lock(gSessionMutex);
    if (EnsureSessionLocked() != 0) {
        return -1;
    }

    char interruptChar = 0x03;
    std::string data(1, interruptChar);
    if (!WriteAllLocked(data)) {
        ResetSessionLocked(true);
        return -1;
    }

    gWaitingForPrompt = true;
    return 0;
}

int ExecuteCommand(const std::string& cmd, std::string& output, int timeoutMs)
{
    output.clear();
    std::string promptBefore;
    if (SendCommand(cmd, promptBefore) != 0) {
        output = "shell write failed\n";
        return -1;
    }

    std::string promptAfter;
    if (WaitForPromptLocked(output, promptAfter, timeoutMs)) {
        return 0;
    }

    if (Interrupt() != 0) {
        return -1;
    }

    std::string interruptOutput;
    if (WaitForPromptLocked(interruptOutput, promptAfter, INTERRUPT_WAIT_MS)) {
        output += interruptOutput;
        if (!output.empty() && output.back() != '\n') {
            output += '\n';
        }
        output += "^C\n";
        return -1;
    }

    output += "[terminal session interrupted]\n";
    return -1;
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
