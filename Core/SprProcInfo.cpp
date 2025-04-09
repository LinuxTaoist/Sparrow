/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprProcInfo.cpp
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
#include <atomic>
#include <unistd.h>
#include "CoreTypeDefs.h"
#include "SprProcInfo.h"

static std::atomic<bool> gObjAlive(true);

SprProcInfo::SprProcInfo() : mEnable(false), mBootTimeUs(0)
{
}

SprProcInfo::~SprProcInfo()
{
    gObjAlive = false;
}

SprProcInfo* SprProcInfo::GetInstance()
{
    if (!gObjAlive) {
        return nullptr;
    }

    static SprProcInfo instance;
    return &instance;
}

void SprProcInfo::Init()
{
    mEnable = true;
    LoadBootTimeUs();
}

uint64_t SprProcInfo::GetTickUs()
{
    uint32_t td = 0;
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    td = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;

    return td;
}

void SprProcInfo::LoadBootTimeUs()
{
    mBootTimeUs = GetTickUs();
}

uint64_t SprProcInfo::GetBootTimeUs()
{
    return mBootTimeUs;
}

uint64_t SprProcInfo::GetRunTimeUs()
{
    return GetTickUs() - mBootTimeUs;
}

std::string SprProcInfo::UsToTimeString(uint64_t us)
{
    const uint64_t hourPerDay = 24;
    const uint64_t minPerHour = 60;
    const uint64_t secPerMin = 60;
    const uint64_t usPerSec = 1000000;

    uint64_t days = us / (hourPerDay * minPerHour * secPerMin * usPerSec);
    uint64_t hours = (us % (hourPerDay * minPerHour * secPerMin * usPerSec)) / (minPerHour * secPerMin * usPerSec);
    uint64_t mins  = (us % (minPerHour * secPerMin * usPerSec)) / (secPerMin * usPerSec);
    uint64_t secs  = (us % (secPerMin * usPerSec)) / usPerSec;
    uint64_t usecs = us % usPerSec;

    std::string timeText;
    if (days > 0) {
        timeText = std::to_string(days) + "d ";
    }
    if (hours > 0 || days > 0) {
        timeText += std::to_string(hours) + "h ";
    }
    if (mins > 0 || hours > 0 || days > 0) {
        timeText += std::to_string(mins) + "m ";
    }

    timeText += std::to_string(secs) + "." + std::to_string(usecs) + "s";
    return timeText;
}

std::string SprProcInfo::GetBootTimeString()
{
    return UsToTimeString(mBootTimeUs);
}

std::string SprProcInfo::GetRunTimeString()
{
    return UsToTimeString(GetRunTimeUs());
}

std::string SprProcInfo::GetProcName()
{
    char buf[512] = {0};
    std::string procName = "Unknown";

    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len != -1) {
        std::string fullPath(buf);
        size_t pos = fullPath.find_last_of('/');
        if (pos != std::string::npos) {
            procName = fullPath.substr(pos + 1);
        } else {
            procName = fullPath;
        }
    }

    return procName;
}

std::string SprProcInfo::GetDebugPath()
{
    return mEnable ? std::string(DEBUG_NODE_ROOT_PATH) + "/" + GetProcName() : "";
}
