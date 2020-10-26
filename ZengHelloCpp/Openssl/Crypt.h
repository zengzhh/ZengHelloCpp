#pragma once
#include <string>
#include "openssl/aes.h"


namespace Crypt
{
	const std::string sha256(const std::string& strSrc);
	/*
	enc: 1-º”√‹£ª0-Ω‚√‹
	*/
	const std::string AesEncrypt(const std::string& strText, const std::string& strKey, int enc);

	const std::string AesCbcEncrypt();
}
