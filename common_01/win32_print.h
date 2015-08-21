#ifndef _WIN32_PRINT_H_
#define _WIN32_PRINT_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
	/* ANSI */
	int win32_printfA(const char *format, ...);
	int win32_vprintfA(const char *format, va_list args);
	int win32_errorfA(const char *format, ...);
	int win32_verrorfA(const char *format, va_list args);
	/* UNICODE */
	int win32_printfW(const wchar_t *format, ...);
	int win32_vprintfW(const wchar_t *format, va_list args);
	int win32_errorfW(const wchar_t *format, ...);
	int win32_verrorfW(const wchar_t *format, va_list args);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// ***** C++ only *****
#include <string>
	std::wstring wstring_printf(size_t max_length, const wchar_t *format, ...);
	std::wstring wstring_printf(const wchar_t *format, ...);
#define WSTRING_PRINTF(...) wstring_printf(__VA_ARGS__).c_str()
#endif

#endif /* _WIN32_PRINT_H_ */
