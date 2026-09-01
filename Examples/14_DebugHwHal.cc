/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : 14_DebugHwHal.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.1
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/07/29
 *
 *  HAL interface debug / verification example.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *  2026/07/29 | 1.1.0.0   | Xiang.D        | Use static HwInterfaceHub API
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include "HwInterfaceHub.h"
#include "IHwPower.h"
#include "IHwGpio.h"
#include "IHwUart.h"
#include "IHwWatchdog.h"
#include "IHwRtc.h"
#include "IHwNetwork.h"

using namespace std;

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)

void TestPower()
{
    SPR_LOG("--------------------------------------------------\n");
    SPR_LOG("  [ Power ]\n");

    IHwPower* pwr = HwInterfaceHub::GetPower();
    if (!pwr) {
        SPR_LOG("  not registered\n");
        return;
    }

    string platform = pwr->GetPlatformName();
    string version  = pwr->GetVersion();
    SPR_LOG("  platform : %s\n", platform.c_str());
    SPR_LOG("  version  : %s\n", version.c_str());

    EHwPowerMode mode;
    int32_t r = pwr->GetMode(mode);
    if (r == HW_OK) {
        SPR_LOG("  GetMode  : %d\n", (int)mode);
    } else {
        SPR_LOG("  GetMode  : ERR(%d)\n", r);
    }

    EHwWakeupSource src;
    r = pwr->GetWakeupSource(src);
    if (r == HW_OK) {
        SPR_LOG("  WakeSrc  : %d\n", (int)src);
    } else {
        SPR_LOG("  WakeSrc  : ERR(%d)\n", r);
    }

    SPR_LOG("  Init     : %s\n", pwr->Init()   == HW_OK ? "OK" : "ERR");
    SPR_LOG("  Deinit   : %s\n", pwr->Deinit() == HW_OK ? "OK" : "ERR");
}

void TestGpio()
{
    SPR_LOG("--------------------------------------------------\n");
    SPR_LOG("  [ GPIO ]\n");

    IHwGpio* gpio = HwInterfaceHub::GetGpio();
    if (!gpio) {
        SPR_LOG("  not registered\n");
        return;
    }

    string version = gpio->GetVersion();
    SPR_LOG("  version  : %s\n", version.c_str());

    vector<SHwGpioPinConfig> cfgs;
    cfgs.push_back({12, HW_GPIO_OUTPUT, HW_GPIO_PULL_NONE, HW_GPIO_EDGE_NONE, "LED0"});
    cfgs.push_back({15, HW_GPIO_INPUT,  HW_GPIO_PULL_UP,   HW_GPIO_EDGE_BOTH, "KEY0"});

    int32_t r = gpio->Init(cfgs);
    SPR_LOG("  Init(%zu) : %s\n", cfgs.size(), r == HW_OK ? "OK" : "ERR");

    EHwGpioValue val;
    r = gpio->Read(15, val);
    if (r == HW_OK) {
        SPR_LOG("  Read(15)  : %d\n", (int)val);
    } else {
        SPR_LOG("  Read(15)  : ERR(%d)\n", r);
    }

    SPR_LOG("  Write(12,1): %s\n", gpio->Write(12, HW_GPIO_HIGH) == HW_OK ? "OK" : "ERR");
    SPR_LOG("  Write(12,0): %s\n", gpio->Write(12, HW_GPIO_LOW)  == HW_OK ? "OK" : "ERR");
    SPR_LOG("  Release   : %s\n", gpio->Release()                == HW_OK ? "OK" : "ERR");
}

void TestUart()
{
    SPR_LOG("--------------------------------------------------\n");
    SPR_LOG("  [ UART ]\n");

    IHwUart* uart = HwInterfaceHub::GetUart();
    if (!uart) {
        SPR_LOG("  not registered\n");
        return;
    }

    string version = uart->GetVersion();
    SPR_LOG("  version  : %s\n", version.c_str());

    SHwUartConfig cfg;
    cfg.baud      = HW_UART_BAUD_115200;
    cfg.dataBits  = HW_UART_DATA_8;
    cfg.parity    = HW_UART_PARITY_NONE;
    cfg.stopBits  = HW_UART_STOP_1;
    cfg.flowCtrl  = HW_UART_FLOW_NONE;
    cfg.vmin      = 1;
    cfg.vtime     = 10;

    SPR_LOG("  Open     : %s\n", uart->Open("/dev/ttyS0", cfg) == HW_OK ? "OK" : "ERR");
    SPR_LOG("  IsOpen   : %s\n", uart->IsOpen() ? "true" : "false");
    SPR_LOG("  Send     : %s\n", uart->Send((const uint8_t*)"AT\r\n", 4) == HW_OK ? "OK" : "ERR");
    SPR_LOG("  Flush    : %s\n", uart->Flush() == HW_OK ? "OK" : "ERR");
    SPR_LOG("  Close    : %s\n", uart->Close() == HW_OK ? "OK" : "ERR");
}

