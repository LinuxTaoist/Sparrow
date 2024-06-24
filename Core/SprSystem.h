/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprSystem.h
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
#ifndef __SPR_SYSTEM_H__
#define __SPR_SYSTEM_H__

#include <string>

#ifndef PROJECT_INFO
    #define PROJECT_INFO    "Unknown"
#endif

#ifndef CXX_STANDARD
    #define CXX_STANDARD    "Unknown"
#endif

#ifndef GXX_VERSION
    #define GXX_VERSION     "Unknown"
#endif

#ifndef GCC_VERSION
    #define GCC_VERSION     "Unknown"
#endif

#ifndef RUN_ENV
    #define RUN_ENV         "Unknown"
#endif

#ifndef BUILD_TIME
    #define BUILD_TIME      "Unknown"
#endif

#ifndef BUILD_TYPE
    #define BUILD_TYPE      "Unknown"
#endif

#ifndef BUILD_HOST
    #define BUILD_HOST      "Unknown"
#endif

#ifndef BUILD_PLATFORM
    #define BUILD_PLATFORM  "Unknown"
#endif

class SprSystem
{
public:
    // --------------------------------------------------------------------------------------------
    // - Init functions
    // --------------------------------------------------------------------------------------------
    ~SprSystem();
    SprSystem(const SprSystem&) = delete;
    SprSystem& operator=(const SprSystem&) = delete;
    void Init();

    // --------------------------------------------------------------------------------------------
    // - Single instance
    // --------------------------------------------------------------------------------------------
    static SprSystem* GetInstance();

private:
    SprSystem();
    void InitEnv();
    void InitMsgQueueLimit();
    void LoadReleaseInformation();
    int EnvReady(const std::string& srvName);
};

#endif // __SPR_SYSTEM_H__
