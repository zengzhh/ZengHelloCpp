// ReleaseX64.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include "mylog.h"
#include "MySort.h"

#pragma comment(lib, "shlwapi.lib")

#define MINIDUMPPATH "C:\\Windows\\minidump"

using namespace std;

namespace Utility
{
	const std::string UnicodeToAnsi(const std::wstring& src)
	{
		int n = ::WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::vector<char> temp(n);
		::WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, &temp[0], n, nullptr, nullptr);
		return &temp[0];
	}
}

/*
  bDeleteSubdirectories
    If this parameter is TRUE, Subdirectories and RootDirectory will be deleted
	If this parameter is FALSE, only files in the RootDirectory will be deleted
*/
int DeleteDirectory(const std::wstring& refcstrRootDirectory, bool bDeleteSubdirectories = true)
{
	bool bSubdirectory = false;        // whether subdirectories have been found
	HANDLE hFile;                      // Handle to directory
	std::wstring strFilePath;          // Filepath
	std::wstring strPattern;           // Pattern
	WIN32_FIND_DATA FileInformation;   // File information


	strPattern = refcstrRootDirectory + L"\\*.*";
	hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FileInformation.cFileName[0] != '.')
			{
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + L"\\" + FileInformation.cFileName;

				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bDeleteSubdirectories)
					{
						// Delete subdirectory
						int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
						if (iRC)
							return iRC;
					}
					else
						bSubdirectory = true;
				}
				else
				{
					// Set file attributes
					if (::SetFileAttributes(strFilePath.c_str(),
						FILE_ATTRIBUTE_NORMAL) == FALSE)
						return ::GetLastError();

					// Delete file
					if (::DeleteFile(strFilePath.c_str()) == FALSE)
						return ::GetLastError();
				}
			}
		} while (::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
			return dwError;
		else
		{
			if (!bSubdirectory)
			{
				// Set directory attributes
				if (::SetFileAttributes(refcstrRootDirectory.c_str(),
					FILE_ATTRIBUTE_NORMAL) == FALSE)
					return ::GetLastError();

				// Delete directory
				if (::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
					return ::GetLastError();
			}
		}
	}

	return 0;
}

// the time interval in seconds
int DiffSecs(FILETIME ft, SYSTEMTIME lt)
{
	FILETIME ft2;
	SystemTimeToFileTime(&lt, &ft2);

	ULARGE_INTEGER const u1 = { ft.dwLowDateTime, ft.dwHighDateTime };
	ULARGE_INTEGER const u2 = { ft2.dwLowDateTime, ft2.dwHighDateTime };
	ULONGLONG span = (std::max)(u1.QuadPart, u2.QuadPart) - (std::min)(u1.QuadPart, u2.QuadPart);
	int secs = span / 10000000;
	return secs;
}

// sort by LastWriteTime
bool LatestSort(WIN32_FIND_DATA a, WIN32_FIND_DATA b)
{
	FILETIME ft1 = a.ftLastWriteTime;
	FILETIME ft2 = b.ftLastWriteTime;
	ULARGE_INTEGER const u1 = { ft1.dwLowDateTime, ft1.dwHighDateTime };
	ULARGE_INTEGER const u2 = { ft2.dwLowDateTime, ft2.dwHighDateTime };
	return (u1.QuadPart > u2.QuadPart);
}

void copyminidump()
{
	// check source folder
	std::wstring spath(TEXT("C:\\Windows\\minidump"));
	if (!PathIsDirectory(spath.c_str()))
	{
		LOG_WARN("invalid path");
		return;
	}
	// check target folder
	wstring tpath = L"D:\\minidump";
	DeleteDirectory(tpath);
	if (!PathIsDirectory(tpath.c_str()))
	{
		LOG_WARN("CreateDirectory");
		if (!CreateDirectory(tpath.c_str(), NULL)) {
			LOG_WARN("CreateDirectory failed");
			return;
		}
	}
	// traverse minidump folder
	vector<WIN32_FIND_DATA> myvector;
	TCHAR sp[MAX_PATH];
	TCHAR tp[MAX_PATH];
	SYSTEMTIME lt;
	GetSystemTime(&lt);
	LOG_INFO("The utc time is: %02d-%02d-%02d %02d:%02d:%02d\n", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	wstring dirpath = spath + L"\\*.*";
	hFind = FindFirstFile(dirpath.c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		LOG_INFO("FindFirstFile failed (%d)\n", GetLastError());
		return;
	}
	do
	{
		if ('.' == wfd.cFileName[0])
		{
			continue;
		}
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		int diff = DiffSecs(wfd.ftLastWriteTime, lt);
		LOG_INFO("dump: %s %u %uB %ds", Utility::UnicodeToAnsi(wfd.cFileName).c_str(), wfd.dwFileAttributes, wfd.nFileSizeLow, diff);
		// Within 7 days && No more than 10 MB
		if (diff <= 7 * 24 * 60 * 60 && wfd.nFileSizeHigh == 0 && wfd.nFileSizeLow <= 10 * 1024 * 1024)
		{
			LOG_INFO("push_back");
			myvector.push_back(wfd);
		}
	} while (FindNextFile(hFind, &wfd));
	FindClose(hFind);

	std::sort(myvector.begin(), myvector.end(), LatestSort);
	// Copy latest 10 files
	int count = 10;
	for (std::vector<WIN32_FIND_DATA>::iterator it = myvector.begin(); it != myvector.end(); ++it)
	{
		LOG_INFO("copy %s", Utility::UnicodeToAnsi(it->cFileName).c_str());
		swprintf_s(sp, MAX_PATH, L"%s\\%s", spath.c_str(), wfd.cFileName);
		swprintf_s(tp, MAX_PATH, L"%s\\%s", tpath.c_str(), wfd.cFileName);
		CopyFile(sp, tp, false);
		if (--count <= 0) break;
	}
}
#define XPS_PRINTER_PORT "Ctyun:"
int main()
{
	std::cout << "Hello World!\n";
	//copyminidump();
	//void StructSort();
	//StructSort();
	wstring wprinterName(L"\\\\a\\bc");
	wstring wportName = TEXT(XPS_PRINTER_PORT) + wprinterName;;
	for (int i = 0; i < wportName.size(); i++)
	{
		if (wportName[i] == L'\\') wportName[i] = L' ';
	}
	wprintf(L"== %s", wportName.c_str());
}
