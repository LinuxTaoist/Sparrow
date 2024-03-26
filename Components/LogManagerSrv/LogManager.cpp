/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : LogManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *  @TODO:
 *  The component prints log directly to the library, not through LogManager.
 *  LogManager only used to monitor real-time log status and manage log storage.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <memory>
#include "SprMediatorIpcProxy.h"
#include "LogManager.h"

using namespace std;
using namespace InternalEnum;

#define SPR_LOGD(fmt, args...) printf("%d LOGM D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d LOGM W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d LOGM E: " fmt, __LINE__, ##args)

LogManager::LogManager(ModuleIDType id, const std::string& name)
            : SprObserver(id, name, make_shared<SprMediatorIpcProxy>())
{

}

LogManager::~LogManager()
{

}

int LogManager::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Receive msg: 0x%x\n", msg.GetMsgId());

    return 0;
}

