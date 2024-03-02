/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprLog.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/03/02
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/02 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdarg.h>
#include "SprLog.h"

// #define GOOGLE_GLOG_DLL_DECL
// #include "glog/logging.h"

// SprLog::SprLog() {
//     google::InitGoogleLogging("MyProgram");
// }

// SprLog::~SprLog() {
//     google::ShutdownGoogleLogging();
// }

// SprLog* SprLog::GetInstance() {
//     static SprLog instance;
//     return &instance;
// }

// int32_t SprLog::Debug(const char* format, ...)
// {
//     va_list args;
//     va_start(args, format);
//     char buffer[4096];
//     vsnprintf(buffer, sizeof(buffer), format, args);
//     va_end(args);

//     LOG(INFO) << buffer;

//     return 0;
// }

// int32_t SprLog::Info(const char* format, ...)
// {
//     va_list args;
//     va_start(args, format);
//     char buffer[4096];
//     vsnprintf(buffer, sizeof(buffer), format, args);
//     va_end(args);

//     LOG(INFO) << buffer;

//     return 0;
// }

// int32_t SprLog::Warn(const char* format, ...)
// {
//     va_list args;
//     va_start(args, format);
//     char buffer[4096];
//     vsnprintf(buffer, sizeof(buffer), format, args);
//     va_end(args);

//     LOG(WARNING) << buffer;

//     return 0;
// }

// int32_t SprLog::Error(const char* format, ...)
// {
//     va_list args;
//     va_start(args, format);
//     char buffer[4096];
//     vsnprintf(buffer, sizeof(buffer), format, args);
//     va_end(args);

//     LOG(ERROR) << buffer;

//     return 0;
// }
