#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

void MYLOG(const char* type, const char* function, const char* format, ...) {
	SYSTEMTIME st;
	GetLocalTime(&st);
	va_list args;
	printf("%lu::%s::%.4u-%.2u-%.2u %.2u:%.2u:%.2u,%.3u::%s::",
		GetCurrentThreadId(), type,
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond,
		st.wMilliseconds,
		function);
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

typedef enum {
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL
} VDLogLevel;

#define LOG(type, format, ...) do {                                     \
        MYLOG(#type, __FUNCTION__, format "\n", ## __VA_ARGS__);  \
} while(0)

#define LOG_INFO(format, ...) LOG(INFO, format, ## __VA_ARGS__)
#define LOG_WARN(format, ...) LOG(WARN, format, ## __VA_ARGS__)
#define LOG_ERROR(format, ...) LOG(ERROR, format, ## __VA_ARGS__)