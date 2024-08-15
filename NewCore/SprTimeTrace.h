/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimeTrace.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/02/24
 *
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
public:
    /**
     * @brief Destroy the Spr Time Trace object
     *
     */
    ~SprTimeTrace();

    /**
     * @brief Get the Instance object
     *
     * @return SprTimeTrace*
     */
    static SprTimeTrace* GetInstance();

    /**
     * @brief
     *
     * @param id    Entry id
     * @param text  Entry description
     *
     * Call it in the system main entry, used to cache information for each entry
     */
    void TimeTracePoint(int id, const std::string& text);

private:
    /**
     * @brief Construct a new Spr Time Trace object
     *
     */
    SprTimeTrace();

private:
    std::vector<EntryInfo> mEntryList;
};

#endif // __SPR_TIME_TRACE_H__