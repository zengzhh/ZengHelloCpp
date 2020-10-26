#include "IniWriter.h"
#include <iostream>
#include <Windows.h>
#include <tchar.h>

CIniWriter::CIniWriter(TCHAR* szFileName)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, szFileName, _tcslen(szFileName) * sizeof(TCHAR));
}
void CIniWriter::WriteInteger(TCHAR* szSection, TCHAR* szKey, int iValue)
{
	TCHAR szValue[255];
	_stprintf(szValue, TEXT("%d"), iValue);
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
void CIniWriter::WriteFloat(TCHAR* szSection, TCHAR* szKey, float fltValue)
{
	TCHAR szValue[255];
	_stprintf(szValue, TEXT("%f"), fltValue);
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
void CIniWriter::WriteBoolean(TCHAR* szSection, TCHAR* szKey, bool bolValue)
{
	TCHAR szValue[255];
	_stprintf(szValue, TEXT("%s"), bolValue ? "True" : "False");
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
void CIniWriter::WriteString(TCHAR* szSection, TCHAR* szKey, TCHAR* szValue)
{
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}