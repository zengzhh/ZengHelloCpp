#include "WinApi.h"
#include <Windows.h>
#include <atlbase.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <commctrl.h>
#include <Tlhelp32.h>
#include "mylog.h"
#include <pdh.h>
#include <winver.h>
#pragma comment(lib,"Pdh.lib")
#pragma comment(lib, "Version.lib")

using namespace std;

bool RegeditNotifyChanged(HKEY hKey_, std::wstring path_)
{
	// 1.创建事件
	HANDLE hNotify = CreateEvent(NULL, FALSE, TRUE, L"RegeditNotifyChanged");
	if (hNotify == NULL || hNotify == INVALID_HANDLE_VALUE)
	{
		cout << "监控事件创建失败" << endl;
		CloseHandle(hNotify);
		return false;
	}

	//2. 打开注册表对应位置
	HKEY hRegKey;
	if (RegOpenKeyEx(hKey_, path_.c_str(), 0, KEY_NOTIFY, &hRegKey) != ERROR_SUCCESS)
	{
		cout << "打开注册表失败" << endl;
		CloseHandle(hNotify);
		RegCloseKey(hRegKey);
		return false;
	}

	//3.使用RegNotifyChangeKeyValue进行监控
	if (RegNotifyChangeKeyValue(hRegKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, hNotify, TRUE) != ERROR_SUCCESS)
	{
		cout << "监控失败" << endl;
		CloseHandle(hNotify);
		RegCloseKey(hRegKey);
		return false;
	}

	if (hNotify && WaitForSingleObject(hNotify, INFINITE) != WAIT_FAILED)
	{
		cout << "监控项发生改变" << endl;
		CloseHandle(hNotify);
		RegCloseKey(hRegKey);
		return true;
	}
	CloseHandle(hNotify);
	RegCloseKey(hRegKey);
	return false;
}

std::wstring GetProcessNameByPID(DWORD pid)
{
	std::wstring process_name = L"";
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
		if (pe.th32ProcessID == pid) {
			CloseHandle(hSnapshot);
			process_name = pe.szExeFile;
			return process_name;
		}
	}
	CloseHandle(hSnapshot);
	return process_name;
}

typedef struct _TRAYDATA
{
	HWND hWnd;
	DWORD uID;
	DWORD uCallbackMessage;
	DWORD Reserved1[2];
	HICON hIcon;
	DWORD Reserved2[3];
} TRAYDATA, * PTRAYDATA;

// 用户提示通知区域
void ShowTrayIconWnd(const std::wstring& processName)
{
	HWND hShellTrayWnd = NULL;
	HWND hTrayNotifyWnd = NULL;
	HWND hSysPagerWnd = NULL;
	HWND hToolbarWindow32Wnd = NULL;
	DWORD dwProcessId = 0;
	DWORD dwButtonCount = 0;
	HANDLE hProcess = NULL;
	LPVOID lpAddress = NULL;
	TBBUTTON tb;

	hShellTrayWnd = FindWindow(TEXT("Shell_TrayWnd"), NULL);
	hTrayNotifyWnd = FindWindowEx(hShellTrayWnd, 0, TEXT("TrayNotifyWnd"), NULL);
	hSysPagerWnd = FindWindowEx(hTrayNotifyWnd, 0, TEXT("SysPager"), NULL);
	if (!hSysPagerWnd)
		hToolbarWindow32Wnd = FindWindowEx(hTrayNotifyWnd, 0, TEXT("ToolbarWindow32"), NULL);
	else
		hToolbarWindow32Wnd = FindWindowEx(hSysPagerWnd, 0, TEXT("ToolbarWindow32"), NULL);
	GetWindowThreadProcessId(hToolbarWindow32Wnd, &dwProcessId);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS
		| PROCESS_VM_OPERATION
		| PROCESS_VM_READ
		| PROCESS_VM_WRITE,
		0,
		dwProcessId);
	lpAddress = VirtualAllocEx(hProcess, 0, 0x4096, MEM_COMMIT, PAGE_READWRITE);
	dwButtonCount = SendMessage(hToolbarWindow32Wnd, TB_BUTTONCOUNT, 0, 0);
	LOG_INFO("num: %d", dwButtonCount);
	for (UINT i = 0; i < dwButtonCount; i++)
	{
		size_t dwLen = 0;
		DWORD dwPID = 0;

		ZeroMemory(&tb, sizeof(tb));
		WriteProcessMemory(hProcess, lpAddress, &tb, sizeof(tb), &dwLen);
		SendMessage(hToolbarWindow32Wnd, TB_GETBUTTON, i, LPARAM(lpAddress));
		ReadProcessMemory(hProcess, lpAddress, &tb, sizeof(tb), &dwLen);

		TRAYDATA stTrayData = { 0 };
		ReadProcessMemory(hProcess, (PVOID)tb.dwData, &stTrayData, sizeof(TRAYDATA), &dwLen);//返回到本地的结构中dwData成员表示托盘图标句柄所在的位置
		GetWindowThreadProcessId(stTrayData.hWnd, &dwPID);
		std::wstring process_name = GetProcessNameByPID(dwPID);
		char szTitle[MAX_PATH] = { 0 };
		GetWindowTextA(stTrayData.hWnd, szTitle, MAX_PATH);
		LOG_INFO("%u hwnd: %x, pid: %d, process name: %S, window title: %s",
			i, stTrayData.hWnd, dwPID, process_name.c_str(), szTitle[0] ? szTitle : "NULL");
		if (process_name == processName)
		{
			LOG_INFO("find in TrayNotifyWnd");
			SendNotifyMessage(stTrayData.hWnd, stTrayData.uCallbackMessage, stTrayData.uID, WM_LBUTTONDOWN);
			SendNotifyMessage(stTrayData.hWnd, stTrayData.uCallbackMessage, stTrayData.uID, WM_LBUTTONUP);
			SendNotifyMessage(stTrayData.hWnd, stTrayData.uCallbackMessage, stTrayData.uID, WM_LBUTTONDBLCLK);
		}
		if (dwPID == NULL) {
			LOG_INFO("delete button");
			::SendMessage(hToolbarWindow32Wnd, TB_DELETEBUTTON, i, 0);
		}
	}

	VirtualFreeEx(hProcess, lpAddress, 0x4096, MEM_RELEASE);
	CloseHandle(hProcess);

	return;
}

