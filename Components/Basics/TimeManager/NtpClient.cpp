/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : NtpClient.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/11/21
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/11/21 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <memory>
#include <cstring>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "SprLog.h"
#include "NtpClient.h"

using namespace std;

#define SPR_LOGD(fmt, args...) LOGD("NtpClient", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("NtpClient", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("NtpClient", fmt, ##args)

NtpClient::NtpClient(const std::string addr, uint16_t port, const std::function<void(double)>& cb)
    : mIsReady(false), mPort(port), mAddr(addr), mCb(cb)
{
    InitNtpClient();
}

NtpClient::~NtpClient()
{
}

int32_t NtpClient::SendTimeRequest()
{
    int ret = -1;
    if (!mNtpCliPtr) {
        SPR_LOGE("mNtpCliPtr is nullptr\n");
        return ret;
    }

    if (!mIsReady) {
        string IPAddress = mNtpCliPtr->ResolveHostToIp(mAddr);
        if (IPAddress.empty()) {
            SPR_LOGE("ResolveHostToIp %s failed!\n", mAddr.c_str());
            return ret;
        }

        SPR_LOGD("Creating UDP %s:%d\n", mAddr.c_str(), mPort);
        ret = mNtpCliPtr->AsUdpClient(IPAddress.c_str(), mPort);
        if (ret == -1) {
            SPR_LOGE("As UDP %s:%d failed!\n", IPAddress.c_str(), mPort);
            return ret;
        }

        mNtpCliPtr->AddToPoll();
    }

    if (mNtpCliPtr) {
        std::string bytes;
        ret = mNtpCliPtr->Write(mNtpCliPtr->GetEpollFd(), bytes);
    }

    return ret;
}

int32_t NtpClient::InitNtpClient()
{
    mNtpCliPtr = make_shared<PSocket>(AF_INET, SOCK_STREAM, 0, [&](int sock, void *arg) {
        PSocket* pCliObj = (PSocket*)arg;
        if (pCliObj == nullptr) {
            SPR_LOGE("PSocket is nullptr\n");
            return;
        }

        std::string rBuf;
        int rc = pCliObj->Read(sock, rBuf);
        if (rc > 0) {
            SPR_LOGD("# RECV [%d]> %d\n", sock, rBuf.size());
            HandleNtpBytes(rBuf);
        } else {
            SPR_LOGD("# CLOSE [%d]\n", sock);
        }

        // Only once read
        mIsReady = false;
        pCliObj->DelFromPoll();
        pCliObj->Close();
    });

    return mNtpCliPtr ? 0 : -1;
}

int32_t NtpClient::HandleNtpBytes(const std::string& bytes)
{
    if (mCb) {
        double time = 0;
        mCb(time);
    }

    return 0;
}
