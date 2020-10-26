#include "IniReader.h"
#include <iostream>
#include <Windows.h>
#include <tchar.h>

CIniReader::CIniReader(TCHAR* szFileName)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, szFileName, _tcslen(szFileName) * sizeof(TCHAR));
}
int CIniReader::ReadInteger(TCHAR* szSection, TCHAR* szKey, int iDefaultValue)
{
	int iResult = GetPrivateProfileInt(szSection, szKey, iDefaultValue, m_szFileName);
	return iResult;
}
float CIniReader::ReadFloat(TCHAR* szSection, TCHAR* szKey, float fltDefaultValue)
{
	TCHAR szResult[255];
	TCHAR szDefault[255];
	float fltResult;
	_stprintf(szDefault, TEXT("%f"), fltDefaultValue);
	GetPrivateProfileString(szSection, szKey, szDefault, szResult, 255, m_szFileName);
	fltResult = (float)_tstof(szResult);
	return fltResult;
}
bool CIniReader::ReadBoolean(TCHAR* szSection, TCHAR* szKey, bool bolDefaultValue)
{
	TCHAR szResult[255];
	TCHAR szDefault[255];
	bool bolResult;
	_stprintf(szDefault, TEXT("%s"), bolDefaultValue ? TEXT("True") : TEXT("False"));
	GetPrivateProfileString(szSection, szKey, szDefault, szResult, 255, m_szFileName);
	bolResult = (_tcscmp(szResult, TEXT("True")) == 0 ||
		_tcscmp(szResult, TEXT("true")) == 0) ? true : false;
	return bolResult;
}
TCHAR* CIniReader::ReadString(TCHAR* szSection, TCHAR* szKey, const TCHAR* szDefaultValue)
{
	TCHAR* szResult = new TCHAR[255];
	memset(szResult, 0x00, 255);
	GetPrivateProfileString(szSection, szKey,
		szDefaultValue, szResult, 255, m_szFileName);
	return szResult;
}
TCHAR* CIniReader::ReadStringEx(TCHAR* szSection, TCHAR* szKey, const TCHAR* szDefaultValue, const int len)
{
	TCHAR* szResult = new TCHAR[len];
	memset(szResult, 0x00, len);
	GetPrivateProfileString(szSection, szKey,
		szDefaultValue, szResult, len, m_szFileName);
	return szResult;
}