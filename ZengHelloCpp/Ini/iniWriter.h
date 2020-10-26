#pragma once
#ifndef INIWRITER_H
#define INIWRITER_H

#include <windows.h>

class CIniWriter
{
public:
	CIniWriter(TCHAR* szFileName);
	void WriteInteger(TCHAR* szSection, TCHAR* szKey, int iValue);
	void WriteFloat(TCHAR* szSection, TCHAR* szKey, float fltValue);
	void WriteBoolean(TCHAR* szSection, TCHAR* szKey, bool bolValue);
	void WriteString(TCHAR* szSection, TCHAR* szKey, TCHAR* szValue);
private:
	TCHAR m_szFileName[255];
};
#endif //INIWRITER_H