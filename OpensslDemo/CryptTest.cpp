#include "CryptTest.h"
#include <iostream>
#include "json/json.h"
#include "Crypt.h"
#include "..\Common\Utils.h"

using namespace std;

void test_aescbc()
{
	cout << "**** test_aescbc()" << endl;

	std::string strKey("Ir1RYxDcJCO{5#yi");


	std::string passwd("2022@test_passwdstr");
	printf("utf8: %s %d\n", passwd.c_str(), passwd.length());
	std::string certHex = Crypt::AesCbcEncrypt(passwd, strKey);
	printf("en(hex): %s\n", certHex.c_str());

	std::string decrypted = Crypt::AesCbcDecrypt(certHex, strKey);
	printf("utf8: %s %d\n", decrypted.c_str(), decrypted.length());
}

void test_aescbc2()
{
//	// encrypt: char* -> unsigned char* -> hex
//	std::string passwd("2022@test_passwdstr");
//	printf("utf8: %s %d\n", passwd.c_str(), passwd.length());
//
//	std::string strKey("Ir1RYxDcJCO{5#yi");
//	std::string ivecStr("fwtWXTQBt3VG@lGu");
//
//	vector<unsigned char> strByte = Crypt::AesEncrypt(passwd, strKey, ivecStr, 1);
//	string strHex;
//	printf("byte[]: ");
//	for (int i = 0; i < strByte.size(); i++)
//	{
//		printf("%d ", (unsigned char)strByte.at(i));
//	}
//	printf("\n");
//
//	int hexLen = strByte.size() * 2;
//	std::unique_ptr<char[]> hex = std::make_unique<char[]>(hexLen + 1);
//	for (int i = 0; i < strByte.size(); i++)
//		sprintf_s(hex.get() + i * 2, 3, "%02x", (unsigned char)strByte[i]);
//	printf("hex: %s", hex.get());
//
//	// decrypt: hex -> unsigned char* -> char*
//	string strText = string(hex.get());
//	int hexLen = strText.length();
//	int byteLen = hexLen / 2;
//	std::unique_ptr<unsigned char[]> pIn = std::make_unique<unsigned char[]>(byteLen);
//	Utils::HexLowerToByte(strText.c_str(), pIn.get(), strText.length());
//	for (int i = 0; i < byteLen; i++)
//	{
//		printf("%d ", (unsigned char)pIn[i]);
//	}
//	printf("\n");
//
//	vector<unsigned char> strByte = Crypt::AesEncrypt(passwd, strKey, ivecStr, 0);

	printf("\n");
}

void test_aesecb()
{
	cout << "**** test_aesecb()" << endl;
	// encrypt: char* -> unsigned char* -> hex
	string k1 = "1234567890";
	std::string strKey("@ctYun@0pen@plat");
	std::string out = Crypt::AesEcbEncrypt(k1, strKey);
	for (int i = 0; i < out.length(); i++)
	{
		printf("%d ", (unsigned char)out.c_str()[i]);
	}
	printf("\n");

	std::unique_ptr<char[]> pEnc = std::make_unique<char[]>(out.length() * 2 + 1);

	for (int i = 0; i < out.length(); i++)
		sprintf_s(pEnc.get() + i * 2, 3, "%02x", (unsigned char)out[i]);

	pEnc[out.length() * 2] = 0;
	printf("en: %s\n", pEnc.get());

	// decrypt: hex -> unsigned char* -> char*
	int deslen = strlen(pEnc.get()) / 2;
	unsigned char* des = (unsigned char*)calloc(deslen + 1, 1);
	Utils::HexLowerToByte(pEnc.get(), des, strlen(pEnc.get()));
	for (int i = 0; i < deslen; i++)
	{
		printf("%d ", (unsigned char)des[i]);
	}
	printf("\n");

	std::string decrypted = Crypt::AesEcbDecrypt(std::string((char*)des), strKey);
	printf("out: %s\n", decrypted.c_str());
}

void test_aesecb_uc()
{
	cout << "**** test_aesecb_uc()" << endl;
	string strin = "Vprt8UirdqHRTKeW";
	const unsigned char* in = (const unsigned char*)strin.c_str();
	string strkey = "SSzT2WB1Cx0DdESX";
	const unsigned char* userKey = (const unsigned char*)strkey.c_str();
	int intFlag = 0;
	AES_KEY key;
	// encrypt
	intFlag = AES_set_encrypt_key(userKey, AES_BLOCK_SIZE * 8, &key);
	if (intFlag < 0) {
		printf("AES_set_encrypt_key failed: %d", intFlag);
	}
	unsigned char en[16] = {0};
	AES_ecb_encrypt(in, en, &key, AES_ENCRYPT);
	for (int i= 0; i< 16; i++)
	{
		printf("%02x ", en[i]);
	}
	printf("\n");
	//decrypt
	intFlag = AES_set_decrypt_key(userKey, AES_BLOCK_SIZE * 8, &key);
	if (intFlag < 0) {
		printf("AES_set_encrypt_key failed: %d", intFlag);
	}
	unsigned char de[16] = { 0 };
	AES_ecb_encrypt(en, de, &key, AES_DECRYPT);
	for (int i = 0; i < 16; i++)
	{
		printf("%02x ", de[i]);
	}
	printf("\n");
}
