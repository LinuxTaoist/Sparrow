/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_service.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/03/26
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/26 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdio.h>
// #include <errno.h>
#include <unistd.h>
// #include <signal.h>
// #include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
// #include <sys/wait.h>
// #include <semaphore.h>
// #include <dirent.h>
// #include <map>      // std::map
// #include <thread>
// #include <memory>   // std::shared_ptr
// #include <condition_variable>   // std::condition_variable

#include "ServiceManager.h"

using namespace std;

// #define CONFIG_SUPPORT_SIGCHLD 0 // SIGCHLD不可靠。 1: 信号中断触发 0: 轮询

#define LOG(fmt, args...)  printf(fmt, ##args)
#define LOGD(fmt, args...)  printf("[%d] %-20s %-4d D: " fmt, getpid(), __FUNCTION__, __LINE__, ##args)
#define LOGE(fmt, args...)  printf("[%d] %-20s %-4d E: " fmt, getpid(), __FUNCTION__,__LINE__, ##args)

// typedef struct exeInfo {
//     char path[20];
//     int  times;
// } SExeInfo;

// using TMapPid2Path = std::map<int, shared_ptr<SExeInfo>>;

// const char PROC_PATH[] = "/proc";
// TMapPid2Path pidMap;

// static std::mutex sigchildMtx;
// static std::condition_variable sigchildCond;

// static bool existExeByProc(int pid)
// {
//     struct stat fileStat;
//     char pidPath[20] = {0};

//     snprintf(pidPath, sizeof(pidPath), "%s/%d", PROC_PATH, pid);
//     int ret = lstat(pidPath, &fileStat);
//     if (ret) {
//         //LOGD("%s lstat failed. errno = %d (%s)\n", pidPath, errno, strerror(errno));
//         return false;
//     }

//     // /proc/pid/ 为目录则当前进程正常
//     if (S_ISDIR(fileStat.st_mode)) {
//         return true;
//     }

//     return false;
// }

// static void dumpPidMapInfo(const TMapPid2Path& aMap)
// {
//     LOGD("PID     PATH                TIMES+\n");
//     LOGD("----------------------------------\n");
//     for (auto it = aMap.begin(); it != aMap.end(); ++it) {
//         LOGD("%-6d  %-20s %-2d \n", it->first, it->second->path, it->second->times);
//     }
//     LOGD("----------------------------------\n");
// }

// static void start_exe(const char* pExePath)
// {
//     static int pid = -1;

//     pid = fork();
//     if (pid == -1) {
//         LOGE("fork failed. errno = %d (%s).\n", errno, strerror(errno));
//     } else if (pid == 0) {          // 子进程
//         static int startCount = 0;

//         LOGD("Pull up %s (%d).\n", pExePath, ++startCount);
//         execl(pExePath, pExePath, nullptr);
//     } else {                        // 父进程
//         LOGD("Child fork pid: %d.\n", pid);

//         auto it = pidMap.begin();
//         for (; it != pidMap.end(); ++it) {
//             if (!strncmp(it->second->path, pExePath, strlen(pExePath))) {
//                 LOG("find %s %s.\n", it->second->path, pExePath);
//                 break;
//             }
//         }

//         if (it == pidMap.end()) {
//             shared_ptr<SExeInfo> spInfo = make_shared<SExeInfo>();

//             strncpy(spInfo->path, pExePath, sizeof(spInfo->path));
//             spInfo->times = 1;
//             pidMap.insert(pair<int, shared_ptr<SExeInfo>>(pid, spInfo));
//         } else {
//             it->second->times++;
//             pidMap.insert(pair<int, shared_ptr<SExeInfo>>(pid, it->second));
//             pidMap.erase(it);
//         }
//     }
// }

// #if CONFIG_SUPPORT_SIGCHLD
// static bool sigchildRcv = false;
// static void handler(int sig, siginfo_t* si, void* unused)
// {
//     LOGD("Receive sig: %d.\n", sig);
//     sigchildRcv = true;
//     sigchildCond.notify_one();
// }
// #endif

// Eg. ./exe /tmp/test_bin
int main(int argc, char* argv[])
{
//     if (argc < 2) {
//         LOGE("usage: %s [path].\n", argv[0]);
//         return 0;
//     }

// #if CONFIG_SUPPORT_SIGCHLD
//     struct sigaction sa;
//     sigemptyset(&sa.sa_mask);
//     sa.sa_sigaction = handler;
//     if (sigaction(SIGCHLD, &sa, NULL) == -1)
//     {
//         LOGE("sigaction failed! errno = %d (%s) \n", errno, strerror(errno));
//     }
// #endif

//     for (int i = 1; i < argc; i++) {
//         start_exe(argv[i]);
//     }


//     thread th1([&]() {
//         while(1) {

// #if !(CONFIG_SUPPORT_SIGCHLD)
//             int pid = 0, status = 0;
//             while( (pid = waitpid(-1, &status, WNOHANG)) > 0) {
//                 if (!existExeByProc(pid) && pidMap.count(pid)) {
//                     start_exe(pidMap[pid]->path);
//                 }
//             }
// #else
//             std::unique_lock<std::mutex> lk(sigchildMtx);
//             sigchildRcv = false;
//             sigchildCond.wait(lk, []{
//                 int pid = 0, status = 0;
//                 while( (pid = waitpid(-1, &status, WNOHANG)) > 0) {
//                     if (!existExeByProc(pid) && pidMap.count(pid)) {
//                         start_exe(pidMap[pid]->path);
//                     }
//                 }
//                 LOGD("Receive SIGCHLD.\n");
//                 return sigchildRcv;
//             });
//             lk.unlock();
// #endif

//             dumpPidMapInfo(pidMap);
//             sleep(1);
//         }
//     });

//     th1.join();

    ServiceManager theServiceManager;
    theServiceManager.Init();

    LOGD("Wait \n");
    while(1);
    return 0;
}

