/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : RunningTiming.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/09/13
 *
 *  RunningTiming is a class designed to measure the execution time of code blocks.
 *  It adheres to the RTII (Reactive, Testable, Isolated, Interoperable) principle,
 *  providing a straightforward interface to measure the execution time of any code block.
 *
 *  Declaration and Initialization:
 *  1. Declare a RunningTiming object where needed:
 *      RunningTiming timer;
 *  3. Get the total elapsed time in seconds:
 *      double totalTimeInSeconds = timer.GetElapsedTimeInSec();
 *  4. Get the total elapsed time in milliseconds:
 *      double totalTimeInMilliseconds = timer.GetElapsedTimeInMSec();
 *
 *  Usage:
 *  To measure the execution time of a code block, you need to call the Start method
 *  before the code block and the Stop method after the code block. Then, use the GetTimeInSec
 *  or GetTimeInMSec methods to retrieve the duration of the interval.
 *
 *  Example:
 *      RunningTiming timer;
 *      // Execute some code...
 *      double timeInSeconds = timer.GetTimeInSec();            // total time in seconds
 *      std::cout << "Code block took " << timeInSeconds << " seconds." << std::endl;
 *      // double timeInMilliseconds = timer.GetTimeInMSec();      // total time in milliseconds
 *      // std::cout << "Code block took " << timeInMilliseconds << " milliseconds." << std::endl;
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/09/13 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <time.h>
#include <sys/time.h>
#include "RunningTiming.h"


RunningTiming::RunningTiming() : mStartTimeInMSec(0), mStopTimeInMSec(0)
{
    Start();
}

RunningTiming::~RunningTiming()
{
    Stop();
}

void RunningTiming::Start()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    mStartTimeInMSec = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void RunningTiming::Stop()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    mStopTimeInMSec = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int RunningTiming::GetElapsedTimeInSec()
{
    Stop();
    return (mStopTimeInMSec - mStartTimeInMSec) / 1000;
}

int RunningTiming::GetElapsedTimeInMSec() {
    Stop();
    return mStopTimeInMSec - mStartTimeInMSec;
}
