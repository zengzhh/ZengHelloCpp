//------------------------
// Author: zengzhh
// Version: 2.0
// Date: 2023-05-11
//------------------------

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <mutex>

class EasyLog
{
public:
	~EasyLog();
	static void init(const TCHAR* path = NULL);
	static void logf(const char* type, const char* function, const char* format, ...);
	static void printf(const char* format, ...);
private:
	EasyLog(FILE* handle, TCHAR* path);
	static FILE* rollback(const TCHAR* path);
	FILE* _handle;
	TCHAR* _path;
	static EasyLog* _log;
	int _count;
};

#define LOG(type, format, ...) do {                                     \
	EasyLog::logf(#type, __FUNCTION__, format "\n", ## __VA_ARGS__);  \
} while(0)

#define LOG_INFO(format, ...) LOG(INFO, format, ## __VA_ARGS__)
#define LOG_WARN(format, ...) LOG(WARN, format, ## __VA_ARGS__)
#define LOG_ERROR(format, ...) LOG(ERROR, format, ## __VA_ARGS__)