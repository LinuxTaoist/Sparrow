/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SessionManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/13
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/13 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <atomic>
#include <sys/types.h>
#include <unistd.h>
#include "SprLog.h"
#include "PtyTerminal.h"
#include "SessionManager.h"
#include "EpollEventHandler.h"

using namespace std;

#define LOG_TAG "SessionM"

static std::atomic<bool> gObjAlive(true);

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{
    gObjAlive = false;
}

SessionManager* SessionManager::GetInstance()
{
    if (!gObjAlive) {
        return nullptr;
    }

    static SessionManager instance;
    return &instance;
}

int32_t SessionManager::Init(uint16_t port)
{
    EpollEventHandler* pEpoll = EpollEventHandler::GetInstance();
    mpTcpSrv = make_shared<PTcpServer>([&](int32_t cli, void* arg) {
        PTcpServer* pSrvObj = (PTcpServer*)arg;
        if (pSrvObj == nullptr) {
            SPR_LOGE("pSrvObj is nullptr\n");
            return;
        }

        auto tcpClient = make_shared<PTcpClient>(cli, [&](int32_t sock, void* arg) {
            PTcpClient* pCliObj = (PTcpClient*)arg;
            if (pCliObj == nullptr) {
                SPR_LOGE("pCliObj is nullptr\n");
                return;
            }

            std::string rBuf;
            int32_t rc = pCliObj->Read(sock, rBuf);
            if (rc <= 0) {
                mTcpClients.remove_if([sock, pEpoll, pCliObj](shared_ptr<PTcpClient>& v) {
                    pCliObj->Close();
                    return (v->GetEvtFd() == sock);
                });
                return;
            }

            // SPR_LOGD("# RECV [%d]> %s shellpid = %d\n", sock, rBuf.c_str(), mShellPid);
            pid_t pid = fork();
            if (pid < 0) {
                SPR_LOGE("fork failed\n");
                pCliObj->Close();
            } else if (pid == 0) {
                // child process
                pSrvObj->Close();
                std::shared_ptr<PtyTerminal> pPty = make_shared<PtyTerminal>();
                pPty->Init(cli);
                exit(EXIT_SUCCESS);
            } else {
                pCliObj->Close();
            }
        });
    });

    mpTcpSrv->AsTcpServer(port, 5);
    mpTcpSrv->AddToPoll();
    return 0;
}

int32_t SessionManager::EpollLoop()
{
    EpollEventHandler::GetInstance()->EpollLoop();
    return 0;
}
