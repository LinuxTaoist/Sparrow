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
#include <list>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include "SprLog.h"
#include "GeneralUtils.h"
#include "RemoteShell.h"

#define SPR_LOGD(fmt, args...) LOGD("RemoteShell", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("RemoteShell", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("RemoteShell", fmt, ##args)

#define BUFFER_SIZE 1024

RemoteShell::RemoteShell()
{
    mEnable = false;
}

RemoteShell::~RemoteShell()
{

}

int RemoteShell::Init()
{
    if (mEnable) {
        SPR_LOGD("Remote shell already initialized!\n");
        return 0;
    }

    SPR_LOGD("Enter remote shell Init!\n");
    std::list<std::shared_ptr<PTcpClient>> clients;
    auto pEpoll = EpollEventHandler::GetInstance();
    mpTcpSrv = std::make_shared<PTcpServer>([&](int cli, void* arg) {
        PTcpServer* pSrvObj = (PTcpServer*)arg;
        if (pSrvObj == nullptr) {
            SPR_LOGE("pSrvObj is nullptr\n");
            return;
        }

        auto tcpClient = std::make_shared<PTcpClient>(cli, [&](int sock, void* arg) {
            PTcpClient* pCliObj = (PTcpClient*)arg;
            if (pCliObj == nullptr) {
                SPR_LOGE("pCliObj is nullptr\n");
                return;
            }

            // Mimics the input display format of a remote terminal session.
            // Example:
            // # pwd
            // /tmp/Release/Bin
            std::string rBuf;
            int rc = pCliObj->Read(sock, rBuf);
            if (rc > 0) {
                SPR_LOGD("# RECV CMD [%d]> [%d]%s\n", sock, rBuf.length(), rBuf.c_str());
                rBuf.erase(std::find_if(rBuf.rbegin(), rBuf.rend(), [](unsigned char ch) {
                    return ch != '\r' && ch != '\n';
                }).base(), rBuf.end());

                std::string out;
                int result = GeneralUtils::SystemCmd(out, rBuf.c_str());
                if (result == -1) {
                    out = "Invaild parameter! CMD: " + rBuf;
                }

                if (out.empty()) {
                    out = "No return. CMD: " + rBuf;
                }

                out += "\r\n# ";
                rc = pCliObj->Write(sock, out);
                if (rc > 0) {
                    SPR_LOGD("# SEND RET [%d]> %s\n", sock, out.c_str());
                }
            }

            if (rc <= 0) {
                clients.remove_if([sock, this, pCliObj, pEpoll](std::shared_ptr<PTcpClient>& v) {
                    pEpoll->DelPoll(pCliObj);
                    return (v->GetEvtFd() == sock);
                });
            }
        });

        tcpClient->AsTcpClient();
        clients.push_back(tcpClient);
    });

    if (!mRcvThread.joinable()) {
        mRcvThread = std::thread([&](RemoteShell* mySelf) {
            if (mySelf == nullptr) {
                SPR_LOGE("mySelf is nullptr!\n");
                return;
            }

            SPR_LOGD("Enter remote shell thread...\n");
            mySelf->mpTcpSrv->AsTcpServer(8080, 5);
            pEpoll->EpollLoop();
        }, this);
    }

    mEnable = true;
    SPR_LOGD("Exit from remote shell Init!\n");
    return 0;
}

int RemoteShell::DeInit()
{
    if (!mEnable) {
        SPR_LOGD("Remote shell uninitialized!\n");
        return 0;
    }

    SPR_LOGD("Enter remote shell DeInit!\n");
    if (mRcvThread.joinable()) {
        EpollEventHandler::GetInstance()->DelPoll(mpTcpSrv.get());
        mpTcpSrv->Close();
        mRcvThread.join();
    }

    mEnable = false;
    SPR_LOGD("Exit from remote shell DeInit!\n");
    return 0;
}

int RemoteShell::Enable()
{
    Init();
    return 0;
}

int RemoteShell::Disable()
{
    DeInit();
    return 0;
}

