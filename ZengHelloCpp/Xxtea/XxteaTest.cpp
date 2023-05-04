#include "XxteaTest.h"
#include <iostream>
#include <string>
#include "xxtea.h"
#include "../../Common/Utils.h"

using namespace std;

void XxteaTest::test_encrypt()
{
	cout << "**** test_encrypt()" << endl;
	// src(char*) -> des(unsigned char*) -> hex(char*)
	string src = "hello xxtea";
	// º”√‹
	size_t deslen = 0;
	uint8_t* des = (uint8_t*)xxtea_encrypt(src.c_str(), src.length(), "Agent&Sdk_2@20#Tea*", &deslen);
	// ±‡¬Î
	int hexlen = deslen * 2;
	char* hex = (char*)calloc(hexlen + 1, 1);
	Utils::ByteToHexLower(des, hex, deslen);
	printf("%d %s\n", deslen, hex); // 16 84e99106b26485c78caede6affc65dc2
	free(des);
	free(hex);
}

void XxteaTest::test_decrypt()
{
	cout << "**** test_decrypt()" << endl;
	// hex(char*) -> des(unsigned char*) -> src(char*)
	string hex = "84e99106b26485c78caede6affc65dc2";
	// Ω‚¬Î
	int deslen = hex.length() / 2;
	unsigned char* des = (unsigned char*)calloc(deslen + 1, 1);
	Utils::HexLowerToByte(hex.c_str(), des, hex.length());
	// Ω‚√‹
	size_t srclen = 0;
	char* src = (char*)xxtea_decrypt(des, deslen, "abc", &srclen);
	printf("%d %s\n", srclen, src); // 11 hello xxtea
	free(des);
	free(src);
}
