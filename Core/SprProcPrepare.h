/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprProcPrepare.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/04/06
 *
 *  Defines the SprProcPrepare class, which is a singleton class used to initialize
 *  the system framework functions for each process in the Sparrow microservice framework.
 *  By calling the Init method of this class, some common functions of the system framework can be loaded,
 *  ensuring that each process has the necessary runtime environment and basic functions when starting.
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/04/06 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_PROC_PREPARE_H__
#define __SPR_PROC_PREPARE_H__

#include <stdint.h>

class SprProcPrepare
{
public:
    static SprProcPrepare* GetInstance();
    int32_t Init(const std::string& procName);

private:
    SprProcPrepare();
    ~SprProcPrepare();

private:
    std::string mProcName;
};

#endif // __SPR_PROC_PREPARE_H__
