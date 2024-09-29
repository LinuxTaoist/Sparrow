/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_watch.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/23
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/04/23 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include <signal.h>
#include "GeneralUtils.h"
#include "MainMenu.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

int main(int argc, const char *argv[])
{
    GeneralUtils::InitSignalHandler([](int signum) {
	    SPR_LOG("Receive signal: %d!\n", signum);

        switch (signum) {
            case SIGHUP:    // 终端挂断
            case SIGINT:    // 用户中断（Ctrl+C）
            case SIGQUIT:   // 退出，带core dump，调试用
            case SIGILL:    // 非法指令, 实际中可能需要更详细的错误处理或日志记录
            case SIGTRAP:   // 调试陷阱
            case SIGABRT:   // 应用异常中止
            case SIGBUS:    // 总线错误
            case SIGFPE:    // 浮点错误
            case SIGSEGV:   // 段错误, 这些通常表示严重错误，记录日志后可能需要退出
            case SIGPIPE:   // 管道破裂
            case SIGALRM:   // 定时器信号
            case SIGTERM:   // 请求进程终止, 清理并退出
                break;

            case SIGUSR1:   // 用户自定义信号1
            case SIGUSR2:   // 用户自定义信号2
            default:
                break;
        }
    });

    theMainMenu.MenuLoop();
    return 0;
}