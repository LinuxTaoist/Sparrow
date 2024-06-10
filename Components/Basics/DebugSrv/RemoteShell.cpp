/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : RemoteShell.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/28
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include "SprLog.h"
#include "RemoteShell.h"

#define SPR_LOGD(fmt, args...) LOGD("RemoteShell", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("RemoteShell", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("RemoteShell", fmt, ##args)

#define BUFFER_SIZE 1024

RemoteShell::RemoteShell()
{
    Init();
}

RemoteShell::~RemoteShell()
{

}

int RemoteShell::Init()
{
    mEpollPtr = std::make_shared<EpollEventHandler>(0, 5000);
    mTcpSrvPtr = std::make_shared<PSocket>(AF_INET, SOCK_STREAM, 0, [](int cli, void* pData) {
        dup2(cli, STDIN_FILENO);
        dup2(cli, STDOUT_FILENO);
        dup2(cli, STDERR_FILENO);

        ssize_t readBytes = 0;
        char buffer[BUFFER_SIZE] = {0};
        readBytes = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
        if (readBytes > 0) {
            buffer[readBytes] = '\0';
            if (strcmp(buffer, "exit\n") == 0) {
                return;
            }

            int ret = system(buffer);
            if (ret == -1) {
                SPR_LOGE("system failed! (%s)\n", buffer);
            }
        }
    });

    return 0;
}

int RemoteShell::Enable()
{
    if (mRcvThread.joinable()) {
        mRcvThread = std::thread([](RemoteShell* mySelf) {
            if (mySelf == nullptr) {
                SPR_LOGE("mySelf is nullptr!\n");
                return;
            }

            mySelf->mTcpSrvPtr->AsTcpServer(8080, 5);
            mySelf->mEpollPtr->AddPoll(mySelf->mTcpSrvPtr.get());
            mySelf->mEpollPtr->EpollLoop(true);
        }, this);
    }

    return 0;
}

int RemoteShell::Disable()
{
    if (mRcvThread.joinable()) {
        mEpollPtr->DelPoll(mTcpSrvPtr.get());
        mTcpSrvPtr->Close();
        mEpollPtr = nullptr;
        mRcvThread.join();
    }

    return 0;
}

