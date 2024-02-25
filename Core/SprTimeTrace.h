/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimeTrace.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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

#ifndef __SPR_TIME_TRACE_H__
#define __SPR_TIME_TRACE_H__

#include <vector>
#include <string>
#include <time.h>

typedef struct
{
    int		    eId;
    timespec    eTimeStamp;
    std::string eText;
} EntryInfo;

class SprTimeTrace
{
private:
    std::vector<EntryInfo> mEntryList;

public:
    ~SprTimeTrace();
    static SprTimeTrace* GetInstance();
    void TimeTracePoint(int id, const std::string& text);

private:
    SprTimeTrace();
};

#endif // __SPR_TIME_TRACE_H__