/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SampleTcpServer.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/05/08
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/08 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <memory>
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include "PSocket.h"
#include "EpollEventHandler.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%d TcpServer D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d TcpServer E: " fmt, __LINE__, ##args)
int main(int argc, const char *argv[])
{
    EpollEventHandler *pEpoll = EpollEventHandler::GetInstance();
    auto tcpServer = make_shared<PSocket>(AF_INET, SOCK_STREAM, 0, [&](int cli, void *arg) {
        PSocket* pSrvObj = (PSocket*)arg;
        if (pSrvObj == nullptr) {
            SPR_LOGE("PSocket is nullptr\n");
            return;
        }

        auto tcpClient = make_shared<PSocket>(cli, [&](int sock, void *arg) {
            PSocket* pCliObj = (PSocket*)arg;
            if (pSrvObj == nullptr) {
                SPR_LOGE("PSocket is nullptr\n");
                return;
            }

            std::string rBuf;
            int rc = pCliObj->Read(sock, rBuf);
            if (rc > 0) {
                SPR_LOGD("client(%02d) rcv %s\n", sock, rBuf.c_str());

                std::string sBuf = "ACK";
                rc = pCliObj->Write(sock, sBuf);
            }
        });
        tcpClient->AsTcpClient();
    });

    tcpServer->AsTcpServer(8080, 5);
    pEpoll->AddPoll(tcpServer.get());
    pEpoll->EpollLoop(true);

    return 0;
}
