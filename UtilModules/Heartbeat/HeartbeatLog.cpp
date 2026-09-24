/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : HeartbeatLog.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Injectable logging implementation for Heartbeat.
 *  @date       : 2026/09/17
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <string>
#include "HeartbeatLog.h"

HeartbeatLog& HeartbeatLog::GetInstance() {
	static HeartbeatLog instance;
	return instance;
}

HeartbeatLog::HeartbeatLog() : mLevel(HB_LOG_LEVEL_INFO) {

}

void HeartbeatLog::SetLevel(HeartbeatLogLevel level) {
	mLevel = level;
}

HeartbeatLogLevel HeartbeatLog::GetLevel() {
	return mLevel;
}

void HeartbeatLog::RegisterPrintCallback(const HeartbeatLogCallback& callback) {
	mCallback = callback;
}

void HeartbeatLog::Print(HeartbeatLogLevel level, int line, const char* tag,
						 const char* fmt, ...) {
	if (level < mLevel) {
		return;
	}

	std::string levelStr;
	switch (level) {
		case HB_LOG_LEVEL_DEBUG:
			levelStr = "D";
			break;
		case HB_LOG_LEVEL_INFO:
			levelStr = "I";
			break;
		case HB_LOG_LEVEL_WARN:
			levelStr = "W";
			break;
		case HB_LOG_LEVEL_ERROR:
			levelStr = "E";
			break;
		default:
			levelStr = "U";
			break;
	}

	va_list ap;
	va_start(ap, fmt);
	if (mCallback) {
		mCallback((int)level, line, tag, fmt, ap);
		va_end(ap);
		return;
	}

	printf("%4d %s %s: ", line, tag, levelStr.c_str());
	vprintf(fmt, ap);
	va_end(ap);
}
