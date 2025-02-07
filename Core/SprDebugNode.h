/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprDebugNode.h
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
#ifndef __SPR_DEBUG_NODE_H__
#define __SPR_DEBUG_NODE_H__

#include <map>
#include <list>
#include <mutex>
#include <memory>
#include <string>
#include <functional>
#include <stdint.h>
#include "PPipe.h"

using DebugCmdFunc = std::function<void(const std::string&)>;

class DebugFuncDetail
{
public:
    DebugFuncDetail() = default;
    DebugFuncDetail(const DebugFuncDetail&) = delete;
    DebugFuncDetail& operator=(const DebugFuncDetail&) = delete;
    DebugFuncDetail(DebugFuncDetail&&) = delete;
    DebugFuncDetail& operator=(DebugFuncDetail&&) = delete;
    ~DebugFuncDetail() = default;

    friend class SprDebugNode;

private:
    std::map<std::string, std::pair<std::string, DebugCmdFunc>> mCmdMap; // <cmd, <desc, func>>
};

class SprDebugNode
{
public:
    SprDebugNode(const SprDebugNode&) = delete;
    SprDebugNode& operator=(const SprDebugNode&) = delete;
    SprDebugNode(SprDebugNode&&) = delete;
    SprDebugNode& operator=(SprDebugNode&&) = delete;
    virtual ~SprDebugNode() = default;

    static SprDebugNode* GetInstance();
    std::string GetDebugPath();
    int32_t InitPipeDebugNode(const std::string& path);
    int32_t SetMaxNum(int32_t num);
    int32_t GetMaxNum();
    int32_t GetCurNum();

    /**
     * @brief Register/UnRegister debug cmd
     *
     * @param owner owner name
     * @param cmd cmd name
     * @param desc cmd desc
     * @param func cmd func
     * @return int32_t
     */
    int32_t RegisterBuildinCmds();
    int32_t RegisterCmd(const std::string& owner, const std::string& cmd, const std::string& desc, DebugCmdFunc func);
    int32_t UnregisterCmd(const std::string& owner);
    int32_t UnregisterCmd(const std::string& owner, const std::string& cmd);

    // Build in cmds
    void DebugDumpAllOwners(const std::string& args);
    void DebugDumpVersion(const std::string& args);

private:
    SprDebugNode();

private:
    int32_t mMaxNum;
    std::mutex mMutex;
    std::string mPipePath;
    std::shared_ptr<PPipe> mpDebugNode;
    std::map<std::string, std::pair<std::string, DebugCmdFunc>> mBuildinCmds; // <cmd, <desc, func>>
    std::map<std::string, DebugFuncDetail> mDebugOwners; // <owner, <cmd, <desc, func>>>
};

#endif // __SPR_DEBUG_NODE_H__
