/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_rshellx.cc
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
#include <stdio.h>
#include <stdlib.h>
#include "SessionManager.h"

using namespace std;

#define SPR_LOGI(fmt, args...) printf(fmt, ##args)
#define SPR_LOGD(fmt, args...) printf(fmt, ##args)
#define SPR_LOGE(fmt, args...) printf(fmt, ##args)

int main(int argc, const char* argv[])
{
    if (argc != 2 && argc != 3) {
        SPR_LOGE("Usage: \n");
        SPR_LOGE(" As server: ./rshellx <port>\n");
        SPR_LOGE(" As client: ./rshellx <ip> <port>\n");
        return -1;
    }

    int ret = 0;
    auto pSMgr = SessionManager::GetInstance();
    if (argc == 2) {    // As tcp server
        uint16_t port = atoi(argv[1]);
        ret = pSMgr->AsTcpServer(port);
        SPR_LOGI("As server, port: %d\n", port);
    } else {            // As tcp client
        string ip = argv[1];
        uint16_t port = atoi(argv[2]);
        ret = pSMgr->AsTcpClient(ip, port);
        SPR_LOGI("As client, ip: %s, port: %d\n", ip.c_str(), port);
    }

    if (ret == 0) {
        pSMgr->EpollLoop();
    }

    return 0;
}
