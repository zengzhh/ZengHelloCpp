// main.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "Common/UtilsTest.h"
#include "Glib/GlibTest.h"
#include "Json/JsonTest.h"
#include "Openssl/CryptTest.h"
#include "Rapidxml/RapidXmlTest.h"
#include "Winapi/WinAPITest.h"
#include "Xxtea/XxteaTest.h"

using namespace std;

int main()
{
	cout << "Hello CPP\n\n";

	RapidXmlTest rapidXmlTest;
	rapidXmlTest.test_createxml();
	rapidXmlTest.test_accessdom();

	UtilsTest utilsTest;
	utilsTest.test_SplitString();

	WinAPITest winAPITest;
	winAPITest.test_strsafe_StringCchPrintf();
	winAPITest.test_winreg_RegOpenKey();

	XxteaTest xxteaTest;
	xxteaTest.test_encrypt();
	xxteaTest.test_decrypt();

	JsonTest jsonTest;
	jsonTest.test_parsejson();
	jsonTest.test_buildjson();

	CryptTest cryptTest;
	cryptTest.test_aes();

	GlibTest glibTest;
	glibTest.test_StringUtility();
	glibTest.test_Base64Encoding();

	cin.get();
}