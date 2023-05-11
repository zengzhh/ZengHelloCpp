//------------------------
// Author: zengzhh
// Version: 2.0
// Date: 2023-05-11
//------------------------

#include "easylog.h"
#include <share.h>
#include <strsafe.h>
#include <Windows.h>

#define LOG_LINE_COUNT (1000)
#define LOG_ROLL_SIZE (5 * 1024 * 1024)

EasyLog* EasyLog::_log = NULL;
std::mutex _mutex;

EasyLog::EasyLog(FILE* handle, TCHAR* path)
{
	_handle = handle;
	_path = path;
	_log = this;
	_count = 0;
}

EasyLog::~EasyLog()
{
	if (_log && _handle && _path) {
		fclose(_handle);
		delete _path;
		_log = NULL;
	}
}

void EasyLog::init(const TCHAR* path)
{
	if (_log) {
		return;
	}
	if (path == NULL || lstrlen(path) == 0) {
		return;
	}
	FILE* handle = _tfsopen(path, _T("at"), _SH_DENYNO);
	if (!handle) {
		return;
	}
	int len = lstrlen(path) + 1;
	TCHAR* tpath = new TCHAR[len];
	StringCchCopy(tpath, len, path);
	_log = new EasyLog(handle, tpath);
}

void EasyLog::logf(const char* type, const char* function, const char* format, ...) {
	FILE* fh = _log ? _log->_handle : stdout;

	SYSTEMTIME st;
	GetLocalTime(&st);

	std::lock_guard<std::mutex> lk(_mutex);

	fprintf(fh, "%lu::%s::%.4u-%.2u-%.2u %.2u:%.2u:%.2u,%.3u::%s::",
		GetCurrentThreadId(), type,
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond,
		st.wMilliseconds,
		function);
	va_list args;
	va_start(args, format);
	vfprintf(fh, format, args);
	va_end(args);

	fflush(fh);

	if (_log && (++_log->_count > LOG_LINE_COUNT) && _log->_handle && _log->_path)
	{
		_log->_count = 0;
		fseek(fh, 0L, SEEK_END);
		long size = ftell(fh);
		if (size > LOG_ROLL_SIZE)
		{
			fclose(fh);
			_log->_handle = rollback(_log->_path);
		}
	}
}

void EasyLog::printf(const char* format, ...)
{
	FILE* fh = _log ? _log->_handle : stdout;

	std::lock_guard<std::mutex> lk(_mutex);

	va_list args;
	va_start(args, format);
	vfprintf(fh, format, args);
	va_end(args);

	if (_log && _log->_handle && _log->_path)
	{
		fseek(fh, 0L, SEEK_END);
		long size = ftell(fh);
		if (size > LOG_ROLL_SIZE)
		{
			fclose(fh);
			_log->_handle = rollback(_log->_path);
		}
	}
}

FILE* EasyLog::rollback(const TCHAR* path) {
	TCHAR roll_path[MAX_PATH];
	_stprintf_s(roll_path, MAX_PATH, _T("%s.1"), path);
	MoveFileEx(path, roll_path, MOVEFILE_REPLACE_EXISTING);
	FILE* file = _tfsopen(path, _T("at"), _SH_DENYNO);
	return file;
}