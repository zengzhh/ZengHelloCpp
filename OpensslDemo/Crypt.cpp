#include "Crypt.h"
#include <memory>
#include "openssl/sha.h"
#include "..\Common\Utils.h"


namespace Crypt
{
	const std::string sha256(const std::string& strSrc)
	{
		unsigned char buf[33] = { 0 };
		SHA256(reinterpret_cast<const unsigned char*>(strSrc.c_str()), strSrc.length(), buf);

		char dst[65] = { 0 };
		for (int i = 0; i < 32; i++)
			sprintf_s(dst + i * 2, 3, "%02x", buf[i]);

		return dst;
	}

	// encrypt: char* -> unsigned char* -> hex
	const std::string AesCbcEncrypt(const std::string& strText, const std::string& strKey)
	{
		
		if (strText.empty() || strKey.empty()) return "";

		int strLen = strText.length();

		// encrypt
		AES_KEY key;
		if (AES_set_encrypt_key(reinterpret_cast<const unsigned char*>(strKey.c_str()), strKey.size() * 8, &key) < 0)
			return "";
		
		unsigned char ivec[AES_BLOCK_SIZE + 1] = "fwtWXTQBt3VG@lGu";

		int byteLen = (strLen % AES_BLOCK_SIZE ? strLen + (AES_BLOCK_SIZE - strLen % AES_BLOCK_SIZE) : strLen);

		auto pOut = std::make_unique<unsigned char[]>(byteLen);

		AES_cbc_encrypt(reinterpret_cast<const unsigned char*>(strText.c_str()), reinterpret_cast<unsigned char*>(pOut.get()), strLen, &key, ivec, AES_ENCRYPT);
		
		for (int i = 0; i < byteLen; i++)
		{
			printf("%d ", (unsigned char)pOut[i]);
		}
		printf("\n");

		// byte to hex
		int hexLen = byteLen * 2;
		std::unique_ptr<char[]> hex = std::make_unique<char[]>(hexLen + 1);
		for (int i = 0; i < byteLen; i++)
			sprintf_s(hex.get() + i * 2, 3, "%02x", (unsigned char)pOut[i]);

		printf("strLen: %d, byteLen: %d, hexLen: %d\n", strLen, byteLen, hexLen);
		return std::string(reinterpret_cast<const char*>(hex.get()), hexLen);
	}

	// decrypt: hex -> unsigned char* -> char*
	const std::string AesCbcDecrypt(const std::string& strText, const std::string& strKey)
	{
		if (strText.empty() || strKey.empty()) return "";

		// hex to byte
		int hexLen = strText.length();
		int byteLen = hexLen / 2;
		std::unique_ptr<unsigned char[]> pIn = std::make_unique<unsigned char[]>(byteLen);
		Utils::HexLowerToByte(strText.c_str(), pIn.get(), strText.length());
		for (int i = 0; i < byteLen; i++)
		{
			printf("%d ", (unsigned char)pIn[i]);
		}
		printf("\n");

		// decrypt
		AES_KEY key;
		if (AES_set_decrypt_key(reinterpret_cast<const unsigned char*>(strKey.c_str()), strKey.size() * 8, &key) < 0)
			return "";

		unsigned char ivec[AES_BLOCK_SIZE + 1] = "fwtWXTQBt3VG@lGu";

		int outLen = (byteLen % AES_BLOCK_SIZE ? byteLen + (AES_BLOCK_SIZE - byteLen % AES_BLOCK_SIZE) : byteLen);

		auto pOut = std::make_unique<unsigned char[]>(outLen + 1);

		AES_cbc_encrypt(pIn.get(), reinterpret_cast<unsigned char*>(pOut.get()), outLen, &key, ivec, AES_DECRYPT);

		std::string str = std::string(reinterpret_cast<const char*>(pOut.get()), outLen);
		printf("hexLen: %d, byteLen: %d, strLen: %d\n", hexLen, byteLen, str.length());

		return std::string(str.c_str());
	}

	std::vector<unsigned char> AesEncrypt(const std::string& strText, const std::string& strKey, std::string& ivecStr, int enc)
	{
		std::vector<unsigned char> vec;

		if (strText.empty() || strKey.empty())
			return vec;

		if (ivecStr.empty() || ivecStr.length() != AES_BLOCK_SIZE)
			return vec;

		AES_KEY key;
		if (AES_ENCRYPT == enc)
		{
			if (AES_set_encrypt_key(reinterpret_cast<const unsigned char*>(strKey.c_str()), AES_BLOCK_SIZE * 8, &key) < 0)
				return vec;
		}
		else
		{
			if (AES_set_decrypt_key(reinterpret_cast<const unsigned char*>(strKey.c_str()), AES_BLOCK_SIZE * 8, &key) < 0)
				return vec;
		}

		unsigned char ivec[AES_BLOCK_SIZE + 1];
		for (int i = 0; i < AES_BLOCK_SIZE; i++)
		{
			ivec[i] = ivecStr.at(i);
		}
		ivec[AES_BLOCK_SIZE] = 0;

		int len = strText.length();
		int outLen = (len % AES_BLOCK_SIZE ? len + (AES_BLOCK_SIZE - len % AES_BLOCK_SIZE) : len);
		auto pOut = std::make_unique<unsigned char[]>(outLen);

		AES_cbc_encrypt(reinterpret_cast<const unsigned char*>(strText.c_str()), reinterpret_cast<unsigned char*>(pOut.get()), len, &key, ivec, enc);

		for (size_t i = 0; i < outLen; i++)
		{
			vec.push_back(pOut[i]);
		}
		//return std::string((), outLen);
		return vec;
	}

	const std::string AesEcbEncrypt(const std::string& strText, const std::string& strKey)
	{
		if (strText.empty() || strKey.empty()) return "";

		AES_KEY key;
		if (AES_set_encrypt_key(reinterpret_cast<const unsigned char*>(strKey.c_str()), strKey.size() * 8, &key) < 0)
			return "";

		int len = strText.size();
		int outLen = (len % AES_BLOCK_SIZE ? len + (AES_BLOCK_SIZE - len % AES_BLOCK_SIZE) : len);
		auto pOut = std::make_unique<unsigned char[]>(outLen);
		AES_ecb_encrypt(reinterpret_cast<const unsigned char*>(strText.c_str()), reinterpret_cast<unsigned char*>(pOut.get()), &key, AES_ENCRYPT);
		return std::string(reinterpret_cast<const char*>(pOut.get()), outLen);
	}

	const std::string AesEcbDecrypt(const std::string& strText, const std::string& strKey)
	{
		if (strText.empty() || strKey.empty()) return "";

		AES_KEY key;
		if (AES_set_decrypt_key(reinterpret_cast<const unsigned char*>(strKey.c_str()), strKey.size() * 8, &key) < 0)
			return "";

		int len = strText.size();
		int outLen = (len % AES_BLOCK_SIZE ? len + (AES_BLOCK_SIZE - len % AES_BLOCK_SIZE) : len);
		auto pOut = std::make_unique<unsigned char[]>(outLen);
		AES_ecb_encrypt(reinterpret_cast<const unsigned char*>(strText.c_str()), reinterpret_cast<unsigned char*>(pOut.get()), &key, AES_DECRYPT);
		return std::string(reinterpret_cast<const char*>(pOut.get()), outLen);
	}
}
