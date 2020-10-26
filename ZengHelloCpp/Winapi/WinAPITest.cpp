#include "WinAPITest.h"
#include <cstdio>
#include <tchar.h>
#include <windows.h>

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