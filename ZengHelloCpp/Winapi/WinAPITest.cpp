#include "WinAPITest.h"
#include <cstdio>
#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <wchar.h>
#include <strsafe.h>

using namespace std;

void WinAPITest::test_fileapi_GetLogicalDriveStrings() {
	DWORD nBufferLength = 100;
	WCHAR lpBuffer[100];

	GetLogicalDriveStrings(nBufferLength, lpBuffer);

	TCHAR* pDrive = lpBuffer;
	while (*pDrive)
	{
		TCHAR mdrive[10] = { 0 };
		memset(mdrive, 0, sizeof(mdrive));
		memcpy(mdrive, pDrive, (_tcslen(lpBuffer) - 1) * sizeof(TCHAR));

		printf("searchDriver %S, type: %u\n", mdrive, GetDriveType(mdrive));
		pDrive = pDrive + _tcslen(pDrive) + 1;
	}
	Sleep(3000);
}

void WinAPITest::test_strsafe_StringCchPrintf()
{
	TCHAR pszDest[30];
	size_t cchDest = 30;
	LPCTSTR pszFormat = TEXT("%s %d + %d = %d\n");
	TCHAR* pszTxt = (TCHAR*)TEXT("The answer is");
	HRESULT hr = StringCchPrintf(pszDest, cchDest, pszFormat, pszTxt, 1, 2, 3);
	// "The answer is 1 + 2 = 3"
	_tprintf(pszDest);
}

void WinAPITest::test_winreg_RegOpenKey()
{
	cout << "**** test_winreg_RegOpenKey()" << endl;
	HKEY hKey;
	DWORD dwType;
	WCHAR data[1024];
	DWORD dwSize = 1024;
	LONG val = RegOpenKey(HKEY_CURRENT_USER, L"Environment", &hKey);
	printf("RegOpenKey Return %ld\n", val);
	if (val == ERROR_SUCCESS)
	{
		val = RegQueryValueEx(hKey, _T("OneDrive"), 0, &dwType, (BYTE*)data, &dwSize);
		printf("RegQueryValueEx Return %ld\n", val);
		if (val == ERROR_SUCCESS)
		{
			printf("Type: %d, cbData: %d\n", dwType, dwSize);
			_tprintf(L"%s\n", data);
			int j = 0;
			while (j < dwSize / 2) {
				printf("%02x ", (unsigned char)data[j]);
				j++;
			}
			printf("\n");
		}
		RegCloseKey(hKey);
	}
}