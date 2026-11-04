/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : main_log.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/11/25
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <thread>
#include "LogManager.h"
#include "SprMainInterface.h"

SPR_ENTRY(
    LogManager theLogManager;
    theLogManager.Init();

    std::thread logThread([&theLogManager]() {
        theLogManager.MainLoop();
    });

    logThread.detach();
);
