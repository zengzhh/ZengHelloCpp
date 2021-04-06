#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <locale>
#include <Windows.h>
#include <ShlObj.h>
#include <atlcomcli.h>
#include <Msi.h>
#include <Tlhelp32.h>
#include "md5.h"
#include "mylog.h"
#pragma comment(lib, "Msi.lib")

using namespace std;

typedef struct _SHORTCUTINFO {
	std::string code;
	std::string name;
	double lastRunDate;
	std::string exename;
}SHORTCUTINFO, * PSHORTCUTINFO;

typedef struct _LNKHEAD
{
	DWORD		dwID;
	DWORD		dwGUID[4];
	DWORD		dwFlags;
	DWORD		dwFileAttributes;
	FILETIME	dwCreationTime;
	FILETIME	dwModificationTime;
	FILETIME	dwLastaccessTime;
	DWORD		dwFileLen;
	DWORD		dwIconNum;
	DWORD		dwWinStyle;
	DWORD		dwHotkey;
	DWORD		dwReserved1;
	DWORD		dwReserved2;
}LNKHEAD, * PLNKHEAD;

typedef struct _FILELOCATIONINFO
{
	DWORD		dwSize;
	DWORD		dwFirstOffset;
	DWORD		dwFlags;
	DWORD		dwOffsetOfVolume;
	DWORD		dwOffsetOfBasePath;
	DWORD		dwOffsetOfNetworkVolume;
	DWORD		dwOffsetOfRemainingPath;
}FILELOCATIONINFO, * PFILELOCATIONINFO;

#define LNK_HASIDLIST	0x1
#define LNK_FILEDIR		0x2
#define LNK_HASDES		0x4
#define LNK_HASPATH		0x8
#define LNK_HASWORKDIR	0x10
#define LNK_HASCMD		0x20

#define LNK_LOCALVOLUME	0x1
#define LNK_NETSHARE	0x2


#define SHORTCUT_ROOT_PATH "C:\\ProgramData\\Microsoft\\Windows\\Start Menu"
#define SHORTCUT_CURRENTUSER_PATH "C:\\Users\\Administrator\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu"
const std::vector<std::string> shortcut_path{ SHORTCUT_ROOT_PATH , SHORTCUT_CURRENTUSER_PATH };

const std::vector<std::string> constfilter = { "home", "工具", "下载", "入门", "意见反馈", "帮助", "官方", "论坛",
"网站", "修复器", "设置", "向导", "皮肤", "uninstall", "dialog", "server", "system", "services", "info", "config",
"update", "readme", "控制台", "control", "help", "官网", "卸载", "website", "主页", "install", "安装", "administrative",
"日志", "管理", "版本", "镜像", "最新", "input", "Speech", "Snipping", "Steps", "Character", "Wordpad", "Component", "Computer",
"dfrgui", "Disk", "Event", "iSCSI", "Memory", "ODBC", "Performance", "Print", "Resource", "Server", "Task", "Immersive",
"MiracastView", "USB", "Magnify", "Narrator", "Command", "Control", "Default", "Devices", "Run", "OpenVPN", "7-Zip",
"Calculator", "Remote Desktop", "Windows Media Player", "XPS", "Policy", "Firewall", "Defender", "Keyboard", "target",
"File Explorer", "PowerShell" };

#define vd_printf LOG_INFO

namespace Utility {
	std::string ANSIToUTF8(const std::string& src)
	{
		int nLength = 0;

		// first step, ANSIToUnicode
		nLength = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
		std::vector<wchar_t> temp(nLength);
		::MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, &temp[0], nLength);

