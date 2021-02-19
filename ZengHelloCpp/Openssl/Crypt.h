#pragma once
#include <string>
#include "openssl/aes.h"


namespace Crypt
{
	const std::string sha256(const std::string& strSrc);
	const std::string AesCbcEncrypt(const std::string& strText, const std::string& strKey);
	const std::string AesCbcDecrypt(const std::string& strText, const std::string& strKey);
	const std::string AesEcbEncrypt(const std::string& strText, const std::string& strKey);
	const std::string AesEcbDecrypt(const std::string& strText, const std::string& strKey);
}
