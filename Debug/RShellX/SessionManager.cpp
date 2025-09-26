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
#include <algorithm>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
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

int32_t SessionManager::AsTcpServer(uint16_t port)
{
    mpTcpServer = make_shared<PTcpServer>([&](int32_t cli, void* arg) {
        PTcpServer* pSrvObj = (PTcpServer*)arg;
        if (pSrvObj == nullptr) {
            SPR_LOGE("pSrvObj is nullptr\n");
            return;
        }

        auto tcpClient = make_shared<PTcpClient>(cli, [&](ssize_t ret, std::string bytes, void* arg) {
            PTcpClient* pCliObj = (PTcpClient*)arg;
            if (pCliObj == nullptr) {
                SPR_LOGE("pCliObj is nullptr!\n");
                return;
            }

            if (ret <= 0) {
                SPR_LOGE("# Client [%d] offline\n", pCliObj->GetEvtFd());
                mpPtyTerminals.remove_if([pCliObj](
                        std::pair<std::shared_ptr<PTcpClient>,
                        std::shared_ptr<PtyTerminal>> item) {
                        return (item.first->GetEvtFd() == pCliObj->GetEvtFd());
                });
                return;
            }

            auto it = std::find_if(mpPtyTerminals.begin(), mpPtyTerminals.end(),
                [&](std::pair< std::shared_ptr<PTcpClient>,
                              std::shared_ptr<PtyTerminal> > item) {
                        return (item.first->GetEvtFd() == pCliObj->GetEvtFd());
            });

            if (it != mpPtyTerminals.end() && it->second) {
                // Write cmd to pty
                it->second->Write(bytes);
            } else {
                SPR_LOGE("Not found pty terminal\n");
            }
        });

        std::shared_ptr<PtyTerminal> pPtyObj = make_shared<PtyTerminal>([&](int32_t ret, std::string bytes, void* arg) {
            PtyTerminal* pPty = (PtyTerminal*) arg;
            if (pPty == nullptr) {
                SPR_LOGE("pPty is nullptr!\n");
                return;
            }

            // Pty closed
            if (ret <= 0) {
                mpPtyTerminals.remove_if([pPty](std::pair<std::shared_ptr<PTcpClient>,
                    std::shared_ptr<PtyTerminal>> item) {
                        bool found = (item.second.get() == pPty);
                        if (found) {
                            SPR_LOGI("# Client [%d] offline", item.first->GetEvtFd());
                        }
                        return found;
                    }
                );

                return;
            }

            auto it = std::find_if(mpPtyTerminals.begin(), mpPtyTerminals.end(),
                [&](std::pair< std::shared_ptr<PTcpClient>,
                              std::shared_ptr<PtyTerminal> > item) {
                    return (item.second.get() == pPty);
            });

            if (it != mpPtyTerminals.end() && it->first) {
                // Write cmd to client
                it->first->Write(bytes);
            }
        });

        SPR_LOGD("# Client [%d] online\n", cli);
        pPtyObj->Init();
        tcpClient->AsTcpClient();
        mpPtyTerminals.push_back(std::make_pair(tcpClient, pPtyObj));
    });

    mpTcpServer->AsTcpServer(port, 5);
    return 0;
}

int32_t SessionManager::AsTcpClient(const std::string& ip, uint16_t port)
{
    static std::shared_ptr<PtyTerminal> pPtyObj = make_shared<PtyTerminal>([&](int32_t ret, std::string bytes, void* arg) {
        if (ret <= 0) {
            SPR_LOGE("Pty closed\n");
            exit(EXIT_FAILURE);
            return;
        }

        if (mpTcpClient) {
            mpTcpClient->Write(bytes);
        }
    });

    mpTcpClient = make_shared<PTcpClient>([&](ssize_t ret, std::string bytes, void* arg) {
        PTcpClient* pCliObj = (PTcpClient*)arg;
        if (pCliObj == nullptr) {
            SPR_LOGE("pCliObj is nullptr!\n");
            return;
        }

        if (ret <= 0) {
            SPR_LOGE("# Client [%d] offline\n", pCliObj->GetEvtFd());
            pCliObj->Close();
            exit(EXIT_FAILURE);
            return;
        }

        // Write cmd to pty
        pPtyObj->Write(bytes);
    });

    int rc1 = pPtyObj->Init();
    int rc2 = mpTcpClient->AsTcpClient(true, ip, port);
    return (rc1 == 0 && rc2 == 0) ? 0 : -1;
}

int32_t SessionManager::EpollLoop()
{
    EpollEventHandler::GetInstance()->EpollLoop();
    return 0;
}
