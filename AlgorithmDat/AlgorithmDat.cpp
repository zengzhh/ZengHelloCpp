// AlgorithmDat.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include <tchar.h>
#include "mylog.h"
#include "json/json.h"
#include "aes128.h"

using namespace std;

#pragma pack(1)
typedef struct MyStruct
{
    uint8_t cafe[16];
    uint32_t timestamp;
    uint8_t protocol[2];
    uint16_t keyNums;
    uint16_t fileAlgType;
    uint16_t fileAlgKeyLen;
    uint8_t keep[20];
};

typedef struct AlgStruct
{
    uint8_t key[32];
	uint16_t keyLen;
	uint16_t type;
	uint16_t isDecrypt;
    uint8_t depId[4];
    uint8_t keep[6];
};
#pragma pack()

int generate(CONST char* filepath, string jsonstr)
{
    LOG_INFO("Hello World!");
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(jsonstr, root))
    {
        LOG_ERROR("Parse json error");
        return 0;
    }

    FILE* pFile;
    pFile = fopen(filepath, "ab");
    auto updateTime = root["updateTime"].asString();
    auto version = root["version"].asString();
	auto keyEncrypt = root["keyEncrypt"];
	auto keyDecrypt = root["keyDecrypt"];
	uint16_t keyNums = keyEncrypt.size() + keyDecrypt.size();
    auto fileAlgorithm = root["fileAlgorithm"];
    auto fileAlgType = fileAlgorithm["algType"].asInt();
	auto fileAlgKeyLen = fileAlgorithm["algKeyLen"].asInt();

    MyStruct head = { 0 };

    // part1
    string tag = "#eCl云&桌*面oud?"; // 23 65 43 6c d4 c6 26 d7 c0 2a c3 e6 6f 75 64 3f
    const char* p1 = tag.c_str();
    memcpy(&head.cafe, p1, 16);
    // part2
    uint32_t u;
    std::stringstream ss;
    ss << updateTime;
    ss >> u;
    printf("timestamp is %u", u);
    head.timestamp = u;
    // part3
    const char* p3 = version.c_str();
    memcpy(&head.protocol, p3, 2);
    // part4
    head.keyNums = keyNums;
    // part5
    head.fileAlgType = fileAlgType;
	// part6
    head.fileAlgKeyLen = fileAlgKeyLen;
    fwrite(&head, 1, sizeof(MyStruct), pFile);
    fflush(pFile);

    AES_ctx ctx;
	const unsigned char userKey[] = { 0x00, 0x01, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0xe, 0xf, 0x00 };
	AES_init_ctx(&ctx, userKey);
    unsigned char buff[16] = { 0 };

    if (keyEncrypt.isArray())
    {
        LOG_INFO("-- keyEncrypt size %d", keyEncrypt.size());
		for (Json::Value::ArrayIndex i = 0; i < keyEncrypt.size(); i++)
		{
            auto k = keyEncrypt[i];
			auto algKey = k["algKey"].asString();
			memcpy(buff, algKey.c_str(), 16);
			AES_ECB_encrypt(&ctx, buff);
            auto algType = k["algType"].asInt();
            auto depId = k["depId"].asString();
            LOG_INFO("%d: %d %s %s", i, algType, algKey.c_str(), depId.c_str());
            AlgStruct body = {0};
            memcpy(&body.key, buff, 16);
            body.keyLen = algKey.size();
            body.type = algType;
            body.isDecrypt = 0;
            memcpy(&body.depId, depId.c_str(), depId.size());
            fwrite(&body, 1, sizeof(AlgStruct), pFile);
        }
    }
	if (keyDecrypt.isArray())
	{
		LOG_INFO("-- keyDecrypt size %d", keyDecrypt.size());
		for (Json::Value::ArrayIndex i = 0; i < keyDecrypt.size(); i++)
		{
			auto k = keyDecrypt[i];
			auto algKey = k["algKey"].asString();
			memcpy(buff, algKey.c_str(), 16);
			AES_ECB_encrypt(&ctx, buff);
			auto algType = k["algType"].asInt();
			auto depId = k["depId"].asString();
			LOG_INFO("%d: %d %s %s", i, algType, algKey.c_str(), depId.c_str());
			AlgStruct body = { 0 };
			memcpy(&body.key, buff, 16);
			body.keyLen = algKey.size();
			body.type = algType;
            body.isDecrypt = 1;
			memcpy(&body.depId, depId.c_str(), depId.size());
			fwrite(&body, 1, sizeof(AlgStruct), pFile);
		}
	}
    
    fclose(pFile);
    LOG_INFO("SUCCESS!");
    return 0;
}

void aes_ecb_ctx() {
    AES_ctx ctx;
    const unsigned char userKey[] = { 0x00, 0x01, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0xe, 0xf, 0x00 };
    AES_init_ctx(&ctx, userKey);

    // encrypt
    string strin = "2Qxyz5fJaBT2J1nG";
    unsigned char buff[16] = {0};
    memcpy(buff, strin.c_str(), 16);
    AES_ECB_encrypt(&ctx, buff);
	for (int i = 0; i < 16; i++)
	{
		printf("%02x ", buff[i]);
	}
	printf("\n");

    // decrypt
    AES_ECB_decrypt(&ctx, buff);
	for (int i = 0; i < 16; i++)
	{
		printf("%02x ", buff[i]);
	}
    printf("\n");
}

int main()
{
    LOG_INFO("hello");
    aes_ecb_ctx();
	//generate("D:\\filecryptoalg-a.dat", string("{\"updateTime\":\"1613630092\",\"version\":\"v1\",\"fileAlgorithm\":{\"algType\":1,\"algKeyLen\":16},\"keyEncrypt\":[{\"algType\":1,\"algKey\":\"Vprt8UirdqHRTKeW\",\"depId\":\"A\"}],\"keyDecrypt\":[{\"algType\":1,\"algKey\":\"Vprt8UirdqHRTKeW\",\"depId\":\"A\"},{\"algType\":1,\"algKey\":\"SSzT2WB1Cx0DdESX\",\"depId\":\"B\"},{\"algType\":1,\"algKey\":\"2Qxyz5fJaBT2J1nG\",\"depId\":\"C\"}]}"));
	//generate("D:\\filecryptoalg-b.dat", string("{\"updateTime\":\"1613630092\",\"version\":\"v1\",\"fileAlgorithm\":{\"algType\":1,\"algKeyLen\":16},\"keyEncrypt\":[{\"algType\":1,\"algKey\":\"Vprt8UirdqHRTKeW\",\"depId\":\"A\"},{\"algType\":1,\"algKey\":\"SSzT2WB1Cx0DdESX\",\"depId\":\"B\"}],\"keyDecrypt\":[{\"algType\":1,\"algKey\":\"SSzT2WB1Cx0DdESX\",\"depId\":\"B\"}]}"));
	//generate("D:\\filecryptoalg-c.dat", string("{\"updateTime\":\"1613630092\",\"version\":\"v1\",\"fileAlgorithm\":{\"algType\":1,\"algKeyLen\":16},\"keyEncrypt\":[{\"algType\":1,\"algKey\":\"Vprt8UirdqHRTKeW\",\"depId\":\"A\"},{\"algType\":1,\"algKey\":\"2Qxyz5fJaBT2J1nG\",\"depId\":\"C\"}],\"keyDecrypt\":[{\"algType\":1,\"algKey\":\"2Qxyz5fJaBT2J1nG\",\"depId\":\"C\"}]}"));
}
