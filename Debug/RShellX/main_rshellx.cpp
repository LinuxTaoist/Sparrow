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
#include "LoginManager.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf(fmt, ##args)
#define SPR_LOGE(fmt, args...) printf(fmt, ##args)

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

    SPR_LOGD("Start RShellX server on port %d\n", port);
    LoginManager* pLoginMgr = LoginManager::GetInstance();
    pLoginMgr->Init();
    pLoginMgr->BuildConnectAsTcpServer(port);
    pLoginMgr->ConnectLoop();
    return 0;
}
