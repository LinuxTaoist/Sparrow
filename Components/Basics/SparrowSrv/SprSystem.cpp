/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprSystem.cpp
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
#include <memory>
#include <fstream>
#include <sys/resource.h>
#include "SprLog.h"
#include "SprContext.h"
#include "CommonMacros.h"
#include "SprSystem.h"
#include "SprTimeTrace.h"
#include "CoreTypeDefs.h"
#include "TimeManager.h"
#include "SprDebugNode.h"
#include "SprSystemTimer.h"
#include "SprTimerManager.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("SprSystem", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("SprSystem", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("SprSystem", fmt, ##args)

#define TTP(ID, TEXT) SprTimeTrace::GetInstance()->TimeTracePoint(ID, TEXT)
#define LOCAL_PATH_VERSION  "/tmp/sparrow_version"

SprSystem::SprSystem()
{
}

SprSystem::~SprSystem()
{
}

SprSystem* SprSystem::GetInstance()
{
    static SprSystem instance;
    return &instance;
}

void SprSystem::InitEnv()
{
    // Init msg queue limit
    InitMsgQueueLimit();

    // write release information
    LoadReleaseInformation();
}

void SprSystem::InitMsgQueueLimit()
{
    // The limit for creating message queues has to be changed, otherwise the other
    // applications can not create enough message queues.
    // Note: The values in /proc/sys/fs/mqueue/* seem to have no influence on this issue.
    // Also ulimit -n has no influence on this issue.
    struct rlimit rlim = {RLIM_INFINITY, RLIM_INFINITY};
    int ret = getrlimit(RLIMIT_MSGQUEUE, &rlim);
    if (ret == 0) {
        rlim.rlim_cur = RLIM_INFINITY;  // soft limit
        rlim.rlim_max = RLIM_INFINITY;  // hard limit
        setrlimit(RLIMIT_MSGQUEUE, &rlim);
    }
}

void SprSystem::LoadReleaseInformation()
{
    std::string projectInfo     = PROJECT_INFO;
    std::string cxxStandard     = CXX_STANDARD;
    std::string gxxStandard     = GXX_VERSION;
    std::string gccVersion      = GCC_VERSION;
    std::string runEnv          = RUN_ENV;
    std::string buildTime       = BUILD_TIME;
    std::string buildType       = BUILD_TYPE;
    std::string buildHost       = BUILD_HOST;
    std::string buildPlatform   = BUILD_PLATFORM;
    std::string moduleConfig    = MODULE_CONFIG_VERSION;

    std::string releaseInfo;
    releaseInfo += "System Version : " + projectInfo + "\n";
    releaseInfo += "C++ Standard   : " + cxxStandard + "\n";
    releaseInfo += "G++ Version    : " + gxxStandard + "\n";
    releaseInfo += "Gcc Version    : " + gccVersion + "\n";
    releaseInfo += "Running Env    : " + runEnv + "\n";
    releaseInfo += "Build Time     : " + buildTime + "\n";
    releaseInfo += "Build Type     : " + buildType + "\n";
    releaseInfo += "Build Host     : " + buildHost + "\n";
    releaseInfo += "Build Platform : " + buildPlatform + "\n";
    releaseInfo += "Module Config  : " + moduleConfig + "\n";

    std::ofstream file(LOCAL_PATH_VERSION);
    if (file) {
        SPR_LOGD("Load system version information to %s\n", LOCAL_PATH_VERSION);
        file << releaseInfo;
        file.close();
    } else {
        SPR_LOGE("Open %s fail!\n", LOCAL_PATH_VERSION);
    }
}

void SprSystem::Init()
{
    SPR_LOGD("=============================================\n");
    SPR_LOGD("=          Sprrow System Init               =\n");
    SPR_LOGD("=============================================\n");

    InitEnv();

    TTP(8, "TimeManager->Initialize()");
    TimeManager::GetInstance(MODULE_TIMEM, "TimeM")->Initialize();

    TTP(9, "pSystemTimer->Initialize()");
    shared_ptr<SprSystemTimer> pSystemTimer = make_shared<SprSystemTimer>(MODULE_SYSTEM_TIMER, "SysTimer");
    pSystemTimer->Initialize();

    TTP(10, "TimerManager->Initialize()");
    SprTimerManager::GetInstance(MODULE_TIMERM, "TimerM", pSystemTimer)->Initialize();

    SprContext ctx;
    mPluginMgr.Init();
    SprDebugNode::GetInstance()->InitPipeDebugNode(string("/tmp/") + SRV_NAME_SPARROW);
}
