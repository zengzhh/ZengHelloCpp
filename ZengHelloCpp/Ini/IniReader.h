#pragma once
#ifndef INIREADER_H
#define INIREADER_H

#include <windows.h>

class CIniReader
{
public:
	CIniReader(TCHAR* szFileName);
	int ReadInteger(TCHAR* szSection, TCHAR* szKey, int iDefaultValue);
	float ReadFloat(TCHAR* szSection, TCHAR* szKey, float fltDefaultValue);
	bool ReadBoolean(TCHAR* szSection, TCHAR* szKey, bool bolDefaultValue);
	TCHAR* ReadString(TCHAR* szSection, TCHAR* szKey, const TCHAR* szDefaultValue);
	TCHAR* ReadStringEx(TCHAR* szSection, TCHAR* szKey, const TCHAR* szDefaultValue, const int len);
private:
	TCHAR m_szFileName[255];
};
#endif//INIREADER_H#pragma once
