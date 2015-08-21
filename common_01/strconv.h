#ifndef _STRCONV_H_
#define _STRCONV_H_

#include <windows.h>
#include <string>

inline std::wstring cp_to_wide(const std::string &s, UINT codepage)
{
	int slength = (int)s.length();
	int len = MultiByteToWideChar(codepage, 0, s.c_str(), slength, 0, 0); 
	std::wstring result(len, L'\0');
	MultiByteToWideChar(codepage, 0, s.c_str(), slength, &result[0], len);
	return result;
}
inline std::string wide_to_cp(const std::wstring &s, UINT codepage)
{
	int slength = (int)s.length();
	int len = WideCharToMultiByte(codepage, 0, s.c_str(), slength, 0, 0, 0, 0); 
	std::string result(len, '\0');
	WideCharToMultiByte(codepage, 0, s.c_str(), slength, &result[0], len, 0, 0); 
	return result;
}
inline std::string cp_to_utf8(const std::string &s, UINT codepage)
{
	std::wstring wide = cp_to_wide(s, codepage);
	return wide_to_cp(wide, CP_UTF8);
}
inline std::string utf8_to_cp(const std::string &s, UINT codepage)
{
	std::wstring wide = cp_to_wide(s, CP_UTF8);
	return wide_to_cp(wide, codepage);
}

static std::wstring utf8_to_wide(const std::string &s)
{
	return cp_to_wide(s, CP_UTF8);
}
#define UTF8_TO_WIDE(s) utf8_to_wide(s).c_str()
static std::string wide_to_utf8(const std::wstring &s)
{
	return wide_to_cp(s, CP_UTF8);
}
#define WIDE_TO_UTF8(s) wide_to_utf8(s).c_str()

static std::wstring ansi_to_wide(const std::string &s)
{
	return cp_to_wide(s, CP_ACP);
}
#define ANSI_TO_WIDE(s) ansi_to_wide(s).c_str()
static std::string wide_to_ansi(const std::wstring &s)
{
	return wide_to_cp(s, CP_ACP);
}
#define WIDE_TO_ANSI(s) wide_to_ansi(s).c_str()

static std::wstring sjis_to_wide(const std::string &s)
{
	return cp_to_wide(s, 932);
}
#define SJIS_TO_WIDE(s) sjis_to_wide(s).c_str()
static std::string wide_to_sjis(const std::wstring &s)
{
	return wide_to_cp(s, 932);
}
#define WIDE_TO_SJIS(s) wide_to_sjis(s).c_str()

static std::string ansi_to_utf8(const std::string &s)
{
	return cp_to_utf8(s, CP_ACP);
}
#define ANSI_TO_UTF8(s) ansi_to_utf8(s).c_str()
static std::string utf8_to_ansi(const std::string &s)
{
	return utf8_to_cp(s, CP_ACP);
}
#define UTF8_TO_ANSI(s) utf8_to_ansi(s).c_str()

static std::string sjis_to_utf8(const std::string &s)
{
	return cp_to_utf8(s, 932);
}
#define SJIS_TO_UTF8(s) sjis_to_utf8(s).c_str()
static std::string utf8_to_sjis(const std::string &s)
{
	return utf8_to_cp(s, 932);
}
#define UTF8_TO_SJIS(s) utf8_to_sjis(s).c_str()

#endif /* _STRCONV_H_ */
