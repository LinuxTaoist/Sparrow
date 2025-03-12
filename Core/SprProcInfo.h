/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprProcInfo.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/03/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_PROC_INFO_H__
#define __SPR_PROC_INFO_H__

#include <string>

class SprProcInfo
{
public:
    ~SprProcInfo();
    static SprProcInfo* GetInstance();

    void Init();
    void LoadInternalAttrs();
    void SetBootTimeUs(uint64_t us);
    void SetDebugPath(const std::string& debugPath);

    uint64_t GetBootTimeUs();
    std::string GetBootTimeString();
    std::string GetProcName();
    std::string GetDebugPath();

private:
    SprProcInfo();
    uint64_t GetTickUs();

private:
    uint64_t mBootTimeUs;
    std::string mDebugPath;
};

#endif // __SPR_PROC_INFO_H__
