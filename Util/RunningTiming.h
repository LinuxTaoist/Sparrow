/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : RunningTiming.h
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
 *      double totalTimeInSeconds = timer.GetTotalTimeInSec();
 *  4. Get the total elapsed time in milliseconds:
 *      double totalTimeInMilliseconds = timer.GetTotalTimeInMSec();
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
#ifndef __RUNNING_TIMING_H__
#define __RUNNING_TIMING_H__

class RunningTiming
{
public:
    /**
     * @brief Constructor. Initializes the timer and automatically records the start time.
     */
    RunningTiming();

    /**
     * @brief Destructor.
     */
    ~RunningTiming();

    /**
     * @brief Get the total time in seconds.
     *
     * @return The total time in seconds.
     */
    int GetTotalTimeInSec();

    /**
     * @brief Get the total time in milliseconds.
     *
     * @return The total time in milliseconds.
     */
    int GetTotalTimeInMSec();

private:
    /**
     * @brief Start the timer and record the start time.
     */
    void Start();

    /**
     * @brief Stop the timer and record the end time.
     */
    void Stop();

private:
    int mStartTimeInMSec;
    int mStopTimeInMSec;
};

#endif // __RUNNING_TIMING_H__
