#include "Utils.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <ctype.h>

namespace Utils
{
	/* 字节转16进制大写字符 */
	void ByteToHexUpper(const unsigned char* source, char* dest, int sourceLen) {
		unsigned char highByte, lowByte;
		for (short i = 0; i < sourceLen; i++) {
			highByte = source[i] >> 4;
			lowByte = source[i] & 0x0f;
			if (highByte > 0x09) {
				dest[i * 2] = highByte + 0x37;
			}
			else {
				dest[i * 2] = highByte + 0x30;
			}
			if (lowByte > 0x09) {
				dest[i * 2 + 1] = lowByte + 0x37;
			}
			else {
				dest[i * 2 + 1] = lowByte + 0x30;
			}
		}
		return;
	}

	/* 字节转16进制大写字符 */
	static void ByteToHexUpper2(const unsigned char* source, char* dest, int sourceLen) {
		char tmp[3];
		for (short i = 0; i < sourceLen; i++)
		{
			sprintf_s(tmp, "%02X", (unsigned char)source[i]);
			memcpy(&dest[i * 2], tmp, 2);
		}
		return;
	}

	/* 16进制大写字符转字节，返回字节数 */
	int HexUpperToByte(const char* source, unsigned char* dest, int sourceLen) {
		int bytelen = 0;
		unsigned char highByte, lowByte;
		for (short i = 0; i < sourceLen; i += 2) {
			highByte = toupper(source[i]);
			lowByte = toupper(source[i + 1]);
			if (highByte > 0x39) {
				highByte -= 0x37;
			}
			else {
				highByte -= 0x30;
			}
			if (lowByte > 0x39) {
				lowByte -= 0x37;
			}
			else {
				lowByte -= 0x30;
			}
			dest[i / 2] = (highByte << 4) | lowByte;
			bytelen++;
		}
		return bytelen;
	}

	/* 字节转16进制小写字符 */
	void ByteToHexLower(const unsigned char* source, char* dest, int sourceLen) {
		unsigned char highByte, lowByte;
		for (short i = 0; i < sourceLen; i++) {
			highByte = source[i] >> 4;
			lowByte = source[i] & 0x0f;
			if (highByte > 0x09) {
				dest[i * 2] = highByte + 0x57;
			}
			else {
				dest[i * 2] = highByte + 0x30;
			}
			if (lowByte > 0x09) {
				dest[i * 2 + 1] = lowByte + 0x57;
			}
			else {
				dest[i * 2 + 1] = lowByte + 0x30;
			}
		}
		return;
	}

	/* 字节转16进制小写字符 */
	void ByteToHexLower2(const unsigned char* source, char* dest, int sourceLen) {
		char tmp[3];
		for (short i = 0; i < sourceLen; i++)
		{
			sprintf_s(tmp, "%02x", (unsigned char)source[i]);
			memcpy(&dest[i * 2], tmp, 2);
		}
		return;
	}

	/* 16进制小写字符转字节，返回字节数 */
	int HexLowerToByte(const char* source, unsigned char* dest, int sourceLen) {
		int bytelen = 0;
		unsigned char highByte, lowByte;
		for (short i = 0; i < sourceLen; i += 2) {
			highByte = tolower(source[i]);
			lowByte = tolower(source[i + 1]);
			if (highByte > 0x59) {
				highByte -= 0x57;
			}
			else {
				highByte -= 0x30;
			}
			if (lowByte > 0x59) {
				lowByte -= 0x57;
			}
			else {
				lowByte -= 0x30;
			}
			dest[i / 2] = (highByte << 4) | lowByte;
			bytelen++;
		}
		return bytelen;
	}

	/* 字符串分割，支持多字符的分隔符 */
	void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
	{
		std::string::size_type pos1, pos2;
		pos1 = 0;
		pos2 = s.find(c);
		while (std::string::npos != pos2)
		{
			v.push_back(s.substr(pos1, pos2 - pos1));
			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		v.push_back(s.substr(pos1));
	}
}