// 托盘隐藏区域
void ShowTrayIconWndEx(const std::wstring& processName)
{
	HWND hNotifyIconOverflowWindowWnd = NULL;
	HWND hToolbarWindow32Wnd = NULL;
	DWORD dwProcessId = 0;
	DWORD dwButtonCount = 0;
	HANDLE hProcess = NULL;
	LPVOID lpAddress = NULL;
	TBBUTTON tb;

	hNotifyIconOverflowWindowWnd = FindWindow(TEXT("NotifyIconOverflowWindow"), NULL);
	hToolbarWindow32Wnd = FindWindowEx(hNotifyIconOverflowWindowWnd, 0, TEXT("ToolbarWindow32"), NULL);
	GetWindowThreadProcessId(hToolbarWindow32Wnd, &dwProcessId);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS
		| PROCESS_VM_OPERATION
		| PROCESS_VM_READ
		| PROCESS_VM_WRITE,
		0,
		dwProcessId);
	lpAddress = VirtualAllocEx(hProcess, 0, 0x4096, MEM_COMMIT, PAGE_READWRITE);
	dwButtonCount = SendMessage(hToolbarWindow32Wnd, TB_BUTTONCOUNT, 0, 0);
	LOG_INFO("num: %d", dwButtonCount);
	for (UINT i = 0; i < dwButtonCount; i++)
	{
		size_t dwLen = 0;
		DWORD dwPID = 0;

		ZeroMemory(&tb, sizeof(tb));
		WriteProcessMemory(hProcess, lpAddress, &tb, sizeof(tb), &dwLen);
		SendMessage(hToolbarWindow32Wnd, TB_GETBUTTON, i, LPARAM(lpAddress));
		ReadProcessMemory(hProcess, lpAddress, &tb, sizeof(tb), &dwLen);

		TRAYDATA stTrayData = { 0 };
		ReadProcessMemory(hProcess, (PVOID)tb.dwData, &stTrayData, 40, &dwLen);//返回到本地的结构中dwData成员表示托盘图标句柄所在的位置
		GetWindowThreadProcessId(stTrayData.hWnd, &dwPID);
		std::wstring process_name = GetProcessNameByPID(dwPID);
		char szTitle[MAX_PATH] = { 0 };
		GetWindowTextA(stTrayData.hWnd, szTitle, MAX_PATH);
		LOG_INFO("%u hwnd: %x, pid: %d, process name: %S, window title: %s",
			i, stTrayData.hWnd, dwPID, process_name.c_str(), szTitle[0] ? szTitle : "NULL");
		if (process_name == processName)
		{
			LOG_INFO("find in NotifyIconOverflowWindow");
			SendNotifyMessage(stTrayData.hWnd, stTrayData.uCallbackMessage, stTrayData.uID, WM_LBUTTONDOWN);
			SendNotifyMessage(stTrayData.hWnd, stTrayData.uCallbackMessage, stTrayData.uID, WM_LBUTTONUP);
			SendNotifyMessage(stTrayData.hWnd, stTrayData.uCallbackMessage, stTrayData.uID, WM_LBUTTONDBLCLK);
		}
		if (dwPID == NULL) {
			LOG_INFO("delete button");
			::SendMessage(hToolbarWindow32Wnd, TB_DELETEBUTTON, i, 0);
		}
	}

	VirtualFreeEx(hProcess, lpAddress, 0x4096, MEM_RELEASE);
	CloseHandle(hProcess);

	return;
}

