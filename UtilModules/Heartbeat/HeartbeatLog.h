/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : HeartbeatLog.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Injectable logging interface for Heartbeat.
 *  @date       : 2026/09/17
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __HEARTBEAT_LOG_H__
#define __HEARTBEAT_LOG_H__

#include <atomic>
#include <functional>
#include <stdarg.h>
#include <stdio.h>

#define HBLOGD(fmt, args...) HeartbeatLog::GetInstance().Print(HB_LOG_LEVEL_DEBUG, __LINE__, HBLOG_TAG, fmt, ##args)
#define HBLOGI(fmt, args...) HeartbeatLog::GetInstance().Print(HB_LOG_LEVEL_INFO,  __LINE__, HBLOG_TAG, fmt, ##args)
#define HBLOGW(fmt, args...) HeartbeatLog::GetInstance().Print(HB_LOG_LEVEL_WARN,  __LINE__, HBLOG_TAG, fmt, ##args)
#define HBLOGE(fmt, args...) HeartbeatLog::GetInstance().Print(HB_LOG_LEVEL_ERROR, __LINE__, HBLOG_TAG, fmt, ##args)

enum HeartbeatLogLevel {
	HB_LOG_LEVEL_DEBUG = 0,
	HB_LOG_LEVEL_INFO,
	HB_LOG_LEVEL_WARN,
	HB_LOG_LEVEL_ERROR,
	HB_LOG_LEVEL_BUTT
};

using HeartbeatLogCallback = std::function<void(int level, int line, const char* tag, const char* fmt, va_list args)>;

class HeartbeatLog {
public:
	static HeartbeatLog& GetInstance();
	void SetLevel(HeartbeatLogLevel level);
	HeartbeatLogLevel GetLevel();

	void RegisterPrintCallback(const HeartbeatLogCallback& callback);
	void Print(HeartbeatLogLevel level, int line, const char* tag, const char* fmt, ...);

private:
	HeartbeatLog();
	~HeartbeatLog() = default;

private:
	std::atomic<HeartbeatLogLevel> mLevel;
	HeartbeatLogCallback mCallback;
};

#endif // __HEARTBEAT_LOG_H__
