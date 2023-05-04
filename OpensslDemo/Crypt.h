#pragma once
#include <string>
#include <vector>
#include "openssl/aes.h"


namespace Crypt
{
	const std::string sha256(const std::string& strSrc);
	const std::string AesCbcEncrypt(const std::string& strText, const std::string& strKey);
	const std::string AesCbcDecrypt(const std::string& strText, const std::string& strKey);
	std::vector<unsigned char> AesEncrypt(const std::string& strText, const std::string& strKey, std::string& ivecStr, int enc);
	const std::string AesEcbEncrypt(const std::string& strText, const std::string& strKey);
	const std::string AesEcbDecrypt(const std::string& strText, const std::string& strKey);
}
