/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : 15_SprLog.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprLog usage examples.
 *  @date       : 2026/09/21
 *
 *  The example adds its own route to sprlog.conf before writing logs.
 *  Restart servicemanagersrv after the first run so LogManagerSrv reloads it.
 *
 *  Usage:
 *   1. Replace the default sprlog.conf with the following content.
 *   2. Run service manager
 *      ./servicemanager
 *   3. Run the example
 *     ./15_sprlog
 *   4. Check the log
 *     tail -f /tmp/sprlog/example/sprlog.log
 *     tail -f /tmp/sprlog/main/main.log
 *     tail -f /tmp/sprlog/example/example.log
 *
 *  sprlog.conf:
 *
 *  [output.default]                        # 默认输出
 *  frame_length_bytes  = 1024              # 单条日志帧最大长度，单位：字节
 *  enabled             = true              # 是否启用日志输出
 *  level               = debug             # 日志级别：error、warn、info、debug
 *  output              = file              # 输出方式：file、stdout
 *  file_name           = main.log          # 日志文件名
 *  file_name_format    = BN.FX             # 日志文件名格式: BN：基础名称，FX：文件扩展名，ST：时间戳，SI: 系统启动短标识，MH: 进程单调时间戳
 *  file_path           = /tmp/sprlog/main  # 日志目录
 *  file_num            = 10                # 保留日志文件数量
 *  file_capacity_mb    = 10                # 单个日志文件最大容量，单位：MB
 *  flush_count         = 64                # 累积日志条数达到该值时刷新
 *  flush_interval_ms   = 1000              # 定时刷新间隔，单位：毫秒
 *  [output.15_sprlog]
 *  file_name = sprlog.log
 *  file_path = /tmp/sprlog/example
 *  level = debug
 *  output = file
 *  [output.SprLogExample]
 *  file_name = example.log
 *  file_name_format = BN_SI_MH_ST.FX
 *  file_path = /tmp/sprlog/example
 *  file_name_format = BN_SI_MH_ST.FX
 *  level = debug
 *  output = file
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include "CommonMacros.h"
#include "SprLog.h"

#define LOG_TAG "LogExample"

// SPR_INIT: use the matching [output.15_sprlog] sink.
void UsageWithCustomLogConfig() {
    printf("1. log config file: /tmp/sprlog/example/sprlog.log\n");
    SPR_LOGI("1. Custom log config");
    SPR_LOGI("log file: /tmp/sprlog/example/sprlog.log");
    SPR_LOGI("info message");
    SPR_LOGD("debug message");
    SPR_LOGW("warning message");
    SPR_LOGE("error message");
}

// SPR_INIT: use not matching sink in sprlog.conf,
// fallback to [output.default].
void UsageWithDefault() {
    // to not match [output.15_sprlog] in sprlog.conf
    SPR_INIT("InvalidModuleName");
    printf("2. log file: /tmp/sprlog/main/main.log\n");

    SPR_LOGI("2. Default usage");
    SPR_LOGI("log file: /tmp/sprlog/main/main.log");
    SPR_LOGI("info message");
    SPR_LOGD("debug message");
    SPR_LOGW("warning message");
    SPR_LOGE("error message");
}

// SPR_INIT: use the matching [output.SprLogExample] sink.
int32_t UsageWithCustomLogInit() {
    SPR_INIT("SprLogExample");
    printf("3. log file: /tmp/sprlog/example/example.log\n");

    SPR_LOGI("3. Custom sprlog module name");
    SPR_LOGI("log file: /tmp/sprlog/example/example.log");
    SPR_LOGI("info message");
    SPR_LOGD("debug message");
    SPR_LOGW("warning message");
    SPR_LOGE("error message");
    return 0;
}

int main() {
    // 1. Custom log config
    // log config route [output.15_sprlog] in sprlog.conf
    // [output.15_sprlog]: log file in /tmp/sprlog/example/sprlog.log
    UsageWithCustomLogConfig();

    // 2. Default usage
    // log config route [output.default] in sprlog.conf
    // [output.default]: log file in /tmp/sprlog/main/main.log
    UsageWithDefault();

    // 3. Custom sprlog module name
    // log config route [output.SprLogExample] in sprlog.conf
    // [output.SprLogExample]: log file in /tmp/sprlog/example/example.log
    UsageWithCustomLogInit();

    return 0;
}