		// second step, UnicodeToUTF8
		nLength = ::WideCharToMultiByte(CP_UTF8, 0, &temp[0], -1, NULL, 0, NULL, NULL);
		std::vector<char> ret(nLength);
		::WideCharToMultiByte(CP_UTF8, 0, &temp[0], -1, &ret[0], nLength, NULL, NULL);
		return &ret[0];
	}

	const std::wstring Utf8ToUnicode(const std::string& src)
	{
		int n = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, nullptr, 0);
		std::vector<wchar_t> temp(n);
		::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, &temp[0], n);
		return &temp[0];
	}

	std::string UTF8ToANSI(const std::string& src)
	{
		int nLength = 0;

		// first step, UTF8ToUnicode
		nLength = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, NULL, 0);
		std::vector<wchar_t> temp(nLength);
		::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, &temp[0], nLength);

		// second step, UnicodeToANSI
		nLength = ::WideCharToMultiByte(CP_ACP, 0, &temp[0], -1, NULL, 0, NULL, NULL);
		std::vector<char> ret(nLength);
		::WideCharToMultiByte(CP_ACP, 0, &temp[0], -1, &ret[0], nLength, NULL, NULL);
		return &ret[0];
	}

	const std::string UnicodeToAnsi(const std::wstring& src)
	{
		int n = ::WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::vector<char> temp(n);
		::WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, &temp[0], n, nullptr, nullptr);
		return &temp[0];
	}

	const std::string UnicodeToUtf8(const std::wstring& src)
	{
		int n = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::vector<char> temp(n);
		::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, &temp[0], n, nullptr, nullptr);
		return &temp[0];
	}

	std::string ParsePathEnv(std::string& path)
	{
		if (path.empty())
		{
			return path;
		}
		std::size_t found1 = path.find_first_of("%");
		if (found1 != std::string::npos)
		{
			std::size_t found2 = path.find_first_of("%", found1 + 1);
			if (found2 != std::string::npos)
			{
				std::string head;
				std::string env;
				std::string tail;
				if (found1 > 0)
				{
					head = path.substr(0, found1);
				}
				if (found2 - found1 - 1 > 0)
				{
					env = path.substr(found1 + 1, found2 - found1 - 1);
				}
				if (found2 + 1 < path.size())
				{
					tail = path.substr(found2 + 1);
				}
				char* cenv = getenv(env.c_str());
				if (cenv)
				{
					std::string newpath = head + std::string(cenv) + tail;
					LOG_INFO("parse %s", path.c_str());
					return ParsePathEnv(newpath);
				}
			}
		}
		return path;
	}

	template<typename charT>
	struct my_equal {
		my_equal(const std::locale& loc) : loc_(loc) {}
		bool operator()(charT ch1, charT ch2) {
			return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
		}
	private:
		const std::locale& loc_;
	};

	// find substring (case insensitive)
	template<typename T>
	int ci_find_substr(const T& str1, const T& str2, const std::locale& loc = std::locale())
	{
		typename T::const_iterator it = std::search(str1.begin(), str1.end(),
			str2.begin(), str2.end(), my_equal<typename T::value_type>(loc));
		if (it != str1.end()) return it - str1.begin();
		else return -1; // not found
	}
}

bool AnalyseShortCut(const char* pFileName, std::string& exePath)
{
	bool				bRet = true;
	FILE* file;
	unsigned short		usLenTemp;
	int					iSize;
	LNKHEAD				head;
	FILELOCATIONINFO	fileLocationInfo;
	char				szFilePath[1024];   //快捷方式所指向的文件
	WCHAR				wszTemp[512];
	char* pDest = NULL;
	DWORD				dwFlags;
	int					p;
	return false;
	if ((file = fopen(pFileName, "rb")) == NULL)
	{
		vd_printf("open file to analyse shortcut failed");
		return false;
	}
	do
	{
		// head
		iSize = sizeof(LNKHEAD);
		if (fread(&head, 1, iSize, file) != iSize)
		{
			vd_printf("read head failed");
			bRet = false;
			break;
		}

		dwFlags = head.dwFlags;

		if (dwFlags & LNK_HASIDLIST)
		{
			// The Shell Item Id List
			if (fread(&usLenTemp, 2, 1, file) != 1)
			{
				vd_printf("read Shell Item Id List failed");
				bRet = false;
				break;
			}
			fseek(file, usLenTemp, SEEK_CUR);
		}

		p = ftell(file);

		// file location info
		if (fread(&fileLocationInfo, sizeof(fileLocationInfo), 1, file) != 1)
		{
			vd_printf("read fileLocationInfo failed");
			bRet = false;
			break;
		}

		fseek(file, fileLocationInfo.dwOffsetOfBasePath + p, SEEK_SET);

		if (fileLocationInfo.dwFlags & LNK_NETSHARE)
		{
			iSize = fileLocationInfo.dwOffsetOfNetworkVolume - fileLocationInfo.dwOffsetOfBasePath;
		}
		else
		{
			iSize = fileLocationInfo.dwOffsetOfRemainingPath - fileLocationInfo.dwOffsetOfBasePath;
		}

		if (fread(&szFilePath, 1, iSize, file) != iSize)
		{
			vd_printf("read szFilePath failed");
			bRet = false;
			break;
		}

		exePath = szFilePath;

	} while (0);

	fclose(file);

	if (bRet)
	{
		vd_printf("exePath:: %s", exePath.c_str());
	}
	else
	{
		vd_printf("analyse shortcut failed");
	}
	return bRet;
}

