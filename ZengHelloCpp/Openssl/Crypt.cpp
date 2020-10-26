#include "Crypt.h"
#include <memory>
#include "openssl/sha.h"


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

	const std::string AesEncrypt(const std::string& strText, const std::string& strKey, int enc)
	{
		if (strText.empty() || strKey.empty())
			return "";

		AES_KEY key;
		if (AES_ENCRYPT == enc)
		{
			if (AES_set_encrypt_key(reinterpret_cast<const unsigned char*>(strKey.c_str()), AES_BLOCK_SIZE * 8, &key) < 0)
				return "";
		}
		else
		{
			if (AES_set_decrypt_key(reinterpret_cast<const unsigned char*>(strKey.c_str()), AES_BLOCK_SIZE * 8, &key) < 0)
				return "";
		}

		unsigned char ivec[AES_BLOCK_SIZE + 1] = "yundia0@020-5@9o";

		int len = strText.length();
		int outLen = (len % AES_BLOCK_SIZE ? len + (AES_BLOCK_SIZE - len % AES_BLOCK_SIZE) : len);
		auto pOut = std::make_unique<unsigned char[]>(outLen);
		AES_cbc_encrypt(reinterpret_cast<const unsigned char*>(strText.c_str()), reinterpret_cast<unsigned char*>(pOut.get()), len, &key, ivec, enc);
		return std::string(reinterpret_cast<const char*>(pOut.get()), outLen);
	}
}