__int64 FileTime2LargeInt(const FILETIME* ftime)
{
	LARGE_INTEGER li;

	li.LowPart = ftime->dwLowDateTime;
	li.HighPart = ftime->dwHighDateTime;
	return li.QuadPart;
}

double GetProcessCpuUsage(const std::wstring& strExe)
{
	HQUERY query;
	PDH_STATUS status = ::PdhOpenQuery(NULL, NULL, &query);

	if (status != ERROR_SUCCESS)
		//            std::cout << "Open Query Error" << std::endl;
		return 0;

	HCOUNTER counter;
	std::wstring strPath = L"\\Process(" + strExe + L")\\% Processor Time";
	status = ::PdhAddCounter(query, strPath.c_str(), NULL, &counter);

	if (status != ERROR_SUCCESS)
	{
		//            std::cout << "Add Counter Error" << std::endl;
		::PdhCloseQuery(query);
		return 0;
	}

	::PdhCollectQueryData(query);

	::Sleep(1000);

	::PdhCollectQueryData(query);

	PDH_FMT_COUNTERVALUE pdhValue;
	DWORD dwValue;

	status = ::PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
	if (status != ERROR_SUCCESS)
	{
		//         std::cout << "Get Value Error" << std::endl;
		::PdhRemoveCounter(&counter);
		::PdhCloseQuery(query);
		return 0;
	}

	//	std::cout << pdhValue.longValue << std::endl;
	auto ret = pdhValue.doubleValue;

	::PdhRemoveCounter(&counter);
	::PdhCloseQuery(query);
	return ret;
}

void getTotalCpu()
{
	FILETIME idleTimeStart, idleTimeEnd;
	FILETIME kernelTimeStart, kernelTimeEnd;
	FILETIME userTimeStart, userTimeEnd;

	::GetSystemTimes(&idleTimeStart, &kernelTimeStart, &userTimeStart);
	double cpu0 = GetProcessCpuUsage(L"Taskmgr");
	::GetSystemTimes(&idleTimeEnd, &kernelTimeEnd, &userTimeEnd);

	__int64 idle = FileTime2LargeInt(&idleTimeEnd) - FileTime2LargeInt(&idleTimeStart);
	__int64 kernel = FileTime2LargeInt(&kernelTimeEnd) - FileTime2LargeInt(&kernelTimeStart);
	__int64 user = FileTime2LargeInt(&userTimeEnd) - FileTime2LargeInt(&userTimeStart);
	__int64 total = kernel + user;

	__int64 cpu = ((total - idle) * 100 + total / 2) / total;
	printf("idle: %lld, kernel:%lld, user:%lld, total:%lld, cpu: %lld, %lf\n", idle, kernel, user, total, cpu, cpu0);
}

void testTotalCpu()
{
	while(1)
	{
		getTotalCpu();
		Sleep(2000);
	}
}

wstring GetFileVersion(const WCHAR* filename)
{
	wstring asVer = L"";
	VS_FIXEDFILEINFO* pVsInfo;
	unsigned int iFileInfoSize = sizeof(VS_FIXEDFILEINFO);
	int iVerInfoSize = GetFileVersionInfoSize(filename, NULL);
	if (iVerInfoSize != 0)
	{
		WCHAR* pBuf = NULL;

		while (!pBuf)
		{
			pBuf = new wchar_t[iVerInfoSize];
		}
		if (GetFileVersionInfo(filename, 0, iVerInfoSize, pBuf))
		{
			if (VerQueryValueA(pBuf, "\\", (void**)&pVsInfo, &iFileInfoSize))
			{
				swprintf_s(pBuf, iVerInfoSize, L"%d.%d.%d.%d", HIWORD(pVsInfo->dwFileVersionMS), LOWORD(pVsInfo->dwFileVersionMS), HIWORD(pVsInfo->dwFileVersionLS), LOWORD(pVsInfo->dwFileVersionLS));
				asVer = pBuf;
			}
		}
		delete[]pBuf;
	}
	return asVer;
}