// 获取快捷方式对应文件名
HRESULT ResolveShortcut(/*in*/ std::wstring linkFilePath, /*out*/ LPTSTR exePath) {
	HRESULT hRes = E_FAIL;
	IShellLink* psl = nullptr;

	TCHAR szPath[MAX_PATH] = { 0 };
	WIN32_FIND_DATA wfd;

	exePath[0] = '\0';

	WCHAR szProductCode[MAX_PATH] = { 0 };
	WCHAR szComponentCode[MAX_PATH] = { 0 };

	// https://docs.microsoft.com/en-us/windows/win32/api/msi/nf-msi-msigetshortcuttargetw
	DWORD errorCode = MsiGetShortcutTarget(linkFilePath.c_str(), szProductCode, nullptr, szComponentCode);
	if (S_OK == errorCode) {
		WCHAR szPathSpecial[MAX_PATH] = { 0 };
		DWORD max_size = MAX_PATH;
		INSTALLSTATE nState = MsiGetComponentPath(szProductCode, szComponentCode, szPathSpecial, &max_size);
		if (nState == INSTALLSTATE_LOCAL || nState == INSTALLSTATE_SOURCE) {
			lstrcpyn(exePath, szPathSpecial, MAX_PATH);
			return ERROR_SUCCESS;
		}
		else
		{
			vd_printf("MsiGetComponentPathA() failed. nState: %d, error: 0x%x", nState, GetLastError());
		}
	}
	else
	{
		vd_printf("MsiGetShortcutTargetA() failed. errorCode: %d, error: 0x%x", errorCode, GetLastError());
	}

	hRes = CoInitialize(NULL);
	if (hRes == S_OK)
	{
		// Get a pointer to the IShellLink interface
		hRes = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl);

		if (SUCCEEDED(hRes)) {
			// Get a pointer to the IPersistFile interface
			IPersistFile* ppf = nullptr;
			hRes = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);

			if (!SUCCEEDED(hRes)) {
				vd_printf("psl->QueryInterface() failed. hRes: 0x%x, error: 0x%x", hRes, GetLastError());
				return hRes;
			}

			// Open the shortcut file and initialize it from its contents
			hRes = ppf->Load(linkFilePath.c_str(), STGM_READ);
			if (SUCCEEDED(hRes)) {
				// 尝试获取到快捷方式的 target 路径。即使目标被删除或者移动。
				hRes = psl->Resolve(nullptr, SLR_UPDATE | SLR_NO_UI);
				if (SUCCEEDED(hRes)) {
					// 获取 Target 路径
					hRes = psl->GetPath(exePath, MAX_PATH, &wfd, SLGP_RAWPATH);
					if (FAILED(hRes)) {
						vd_printf("psl->GetPath() failed. hRes: 0x%x, error: 0x%x", hRes, GetLastError());
						ppf->Release();
						psl->Release();
						return hRes;
					}
					//vd_printf("exePath: %S", exePath);
				}
				else
				{
					vd_printf("psl->Resolve() failed. hRes: 0x%x, error: 0x%x", hRes, GetLastError());
				}
			}
			else
			{
				vd_printf("ppf->Load() failed. hRes: 0x%x, error: 0x%x", hRes, GetLastError());
			}
			ppf->Release();
			psl->Release();
		}
		else
		{
			vd_printf("CoCreateInstance() failed. hRes: 0x%x, error: 0x%x", hRes, GetLastError());
		}
		CoUninitialize();
	}

	return hRes;
}

