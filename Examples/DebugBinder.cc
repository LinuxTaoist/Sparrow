/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugBinder.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/31
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/31 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "CommonMacros.h"
#include "BindInterface.h"

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)
#define SPR_LOGD(fmt, args...) printf("%d DebugBinder D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d DebugBinder E: " fmt, __LINE__, ##args)

#define SERVICE_NAME "DebugBinder"

enum TestBinderCmd {
    CMD_TEST = 0,
    CMD_SUM,
    CMD_VEC,
    CMD_CUST_VEC,
    CMD_MAX
};

struct STestData {
    int value1;
    int value2;
};
int Server()
{
    std::shared_ptr<Parcel> pReqParcel = nullptr;
    std::shared_ptr<Parcel> pRspParcel = nullptr;

    BindInterface::GetInstance()->InitializeServiceBinder(SERVICE_NAME, pReqParcel, pRspParcel);
    if (pReqParcel == nullptr || pRspParcel == nullptr) {
        SPR_LOGE("GetParcel failed\n");
        return -1;
    }

    do {
        int cmd = 0;
        NONZERO_CHECK_RET(pReqParcel->Wait());
        NONZERO_CHECK_RET(pReqParcel->ReadInt(cmd));
        switch(cmd) {
            case CMD_TEST: {
                SPR_LOGD("CMD_TEST\n");
                NONZERO_CHECK_RET(pRspParcel->WriteInt(0));
                NONZERO_CHECK_RET(pRspParcel->Post());
                break;
            }
            case CMD_SUM: {
                SPR_LOGD("CMD_SUM\n");
                int a = 0, b = 0;
                NONZERO_CHECK_RET(pReqParcel->ReadInt(a));
                NONZERO_CHECK_RET(pReqParcel->ReadInt(b));

                int sum = a + b;
                NONZERO_CHECK_RET(pRspParcel->WriteInt(sum));
                NONZERO_CHECK_RET(pRspParcel->WriteInt(0));
                NONZERO_CHECK_RET(pRspParcel->Post());
                break;
            }
            case CMD_VEC: {
                std::vector<int> vec = {1, 2, 3, 4, 5};
                NONZERO_CHECK_RET(pRspParcel->WriteVector(vec));
                NONZERO_CHECK_RET(pRspParcel->WriteInt(0));
                NONZERO_CHECK_RET(pRspParcel->Post());
                break;
            }
            case CMD_CUST_VEC: {
                std::vector<STestData> vec = {{1,1}, {2,2}, {3,4}};
                NONZERO_CHECK_RET(pRspParcel->WriteVector(vec));
                NONZERO_CHECK_RET(pRspParcel->WriteInt(0));
                NONZERO_CHECK_RET(pRspParcel->Post());
                break;
            }
            default: {
                SPR_LOGE("Unknown cmd: %d\n", cmd);
                break;
            }
        }
    } while(1);

    return 0;
}

void usage()
{
    SPR_LOG("------------------------------------------------------------------\n"
            "Usage:\n"
            "0: CMD_TEST\n"
            "1: CMD_SUM\n"
            "2: CMD_VEC\n"
            "3: CMD_CUST_VEC\n"
            "h: Help\n"
            "q: Quit\n"
            "------------------------------------------------------------------\n"
    );
}

int Client()
{
    char in = 0;
    std::shared_ptr<Parcel> pReqParcel = nullptr;
    std::shared_ptr<Parcel> pRspParcel = nullptr;

    BindInterface::GetInstance()->InitializeClientBinder(SERVICE_NAME, pReqParcel, pRspParcel);
    if (pReqParcel == nullptr || pRspParcel == nullptr) {
        SPR_LOGE("GetParcel failed!\n");
        return -1;
    }

    SPR_LOGD("Client start\n");
    usage();

    do {
        SPR_LOGD("Input: \n");
        std::cin >> in;
        switch (in) {
            case '0': {
                NONZERO_CHECK_RET(pReqParcel->WriteInt(CMD_TEST));
                NONZERO_CHECK_RET(pReqParcel->Post());

                int ret = 0;
                NONZERO_CHECK_RET(pRspParcel->TimedWait());
                NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
                SPR_LOGD("ret: %d\n", ret);
                break;
            }
            case '1': {
                NONZERO_CHECK_RET(pReqParcel->WriteInt(CMD_SUM));
                NONZERO_CHECK_RET(pReqParcel->WriteInt(10));
                NONZERO_CHECK_RET(pReqParcel->WriteInt(20));
                NONZERO_CHECK_RET(pReqParcel->Post());

                int sum = 0, ret = 0;
                NONZERO_CHECK_RET(pRspParcel->TimedWait());
                NONZERO_CHECK_RET(pRspParcel->ReadInt(sum));
                NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
                SPR_LOGD("sum = %d, ret = %d\n", sum, ret);
                break;
            }
            case '2': {
                NONZERO_CHECK_RET(pReqParcel->WriteInt(CMD_VEC));
                NONZERO_CHECK_RET(pReqParcel->Post());

                int ret = 0;
                std::vector<int> vec;
                NONZERO_CHECK_RET(pRspParcel->TimedWait());
                NONZERO_CHECK_RET(pRspParcel->ReadVector(vec));
                NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
                for (auto v : vec) {
                    SPR_LOGD("vec: %d\n", v);
                }
                break;
            }
            case '3': {
                NONZERO_CHECK_RET(pReqParcel->WriteInt(CMD_CUST_VEC));
                NONZERO_CHECK_RET(pReqParcel->Post());

                int ret = 0;
                std::vector<STestData> vec;
                NONZERO_CHECK_RET(pRspParcel->TimedWait());
                NONZERO_CHECK_RET(pRspParcel->ReadVector(vec));
                NONZERO_CHECK_RET(pRspParcel->ReadInt(ret));
                for (auto v : vec) {
                    SPR_LOGD("vec: %d, %d\n", v.value1, v.value2);
                }
                break;
            }
            case 'h':
                usage();
                break;

            case 'q':
                break;
            default:
                break;
        }

    } while(1);

    return 0;
}

int main(int argc, const char *argv[])
{
    pid_t pid = fork();
    if (pid == -1) {
        SPR_LOGE("fork failed! (%s)", strerror(errno));
    } else if (pid == 0) {
        Server();
    } else {
        usleep(10000);
        Client();
    }

    return 0;
}
