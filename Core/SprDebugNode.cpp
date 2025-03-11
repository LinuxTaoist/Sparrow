/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprDebugNode.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/12/06
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/12/06 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <atomic>
#include <algorithm>
#include <errno.h>
#include <string.h>
#include "SprLog.h"
#include "SprDebugNode.h"
#include "CoreTypeDefs.h"
#include "CommonMacros.h"
#include "GeneralUtils.h"
#include "CommonTypeDefs.h"
#include "SprObserverWithMQueue.h"

#define LOG_TAG "SprDebugNode"

#define DEFAULT_CMD_MAX_SIZE   200

static std::atomic<bool> gObjAlive(true);

SprDebugNode::SprDebugNode() : mMaxNum(DEFAULT_CMD_MAX_SIZE)
{
    RegisterBuildinCmds();
}

SprDebugNode::~SprDebugNode()
{
    gObjAlive = false;
}

SprDebugNode* SprDebugNode::GetInstance()
{
    if (!gObjAlive) {
        return nullptr;
    }

    static SprDebugNode instance;
    return &instance;
}

std::string SprDebugNode::GetDebugPath()
{
    return mPipePath;
}

int32_t SprDebugNode::InitPipeDebugNode(const std::string& path)
{
    if (mpDebugNode && mpDebugNode->IsReady()) {
        SPR_LOGD("Already init! path = %s\n", mPipePath.c_str());
        return 0;
    }

    mpDebugNode = std::make_shared<PPipe>(path, [&](ssize_t size, std::string bytes, void* arg) {
        auto pPipe = reinterpret_cast<PPipe*>(arg);
        if (!pPipe) {
            SPR_LOGE("pPipe is nullptr!\n");
            return;
        }

        bytes.erase(std::find_if(bytes.rbegin(), bytes.rend(), [](unsigned char ch) {
            return ch != '\r' && ch != '\n';
        }).base(), bytes.end());

        bool found = false;
        std::vector<std::string> args = GeneralUtils::Split(bytes, ' ');
        SPR_LOGD("Recv bytes: %s num = %d\n", bytes.c_str(), args.size());
        if (args.empty()) {
            return;
        }

        // buildin cmds
        for (const auto& blnPair : mBuildinCmds) {
            std::string cmd = blnPair.first;
            if (args[0].compare(0, cmd.size(), cmd) == 0) {
                auto& func = blnPair.second.second;
                func(args);
                found = true;
            }
        }

        // user cmds
        for (const auto& ownerPair : mDebugOwners) {
            auto& cmdMap = ownerPair.second.mCmdMap;
            for (const auto& cmdPair : cmdMap) {
                std::string cmd = cmdPair.first;
                if (args[0].compare(0, cmd.size(), cmd) == 0) {
                    auto& func = cmdPair.second.second;
                    func(args);
                    found = true;
                }
            }
        }

        if (!found) {
            SPR_LOGE("Unknown cmd: %s\n", bytes.c_str());
        }
    }, this);

    if (!mpDebugNode || !mpDebugNode->IsReady()) {
        SPR_LOGE("Init pipe %s failed! (%s)\n", path.c_str(), strerror(errno));
        return -1;
    }

    mPipePath = path;
    mpDebugNode->AddToPoll();
    SPR_LOGD("Init pipe node %s success!\n", path.c_str());
    return 0;
}

int32_t SprDebugNode::RegisterBuildinCmds()
{
    mBuildinCmds["help"]          = { "Dump all cmds", std::bind(&SprDebugNode::DebugDumpAllOwners, this, std::placeholders::_1)};
    mBuildinCmds["version"]       = { "Dump version", std::bind(&SprDebugNode::DebugDumpVersion, this, std::placeholders::_1)};
    mBuildinCmds["mqs"]           = { "Dump all message queues", std::bind(&SprDebugNode::DebugDumpCurMQs, this, std::placeholders::_1)};
    return 0;
}

int32_t SprDebugNode::RegisterCmd(const std::string& owner, const std::string& cmd, const std::string& desc, DebugCmdFunc func)
{
    std::lock_guard<std::mutex> lock(mMutex);
    auto& pair = mDebugOwners[owner];
    pair.mCmdMap[cmd] = {desc, func};
    return 0;
}

int32_t SprDebugNode::UnregisterCmd(const std::string& owner)
{
    std::lock_guard<std::mutex> lock(mMutex);
    if (mDebugOwners.find(owner) == mDebugOwners.end()) {
        return -1;
    }

    mDebugOwners.erase(owner);
    return 0;
}

