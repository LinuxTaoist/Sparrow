/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SampleTcpClient.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
#include <mutex>
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
    std::mutex epFdMutex;
    auto pEpoll = EpollEventHandler::GetInstance();
    auto tcpClient = make_shared<PTcpClient>([&](int sock, void *arg) {
        PTcpClient* pCliObj = (PTcpClient*)arg;
        if (pCliObj == nullptr) {
            SPR_LOGE("pCliObj is nullptr\n");
            return;
        }

        std::string rBuf;
        int rc = pCliObj->Read(sock, rBuf);
        if (rc > 0) {
            SPR_LOGD("# RECV [%d]> %s\n", sock, rBuf.c_str());
        } else {
            SPR_LOGD("## CLOSE [%d]\n", sock);
            std::lock_guard<std::mutex> lock(epFdMutex);
            pCliObj->Close();
        }
    });

    tcpClient->AsTcpClient(true, "192.168.0.104", 1883);
    std::thread wThread([&]{
        while(true) {
            std::lock_guard<std::mutex> lock(epFdMutex);
            tcpClient->Write(tcpClient->GetEvtFd(), "Hello World");
            sleep(1);
        }
    });

    pEpoll->EpollLoop();
    wThread.join();
    return 0;
}
