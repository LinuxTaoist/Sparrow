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
    static SprProcInfo* GetInstance();

    void Init();
    uint64_t GetBootTimeUs();
    uint64_t GetRunTimeUs();
    std::string GetBootTimeString();
    std::string GetRunTimeString();
    std::string GetProcName();
    std::string GetDebugPath();
    std::string GetRunRootPath();
    std::string GetRunEtcPath();

private:
    SprProcInfo();
    ~SprProcInfo();
    void LoadBootTimeUs();
    uint64_t GetTickUs();
    std::string UsToTimeString(uint64_t us);

private:
    bool mEnable;
    uint64_t mBootTimeUs;
};

#endif // __SPR_PROC_INFO_H__
