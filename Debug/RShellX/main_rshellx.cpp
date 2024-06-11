/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SampleTcpServer.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/06/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/06/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <list>
#include <memory>
#include <algorithm>
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include "PSocket.h"
#include "GeneralUtils.h"
#include "EpollEventHandler.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%4d TcpServer D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d TcpServer E: " fmt, __LINE__, ##args)
int main(int argc, const char *argv[])
{
    if (argc < 2) {
        SPR_LOGE("Usage: ./rshellx <port>\n");
        return -1;
    }

    short port = atoi(argv[1]);
    if (port <= 0) {
        SPR_LOGE("Invalid port: %d\n", port);
        return -1;
    }

    auto pEpoll = make_shared<EpollEventHandler>();

    std::list<std::shared_ptr<PSocket>> clients;
    auto tcpServer = make_shared<PSocket>(AF_INET, SOCK_STREAM, 0, [&](int cli, void *arg) {
        PSocket* pSrvObj = (PSocket*)arg;
        if (pSrvObj == nullptr) {
            SPR_LOGE("PSocket is nullptr\n");
            return;
        }

        auto tcpClient = make_shared<PSocket>(cli, [&](int sock, void *arg) {
            PSocket* pCliObj = (PSocket*)arg;
            if (pCliObj == nullptr) {
                SPR_LOGE("PSocket is nullptr\n");
                return;
            }

            // Mimics the input display format of a remote terminal session.
            // Example:
            // # pwd
            // /tmp/Release/Bin
            std::string rBuf;
            int rc = pCliObj->Read(sock, rBuf);
            if (rc > 0) {
                SPR_LOGD("# RECV [%d]> %s\n", sock, rBuf.c_str());
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
                    // SPR_LOGD("# SEND [%d]> %s\n", sock, out.c_str());
                } else {
                    SPR_LOGE("# SEND failed!\n");
                }
            }

            if (rc <= 0) {
                clients.remove_if([sock, pEpoll, pCliObj](shared_ptr<PSocket>& v) {
                    pEpoll->DelPoll(pCliObj);
                    return (v->GetEpollFd() == sock);
                });
            }
        });

        tcpClient->AsTcpClient();
        pEpoll->AddPoll(tcpClient.get());
        clients.push_back(tcpClient);
    });

    tcpServer->AsTcpServer(port, 5);
    pEpoll->AddPoll(tcpServer.get());
    pEpoll->EpollLoop(true);

    return 0;
}
