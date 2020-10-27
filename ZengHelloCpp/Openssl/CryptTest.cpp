#include "CryptTest.h"
#include <iostream>
#include "json/json.h"
#include "..\Openssl/Crypt.h"
#include "..\Common\Utils.h"

using namespace std;

void CryptTest::test_aes()
{
	cout << "**** test_aes()" << endl;
	// encrypt: char* -> unsigned char* -> hex
	Json::Value root;
	root["checkParam"] = "apiKey=testkeyasdsadsadasdas&nonStr=TYphone9&timestamp=1601005126&sign=94a1bc9cd97adce8e72cdb8d62c10606";
	auto strCertOrg = root.toStyledString();
	printf("[net] strCertOrg: %s\n", strCertOrg.c_str());

	std::string strKey("@ctYun@0pen@plat");
	std::string cert = Crypt::AesEncrypt(strCertOrg, strKey, AES_ENCRYPT);
	for (int i = 0; i < cert.length(); i++)
	{
		printf("%d ", (unsigned char)cert.c_str()[i]);
	}
	printf("\n");

	std::unique_ptr<char[]> pEnc = std::make_unique<char[]>(cert.length() * 2 + 1);

	for (int i = 0; i < cert.length(); i++)
		sprintf_s(pEnc.get() + i * 2, 3, "%02x", (unsigned char)cert[i]);

	pEnc[cert.length() * 2] = 0;
	printf("%s\n", pEnc.get());

	// decrypt: hex -> unsigned char* -> char*
	int deslen = strlen(pEnc.get()) / 2;
	unsigned char* des = (unsigned char*)calloc(deslen + 1, 1);
	Utils::HexLowerToByte(pEnc.get(), des, strlen(pEnc.get()));
	for (int i = 0; i < deslen; i++)
	{
		printf("%d ", (unsigned char)des[i]);
	}
	printf("\n");

	std::string decrypted = Crypt::AesEncrypt(std::string((char*)des), strKey, AES_DECRYPT);
	printf("%s\n", decrypted.c_str());
}
