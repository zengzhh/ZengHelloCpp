#pragma once
#include <string>
#include <vector>

namespace Utils
{
	void ByteToHexUpper(const unsigned char* source, char* dest, int sourceLen);
	int HexUpperToByte(const char* source, unsigned char* dest, int sourceLen);
	void ByteToHexLower(const unsigned char* source, char* dest, int sourceLen);
	int HexLowerToByte(const char* source, unsigned char* dest, int sourceLen);
	void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);
}
