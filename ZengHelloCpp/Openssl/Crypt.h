#pragma once
#include <string>
#include "openssl/aes.h"


namespace Crypt
{
	const std::string sha256(const std::string& strSrc);
	/*
	enc: 1-���ܣ�0-����
	*/
	const std::string AesEncrypt(const std::string& strText, const std::string& strKey, int enc);

	const std::string AesCbcEncrypt();
}