void TestWatchdog()
{
    SPR_LOG("--------------------------------------------------\n");
    SPR_LOG("  [ Watchdog ]\n");

    IHwWatchdog* wdt = HwInterfaceHub::GetWatchdog();
    if (!wdt) {
        SPR_LOG("  not registered\n");
        return;
    }

    string version = wdt->GetVersion();
    SPR_LOG("  version  : %s\n", version.c_str());

    SPR_LOG("  Start(30): %s\n", wdt->Start(30) == HW_OK ? "OK" : "ERR");
    SPR_LOG("  Feed     : %s\n", wdt->Feed()     == HW_OK ? "OK" : "ERR");
    SPR_LOG("  Feed     : %s\n", wdt->Feed()     == HW_OK ? "OK" : "ERR");
    SPR_LOG("  Stop     : %s\n", wdt->Stop()     == HW_OK ? "OK" : "ERR");
}

void TestRtc()
{
    SPR_LOG("--------------------------------------------------\n");
    SPR_LOG("  [ RTC ]\n");

    IHwRtc* rtc = HwInterfaceHub::GetRtc();
    if (!rtc) {
        SPR_LOG("  not registered\n");
        return;
    }

    string version = rtc->GetVersion();
    SPR_LOG("  version  : %s\n", version.c_str());

    int64_t now;
    int32_t r = rtc->GetTime(now);
    if (r == HW_OK) {
        SPR_LOG("  GetTime  : %lld\n", (long long)now);
    } else {
        SPR_LOG("  GetTime  : ERR(%d)\n", r);
    }

    SPR_LOG("  RegisterAlarmCallback : %s\n", rtc->RegisterAlarmCallback(now + 3600, {}) == HW_OK ? "OK" : "ERR");
    SPR_LOG("  Cancel   : %s\n", rtc->CancelAlarm()             == HW_OK ? "OK" : "ERR");
}

void TestNetwork()
{
    SPR_LOG("--------------------------------------------------\n");
    SPR_LOG("  [ Network ]\n");

    IHwNetwork* net = HwInterfaceHub::GetNetwork();
    if (!net) {
        SPR_LOG("  not registered\n");
        return;
    }

    string version = net->GetVersion();
    SPR_LOG("  version  : %s\n", version.c_str());

    EHwNetRegState state;
    int32_t r = net->GetRegistrationState(state);
    if (r == HW_OK) {
        SPR_LOG("  RegState : %d\n", (int)state);
    } else {
        SPR_LOG("  RegState : ERR(%d)\n", r);
    }

    SHwSignalStrength sig;
    r = net->GetSignalStrength(sig);
    if (r == HW_OK) {
        SPR_LOG("  RSSI/RSRP: %d / %d\n", sig.rssi, sig.rsrp);
    } else {
        SPR_LOG("  Signal   : ERR(%d)\n", r);
    }
}

void ShowVersions()
{
    SPR_LOG("--------------------------------------------------\n");
    SPR_LOG("  Versions (all registered interfaces)\n");
    SPR_LOG("--------------------------------------------------\n");

    struct {
        const char* name;
        bool        reg;
        string      ver;
    } items[6];

    auto* pwr = HwInterfaceHub::GetPower();
    items[0] = {"Power   ", !!pwr, pwr ? pwr->GetVersion() : "-"};

    auto* gpio = HwInterfaceHub::GetGpio();
    items[1] = {"GPIO    ", !!gpio, gpio ? gpio->GetVersion() : "-"};

    auto* uart = HwInterfaceHub::GetUart();
    items[2] = {"UART    ", !!uart, uart ? uart->GetVersion() : "-"};

    auto* wdt = HwInterfaceHub::GetWatchdog();
    items[3] = {"Watchdog", !!wdt, wdt ? wdt->GetVersion() : "-"};

    auto* rtc = HwInterfaceHub::GetRtc();
    items[4] = {"RTC     ", !!rtc, rtc ? rtc->GetVersion() : "-"};

    auto* net = HwInterfaceHub::GetNetwork();
    items[5] = {"Network ", !!net, net ? net->GetVersion() : "-"};

    for (auto& it : items) {
        SPR_LOG("  %-10s %-16s v%s\n", it.name, it.reg ? "registered" : "NOT REGISTERED", it.ver.c_str());
    }
}

void usage()
{
    SPR_LOG("------------------------------------------------------------------\n"
            "1: Power\n"
            "2: GPIO\n"
            "3: UART\n"
            "4: Watchdog\n"
            "5: RTC\n"
            "6: Network\n"
            "V: Versions\n"
            "0: Quit\n"
            "------------------------------------------------------------------\n"
    );
}

int main(int argc, const char* argv[])
{
    printf("\n  === Sparrow HAL Debug Tool ===\n\n");
    usage();

    char val = 0;
    bool run = true;
    do {
        printf("Input: ");
        cin >> val;
        switch (val) {
            case '1': {
                TestPower();
                break;
            }
            case '2': {
                TestGpio();
                break;
            }
            case '3': {
                TestUart();
                break;
            }
            case '4': {
                TestWatchdog();
                break;
            }
            case '5': {
                TestRtc();
                break;
            }
            case '6': {
                TestNetwork();
                break;
            }
            case 'V':
            case 'v': {
                ShowVersions();
                break;
            }
            case '0': {
                run = false;
                break;
            }
            default: {
                usage();
                break;
            }
        }
    } while (run);

    printf("  Bye.\n\n");
    return 0;
}
