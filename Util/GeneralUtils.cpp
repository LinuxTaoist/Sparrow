/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : GeneralUtils.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <random>
#include <chrono>
#include <random>
#include <stdarg.h>
#include <signal.h>
#include "GeneralUtils.h"

namespace GeneralUtils {

int RandomDecimalDigits(int digits)
{
    int maxValue = pow(10, digits) - 1;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, maxValue);

    return distribution(generator);
}

int InitSignalHandler(void (*signalHandler)(int))
{
    struct sigaction signal_action;

    signal_action.sa_handler = signalHandler;
    signal_action.sa_flags = 0;
    sigemptyset(&signal_action.sa_mask);

    sigaction(SIGHUP,    &signal_action, NULL);    // 终端挂断，重载配置或终止
    sigaction(SIGINT,    &signal_action, NULL);    // 用户中断（Ctrl+C）
    // sigaction(SIGQUIT,   &signal_action, NULL);    // 退出，带core dump，调试用
    // sigaction(SIGILL,    &signal_action, NULL);    // 非法指令
    // sigaction(SIGTRAP,   &signal_action, NULL);    // 调试陷阱
    // sigaction(SIGABRT,   &signal_action, NULL);    // 应用异常中止（abort函数）
    // sigaction(SIGBUS,    &signal_action, NULL);    // 总线错误，内存访问对齐问题
    // sigaction(SIGFPE,    &signal_action, NULL);    // 浮点错误，如除以0
    // sigaction(SIGUSR1,   &signal_action, NULL);    // 用户自定义信号1
    // sigaction(SIGSEGV,   &signal_action, NULL);    // 段错误，非法内存访问
    // sigaction(SIGUSR2,   &signal_action, NULL);    // 用户自定义信号2
    // sigaction(SIGPIPE,   &signal_action, NULL);    // 管道破裂，写入无读取端的管道
    // sigaction(SIGALRM,   &signal_action, NULL);    // 定时器信号
    sigaction(SIGTERM,   &signal_action, NULL);    // 请求进程终止

    return 0;
}
int SystemCmd(const char* format, ...)
{
    std::string out;

    va_list vlist;
    va_start(vlist, format);
    int ret = SystemCmd(out, format, vlist);

    va_end(vlist);
    return ret;
}

int SystemCmd(std::string& out, const char* format, ...)
{
    va_list vlist;
    va_start(vlist, format);

    char* fmt = nullptr;
    if (vasprintf(&fmt, format, vlist) == -1)
    {
        va_end(vlist);
        return -1;
    }

    va_end(vlist);
    if (fmt == nullptr)
    {
        return -1;
    }

    FILE *fp = popen(fmt, "r");
    if (fp == nullptr)
    {
        free(fmt);
        return -1;
    }

    out.clear();
    char buffer[256] = {0};
    while (fgets(buffer, sizeof(buffer), fp) != nullptr)
    {
        out.append(buffer);
    }

    free(fmt);
    int exitCode = pclose(fp);
    return (exitCode == 0) ? 0 : -1;
}

std::string RandomString(int len)
{
    std::string strRandom;
    const std::string seedStr = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(0, seedStr.size() - 1);

    for (int i = 0; i < len; i++) {
        int randomValue = distribution(generator);
        strRandom += seedStr[randomValue];
    }

    return strRandom;
}

std::string GetCurTimeStr()
{
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);

    time_t seconds = currentTime.tv_sec;
    struct tm* localTime = localtime(&seconds);
    int milliseconds = currentTime.tv_nsec / 1000000;

    char buffer[80];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
            localTime->tm_hour, localTime->tm_min, localTime->tm_sec, milliseconds);

    return std::string(buffer);
}

std::string GetSubstringAfterLastDelimiter(const std::string& str, char delimiter)
{
    std::string subStr;
    size_t found = str.find_last_of(delimiter);
    if (found != std::string::npos && found + 1 < str.length()) {
        subStr = str.substr(found + 1);
    }

    return subStr;
}

int GetCharAfterNthTarget(const std::string& str, char targetChar, int index, char& out)
{
    int count = 0;

    for(size_t i = 0; i < str.size(); ++i) {
        if(str[i] == targetChar) {
            ++count;
            if(count == index && i + 1 < str.size()) {
                out = str[i + 1];;
                return 0;
            }
        }
    }

    return -1;
}

int GetCharBeforeNthTarget(const std::string& str, char targetChar, int index, char& out)
{
    int count = 0;

    for(size_t i = 0; i < str.size(); ++i) {
        if(str[i] == targetChar) {
            ++count;
            if(count == index && i - 1 < str.size() && (int)i - 1 >= 0) {
                out = str[i - 1];;
                return 0;
            }
        }
    }

    return -1;
}

}; // namespace GeneralUtils
