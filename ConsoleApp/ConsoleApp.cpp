// ConsoleApp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <iostream>
#include <map>
#include <windows.h>

using namespace std;

void windowtest()
{
	HWND fhWnd = GetForegroundWindow();
	printf("GetForegroundWindow %x\n", fhWnd);
	if (fhWnd)
	{
		printf("IsWindowEnabled %d\n", IsWindowEnabled(fhWnd));
		printf("Iconic %d\n", IsIconic(fhWnd));
		printf("IsWindowVisible %d\n", IsWindowVisible(fhWnd));
		DWORD pid = 0;
		GetWindowThreadProcessId(fhWnd, &pid);
		printf("pid: %d\n", pid);
		char szTitle[MAX_PATH] = { 0 };
		if (GetWindowTextA(fhWnd, szTitle, MAX_PATH))
		{
			printf("title: %s\n", szTitle);
		}
		else
		{
			printf("title is NULL\n");
		}
	}

	printf("---- FindWindow ----\n");
	HWND hWndTmp = FindWindow(NULL, L"计算器");
	if (hWndTmp)
	{
		printf("IsWindowVisible %d\n", IsWindowVisible(hWndTmp));
		printf("IsIconic %d\n", IsIconic(hWndTmp));
	}
}

int main()
{
	windowtest();
	getchar();
	return 0;
}
