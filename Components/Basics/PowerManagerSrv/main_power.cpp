/**
 *******************************************************************************
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file main_power.cpp
 *  @author Xiang.D (dx_65535@163.com)
 *  @version 1.0
 *  @brief Blog: https://linuxtaoist.gitee.io
 *  @date 2023-11-22
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/12/21 |  1.0.0.1  | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 *******************************************************************************
 */
#include <stdio.h>
#include <memory>
#include "PowerManager.h"
#include "PowerManagerHub.h"
#include "SprMediatorIpcProxy.h"

using namespace std;
using namespace InternalDefs;

int main(int argc, const char *argv[])
{
    PowerManager thePowerManager(MODULE_POWERM, "PowerM", make_shared<SprMediatorIpcProxy>());
    PowerManagerHub thePowerManagerHub("powermanagersrv", &thePowerManager);

    thePowerManagerHub.InitializeHub();
    SprObserver::MainLoop();
    return 0;
}