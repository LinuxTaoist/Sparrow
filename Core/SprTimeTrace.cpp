/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimeTrace.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/02/24
 *
 *  System initialization file
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/02/24 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <atomic>
#include "SprTimeTrace.h"

static std::atomic<bool> gObjAlive(true);

SprTimeTrace::SprTimeTrace()
{
}

SprTimeTrace::~SprTimeTrace()
{
    gObjAlive = false;
}

SprTimeTrace* SprTimeTrace::GetInstance()
{
    if (!gObjAlive) {
        return nullptr;
    }

    static SprTimeTrace instance;
    return &instance;
}

void SprTimeTrace::TimeTracePoint(int id, const std::string& text)
{
    EntryInfo entry;
    entry.eId = id;
    entry.eText = text;
    clock_gettime(CLOCK_MONOTONIC, &entry.eTimeStamp);
    mEntryList.push_back(entry);
}

