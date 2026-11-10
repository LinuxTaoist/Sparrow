/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprSystem.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/02/24
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <atomic>
#include <fstream>
#include "SprLog.h"
#include "SprContext.h"
#include "CommonMacros.h"
#include "SprSystem.h"

using namespace std;

#define LOG_TAG "SprSystem"

static std::atomic<bool> gObjAlive(true);

SprSystem::SprSystem() {
    mVersionPath = std::string(DEFAULT_DEBUG_ROOT_DIR) + "/" + "sparrow_version";
}

SprSystem::~SprSystem() {
    gObjAlive = false;
}

SprSystem* SprSystem::GetInstance() {
    if (gObjAlive == false) {
        return nullptr;
    }

    static SprSystem instance;
    return &instance;
}

void SprSystem::InitEnv() {
    // write release information
    LoadReleaseInformation();
}

void SprSystem::LoadReleaseInformation() {
    std::string projectInfo     = PROJECT_INFO;
    std::string cxxStandard     = CXX_STANDARD;
    std::string gxxStandard     = GXX_VERSION;
    std::string gccVersion      = GCC_VERSION;
    std::string runPlatform     = PROJECT_PLATFORM;
    std::string buildTime       = BUILD_TIME;
    std::string buildType       = BUILD_TYPE;
    std::string buildHost       = BUILD_HOST;
    std::string buildPlatform   = BUILD_PLATFORM;
    std::string moduleConfig    = MODULE_CONFIG_VERSION;
    std::string halFamily       = HAL_FAMILY;

    std::string releaseInfo;
    releaseInfo += "Project        : " + projectInfo + "\n";
    releaseInfo += "C++ Standard   : " + cxxStandard + "\n";
    releaseInfo += "G++ Version    : " + gxxStandard + "\n";
    releaseInfo += "Gcc Version    : " + gccVersion + "\n";
    releaseInfo += "Run Platform   : " + runPlatform + "\n";
    releaseInfo += "HAL Family     : " + halFamily + "\n";
    releaseInfo += "Build Time     : " + buildTime + "\n";
    releaseInfo += "Build Type     : " + buildType + "\n";
    releaseInfo += "Build Host     : " + buildHost + "\n";
    releaseInfo += "Build Platform : " + buildPlatform + "\n";
    releaseInfo += "Module Config  : " + moduleConfig + "\n";

    std::ofstream file(mVersionPath);
    if (file) {
        SPR_LOGD("Load system version information to %s\n", mVersionPath.c_str());
        file << releaseInfo;
        file.close();
    } else {
        SPR_LOGE("Open %s fail!\n", mVersionPath.c_str());
    }
}

void SprSystem::Init() {
    SPR_LOGD("=============================================\n");
    SPR_LOGD("=          Sprrow System Init               =\n");
    SPR_LOGD("=============================================\n");

    InitEnv();

    SprContext ctx;
    mPluginMgr.Init();
}
