/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SampleTcpClient.cc
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
#include <list>
#include <memory>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include "PSocket.h"
#include "EpollEventHandler.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%4d TcpClient D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d TcpClient E: " fmt, __LINE__, ##args)
int main(int argc, const char *argv[])
{
    EpollEventHandler *pEpoll = EpollEventHandler::GetInstance();
    auto tcpClient = make_shared<PSocket>(AF_INET, SOCK_STREAM, 0, [&](int sock, void *arg) {
        PSocket* pCliObj = (PSocket*)arg;
        if (pCliObj == nullptr) {
            SPR_LOGE("PSocket is nullptr\n");
            return;
        }

        std::string rBuf;
        int rc = pCliObj->Read(sock, rBuf);
        if (rc > 0) {
            SPR_LOGD("# RECV [%d]> %s\n", sock, rBuf.c_str());
        }
    });

    tcpClient->AsTcpClient(true, "127.0.0.1", 8080);
    pEpoll->AddPoll(tcpClient.get());

    std::thread wThread([&]{
        while(true) {
            int ret = tcpClient->Write(tcpClient->GetEpollFd(), "Hello World");
            sleep(1);
        }
    });

    pEpoll->EpollLoop(true);
    wThread.join();
    return 0;
}
