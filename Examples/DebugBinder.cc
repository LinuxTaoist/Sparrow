/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugBinder.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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
#include "IBinderManager.h"

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

void InitServer(std::shared_ptr<Binder>& binder)
{
    IBinderManager* pObj = IBinderManager::GetInstance();
    binder = pObj->AddService(SERVICE_NAME);
}

int Server(const std::shared_ptr<Binder>& binder)
{
    std::shared_ptr<Parcel> pReqParcel = nullptr;
    std::shared_ptr<Parcel> pRspParcel = nullptr;

    binder->GetParcel(pReqParcel, pRspParcel);
    if (pReqParcel == nullptr || pRspParcel == nullptr) {
        SPR_LOGE("GetParcel failed\n");
        return -1;
    }


    SPR_LOGD("Server start\n");

    do {
        int cmd = 0;
        pReqParcel->Wait();
        pReqParcel->ReadInt(cmd);
        switch(cmd)
        {
            case CMD_TEST:
            {
                SPR_LOGD("CMD_TEST\n");
                pRspParcel->WriteInt(0);
                pRspParcel->Post();
                break;
            }

            case CMD_SUM:
            {
                SPR_LOGD("CMD_SUM\n");
                int a = 0, b = 0;
                pReqParcel->ReadInt(a);
                pReqParcel->ReadInt(b);

                int sum = a + b;
                pRspParcel->WriteInt(sum);
                pRspParcel->WriteInt(0);
                pRspParcel->Post();
                break;
            }

            case CMD_VEC:
            {
                std::vector<int> vec = {1, 2, 3, 4, 5};
                pRspParcel->WriteVector(vec);
                pRspParcel->WriteInt(0);
                pRspParcel->Post();
                break;
            }

            case CMD_CUST_VEC:
            {
                std::vector<STestData> vec = {{1,1}, {2,2}, {3,4}};
                pRspParcel->WriteVector(vec);
                pRspParcel->WriteInt(0);
                pRspParcel->Post();
                break;
            }

            default:
            {
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
    IBinderManager* pObj = IBinderManager::GetInstance();
    std::shared_ptr<IBinder> pBinder = pObj->GetService(SERVICE_NAME);
    if (pBinder == nullptr) {
        SPR_LOGE("GetService failed\n");
        return -1;
    }

    std::shared_ptr<Parcel> pReqParcel = nullptr;
    std::shared_ptr<Parcel> pRspParcel = nullptr;
    pBinder->GetParcel(pReqParcel, pRspParcel);
    if (pReqParcel == nullptr || pRspParcel == nullptr) {
        SPR_LOGE("GetParcel failed!\n");
        return -1;
    }

    SPR_LOGD("Client start\n");
    usage();

    do
    {
        SPR_LOGD("Input: \n");
        std::cin >> in;
        switch (in)
        {
            case '0':
            {
                pReqParcel->WriteInt(CMD_TEST);
                pReqParcel->Post();

                int ret = 0;
                pRspParcel->Wait();
                pRspParcel->ReadInt(ret);
                SPR_LOGD("ret: %d\n", ret);
                break;
            }

            case '1':
            {
                pReqParcel->WriteInt(CMD_SUM);
                pReqParcel->WriteInt(10);
                pReqParcel->WriteInt(20);
                pReqParcel->Post();

                int sum = 0, ret = 0;
                pRspParcel->Wait();
                pRspParcel->ReadInt(sum);
                pRspParcel->ReadInt(ret);
                SPR_LOGD("sum = %d, ret = %d\n", sum, ret);
                break;
            }

            case '2':
            {
                pReqParcel->WriteInt(CMD_VEC);
                pReqParcel->Post();

                int ret = 0;
                std::vector<int> vec;
                pRspParcel->Wait();
                pRspParcel->ReadVector(vec);
                pRspParcel->ReadInt(ret);
                for (auto v : vec) {
                    SPR_LOGD("vec: %d\n", v);
                }
                break;
            }

            case '3':
            {
                pReqParcel->WriteInt(CMD_CUST_VEC);
                pReqParcel->Post();

                int ret = 0;
                std::vector<STestData> vec;
                pRspParcel->Wait();
                pRspParcel->ReadVector(vec);
                pRspParcel->ReadInt(ret);
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
    std::shared_ptr<Binder> binder;
    InitServer(binder);

    pid_t pid = fork();
    if (pid == -1) {
        SPR_LOGE("fork failed! (%s)", strerror(errno));
    } else if (pid == 0) {
        Server(binder);
    } else {
        Client();
    }

    return 0;
}
