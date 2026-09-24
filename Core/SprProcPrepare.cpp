/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprProcPrepare.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/04/06
 *
 *  Defines the SprProcPrepare class, which is a singleton class used to initialize
 *  the system framework functions for each process in the Sparrow microservice framework.
 *  By calling the Init method of this class, some common functions of the system framework can be loaded,
 *  ensuring that each process has the necessary runtime environment and basic functions when starting.
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <atomic>
#include <fstream>
#include <string>
#include <errno.h>
#include <string.h>
#include "SprLog.h"
#include "SprProcInfo.h"
#include "SprDebugNode.h"
#include "CommonMacros.h"
#include "HeartbeatLog.h"
#include "HeartbeatReporter.h"
#include "SprProcPrepare.h"

#define LOG_TAG "SprProcPre"

static std::atomic<bool> gObjAlive(true);

SprProcPrepare::SprProcPrepare() {
}

SprProcPrepare::~SprProcPrepare() {
    gObjAlive = false;
}

SprProcPrepare* SprProcPrepare::GetInstance() {
    if (!gObjAlive) {
        return nullptr;
    }

    static SprProcPrepare instance;
    return &instance;
}

int32_t SprProcPrepare::Init(const std::string& procName) {
    SprProcInfo::GetInstance()->Init();
    SprDebugNode::GetInstance()->InitPipeDebugNode(std::string(DEFAULT_DEBUG_ROOT_DIR) + "/" + procName);
    return InitHeartbeatReporter(procName);
}

int32_t SprProcPrepare::InitHeartbeatReporter(const std::string& procName) {
    HeartbeatLog& theLog = HeartbeatLog::GetInstance();
    theLog.RegisterPrintCallback([](int level, int line, const char* tag, const char* fmt, va_list ap) {
        char logBuf[1024] = {0};
        vsnprintf(logBuf, sizeof(logBuf), fmt, ap);
        switch (level) {
            case HeartbeatLogLevel::HB_LOG_LEVEL_DEBUG:
                SprLog::GetInstance()->d(tag, "%4d %s", line, logBuf);
                break;
            case HeartbeatLogLevel::HB_LOG_LEVEL_INFO:
                SprLog::GetInstance()->i(tag, "%4d %s", line, logBuf);
                break;
            case HeartbeatLogLevel::HB_LOG_LEVEL_ERROR:
                SprLog::GetInstance()->e(tag, "%4d %s", line, logBuf);
                break;
            case HeartbeatLogLevel::HB_LOG_LEVEL_WARN:
                SprLog::GetInstance()->w(tag, "%4d %s", line, logBuf);
                break;
            default:
                SprLog::GetInstance()->i(tag, "%4d %s", line, logBuf);
                break;
        }
    });

    HeartbeatReporter* pReporter = HeartbeatReporter::GetInstance();
    if (!pReporter) {
        SPR_LOGE("pReporter is nullptr!");
        return -1;
    }

    std::string path = "/proc/self/cmdline";
    std::ifstream cmdline(path, std::ios::in | std::ios::binary);
    if (!cmdline.is_open()) {
        SPR_LOGE("Open %s failed! (%s)", path.c_str(), strerror(errno));
        return 0;
    }

    std::string argument;
    while (std::getline(cmdline, argument, '\0')) {
        if (argument == "--heartbeat=on") {
            return pReporter->Start(procName);
        }
    }

    return 0;
}