void SearchLnk(const std::string& path, std::vector<std::string>& v)
{
	HANDLE hFind = NULL;
	WIN32_FIND_DATAA wfd;
	ZeroMemory(&wfd, sizeof(WIN32_FIND_DATAA));
	std::string tmpPath = path + "\\*.*";
	SHORTCUTINFO shortcutInfo;
	hFind = FindFirstFileA(tmpPath.c_str(), &wfd);
	char szDescription[MAX_PATH] = { 0 };
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}

	if (GetFileAttributesA(path.c_str()) == (FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_DIRECTORY))
	{
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
			tmpPath = path + "\\" + wfd.cFileName;
			SearchLnk(tmpPath, v);
		}
		else
		{
			if (strstr(wfd.cFileName, ".lnk"))
			{
				tmpPath = path + "\\" + wfd.cFileName;
				v.push_back(tmpPath);
			}
		}
	} while (FindNextFileA(hFind, &wfd));
	FindClose(hFind);
}

std::vector<SHORTCUTINFO> GetShortCutInfo()
{
	std::vector<std::string> V;
	std::vector<SHORTCUTINFO> shortcutInfo;
	std::map<std::string, std::string> m;
	SHORTCUTINFO s;
	std::string fname;
	for (auto& path : shortcut_path)
	{
		SearchLnk(path, V);
	}

	for (auto v : V)
	{
		bool bFind = false;
		fname = strrchr(v.c_str(), '\\') + 1;
		size_t pos1 = fname.rfind(".lnk");
		fname = fname.substr(0, pos1);
		for (auto f : constfilter)
		{
			if (Utility::ci_find_substr(fname, f) != -1)
			{
				bFind = true;
				break;
			}
		}
		if (!bFind)
		{
			if (!(m.find(fname) != m.end()))
			{
				m.insert(std::pair<std::string, std::string>(fname, v));
				s.code = Utility::ANSIToUTF8(v);
				std::string tmp = strrchr(s.code.c_str(), '\\') + 1;
				size_t pos2 = tmp.rfind(".lnk");
				s.name = tmp.substr(0, pos2);

				// get exename by link
				std::string appLnk = v;
				std::string exePath;
				std::string exeName;
				vd_printf("appLnk: %s", appLnk.c_str());
				std::wstring wLnkName = Utility::Utf8ToUnicode(appLnk);
				std::string lnkName = Utility::UnicodeToAnsi(wLnkName);
				if (!AnalyseShortCut(lnkName.c_str(), exePath))
				{
					const wchar_t* appstr = wLnkName.c_str();
					wchar_t szBuf[MAX_PATH];
					int try_num = 1;
					while (ResolveShortcut(appstr, szBuf) != S_OK && try_num)
					{
						Sleep(100);
						--try_num;
					}
					exePath = Utility::UnicodeToAnsi(szBuf);
				}
				exePath = Utility::ANSIToUTF8(exePath.c_str());
				LOG_INFO("exePath: %s", exePath.c_str());
				if (strstr(exePath.c_str(), ".exe") || strstr(exePath.c_str(), ".EXE"))
				{
					// appName
					exeName = strrchr(exePath.c_str(), '\\') + 1;
					// md5
					std::string aexeName = Utility::UTF8ToANSI(exePath);
					std::string aexeName_env = Utility::ParsePathEnv(aexeName);
					std::string uexeName_env = Utility::ANSIToUTF8(aexeName_env);
					std::string exeMd5 = md5(aexeName_env.c_str(), true);
					LOG_INFO("exeMd5: %s", exeMd5.c_str());
					// digitalSign
				}
				shortcutInfo.push_back(s);
			}
		}
	}

	return shortcutInfo;
}

int main()
{
    std::cout << "Hello World!\n";

	GetShortCutInfo();

	//std::string s("%windir%\\%COMPUTERNAME%\\%COMPUTERNAME%\\%COMPUTERNAME\\py.exe");
	//string ns = parsePath(s);
	//LOG_INFO("%s", ns.c_str());

	std::cout << "End!\n";
}