int32_t SprDebugNode::UnregisterCmd(const std::string& owner, const std::string& cmd)
{
    std::lock_guard<std::mutex> lock(mMutex);
    if (mDebugOwners.find(owner) == mDebugOwners.end()) {
        return -1;
    }

    auto& cmdMaps = mDebugOwners[owner].mCmdMap;
    if (cmdMaps.find(cmd) == cmdMaps.end()) {
        return -1;
    }

    cmdMaps.erase(cmd);
    return 0;
}

void SprDebugNode::DebugDumpAllOwners(const std::vector<std::string>& args)
{
    SPR_LOGD("==============================================================================\n");
    SPR_LOGD("                     Debug Command List                                       \n");
    SPR_LOGD("==============================================================================\n");

    int32_t mIndex = 0, cIndex = 0;     // modle index, cmd index
    int32_t total = (int32_t)mBuildinCmds.size();
    SPR_LOGD(" %3d. %-20s      Total: %2d", ++mIndex, "Built-in Commands", total);
    for (const auto& pair : mBuildinCmds) {
        const std::string prefix = (++cIndex == total) ? "└──" : "├──";
        SPR_LOGD("      %s %-20s: %-20s \n", prefix.c_str(), pair.first.c_str(), pair.second.first.c_str());
    }

    SPR_LOGD("\n");
    for (const auto& ownerPair : mDebugOwners) {
        cIndex = 0;
        total = (int32_t)ownerPair.second.mCmdMap.size();
        SPR_LOGD(" %3d. %-20s      Total: %2d", ++mIndex, ownerPair.first.c_str(), (int32_t)ownerPair.second.mCmdMap.size());
        for (const auto& cmdPair : ownerPair.second.mCmdMap) {
            const std::string prefix = (++cIndex == total) ? "└──" : "├──";
            SPR_LOGD("      %s %-20s: %-20s \n", prefix.c_str(), cmdPair.first.c_str(), cmdPair.second.first.c_str());
        }
        SPR_LOGD("\n");
    }

    SPR_LOGD("==============================================================================\n");
    SPR_LOGD("   E.g. echo help > %-22s  \n", mPipePath.c_str());
    SPR_LOGD("==============================================================================\n");
}

void SprDebugNode::DebugDumpVersion(const std::vector<std::string>& args)
{
    SPR_LOGD("==============================================================================\n");
    SPR_LOGD("                        Version About                                         \n");
    SPR_LOGD("==============================================================================\n");
    SPR_LOGD("- CommonTypeDefs.h: %s\n", COMMON_TYPE_DEFS_VERSION);
    SPR_LOGD("- CommonMacros.h  : %s\n", COMMON_MACROS_VERSION);
    SPR_LOGD("- CoreTypeDefs.h  : %s\n", CORE_TYPE_DEFS_VERSION);
}

void SprDebugNode::DebugDumpCurMQs(const std::vector<std::string>& args)
{
    std::vector<SMQStatus> tmpMQAttrs;
    SprObserverWithMQueue::GetAllMQStatus(tmpMQAttrs);

    SPR_LOGD("====================================================================================================\n");
    SPR_LOGD("                                   Show All Message Queues                                          \n");
    SPR_LOGD("====================================================================================================\n");
    //          %6d  %7ld  %7ld  %7ld  %s  %6ld %7u %7u  %6u  %s
    SPR_LOGD(" HANDLE  MNLIMIT  MNPUSED  MNCUSED  BLOCK    MLLIMIT MLPUSED MIDLAST  MTOTAL  QNAME\n");
    SPR_LOGD("====================================================================================================\n");
    for (const auto& mqInfo : tmpMQAttrs) {
        SPR_LOGD(" %6d  %7ld  %7ld  %7ld  %s  %6ld %7u %7u  %6u  %s\n", mqInfo.handle, mqInfo.mqAttr.mq_maxmsg, mqInfo.maxCount,
                    mqInfo.mqAttr.mq_curmsgs, (mqInfo.mqAttr.mq_flags & O_NONBLOCK) ? "NONBLOCK" : "BLOCK  ",
                    mqInfo.mqAttr.mq_msgsize, mqInfo.maxBytes, mqInfo.lastMsg, mqInfo.total % 100000, mqInfo.mqName);
    }
    SPR_LOGD("\n");
}

int32_t SprDebugNode::SetMaxNum(int32_t num)
{
    mMaxNum = num;
    return 0;
}

int32_t SprDebugNode::GetMaxNum()
{
    return mMaxNum;
}

int32_t SprDebugNode::GetCurNum()
{
    return (int32_t)mDebugOwners.size();
}