typedef struct DriveEntry
{
	TCHAR* nName;
	int nAvail;
	int nTotal;
	int nFree;
};

DWORD WINAPI MyCreateProcess(LPVOID pCmd)
{
	//auto pParam = std::make_shared<std::string>(*static_cast<std::string*>(pCmd));
	std::shared_ptr<std::wstring> pParam(static_cast<std::wstring*>(pCmd));

	if (!pParam || pParam->empty())
	{
		LOG(ERROR, __FUNCTION__ "pParam is nullptr");
		return 0;
	}
	USES_CONVERSION;
	LOG_INFO("Start: %s", W2A(pParam->c_str()));

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.lpDesktop = const_cast<LPTSTR>(L"Winsta0\\winlogon");

	PROCESS_INFORMATION pi = { 0 };

	BOOL ret = CreateProcess(NULL, (LPTSTR)pParam->c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	LOG_INFO("End %d", ret);
	return ret;
}

void printDiskSpace()
{
	/* eg C:\ D:\ */
	TCHAR _driveString[256];
	/* eg C: */
	TCHAR _drive[16];
	ULARGE_INTEGER FreeAvailable, TotalNum, TotalFreeNum;

	GetLogicalDriveStrings(sizeof(_driveString), _driveString);

	TCHAR* pDrive = _driveString;
	while (*pDrive && _tcslen(_driveString) > 1)
	{
		memset(_drive, 0, sizeof(_drive));
		memcpy(_drive, pDrive, (_tcslen(_driveString) - 1) * sizeof(TCHAR));

		UINT driveType = GetDriveType(_drive);
		if (driveType == DRIVE_FIXED)
		{
			BOOL ret = GetDiskFreeSpaceEx(_drive, &FreeAvailable, &TotalNum, &TotalFreeNum);
			if (ret)
			{
				double nAvail = FreeAvailable.QuadPart / (double)(1024 * 1024 * 1024);
				double nTotal = TotalNum.QuadPart / (double)(1024 * 1024 * 1024);
				//double nFree = TotalFreeNum.QuadPart / (double)(1024 * 1024 * 1024);
				LOG_INFO("%S %.1fGB %.1fGB", _drive, nAvail, nTotal);
			}
		}
		pDrive = pDrive + _tcslen(pDrive) + 1;
	}
}

/* 	64: 计算机\HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall
	32: 计算机\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall
*/
bool RegeditSubkeyChanged(HKEY hKey_, std::wstring path_)
{

	// 1.创建事件
	//HANDLE hNotify = CreateEvent(NULL, FALSE, FALSE, L"RegeditNotifyChanged");
	//if (hNotify == NULL || hNotify == INVALID_HANDLE_VALUE)
	//{
	//	cout << "监控事件创建失败" << endl;
	//	CloseHandle(hNotify);
	//	return false;
	//}

	//2. 打开注册表对应位置
	HKEY hRegKey;
	if (RegOpenKeyEx(hKey_, path_.c_str(), 0, KEY_NOTIFY, &hRegKey) != ERROR_SUCCESS)
	{
		cout << "打开注册表失败" << endl;
		//CloseHandle(hNotify);
		RegCloseKey(hRegKey);
		return false;
	}

	//3.使用RegNotifyChangeKeyValue进行监控
	if (RegNotifyChangeKeyValue(hRegKey, FALSE, REG_NOTIFY_CHANGE_NAME, NULL, FALSE) != ERROR_SUCCESS)
	{
		cout << "监控失败" << endl;
		//CloseHandle(hNotify);
		RegCloseKey(hRegKey);
		return false;
	}

	//if (hNotify && WaitForSingleObject(hNotify, INFINITE) != WAIT_FAILED)
	//{
	//	cout << "监控项发生改变" << endl;
	//	//CloseHandle(hNotify);
	//	RegCloseKey(hRegKey);
	//	return true;
	//}
	//CloseHandle(hNotify);
	RegCloseKey(hRegKey);
	return false;
}