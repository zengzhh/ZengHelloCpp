#pragma once
#include <string>
#include <Windows.h>

bool RegeditNotifyChanged(HKEY hKey_, std::wstring path_);
void ShowTrayIconWnd(const std::wstring& processName);
void ShowTrayIconWndEx(const std::wstring& processName);
void testTotalCpu();
std::wstring GetFileVersion(const WCHAR* filename);
// ��ȡ���̿ռ�
void printDiskSpace();
// ��������
DWORD WINAPI MyCreateProcess(LPVOID pCmd);
// ����ж��ע���
bool RegeditSubkeyChanged(HKEY hKey_, std::wstring path_);
// ����·��
std::wstring GetCurrentExePath();
// ѡ��Ŀ¼
void open_folder();
// ö���豸VIDPID
std::string getDeviceVIDPID();