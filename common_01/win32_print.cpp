#undef NDEBUG
#include <assert.h>

#include "win32_print.h"
#include <windows.h>
#include <stdio.h>
#include "vardecl.h"

#include <mutex>
#include <string>
#include <vector>

#include <QtCore>

#ifdef __GNUC__
/* http://stackoverflow.com/questions/3385515/static-assert-in-c */
#   define STATIC_ASSERT(X,Y) ({ extern int __attribute__((error("assertion failure: '" #X "' not true"))) compile_time_check(); ((X)?0:compile_time_check()),0; })
#else
#   define STATIC_ASSERT static_assert
#endif

/* for debug */
static void sync_putsA(const char* s)
{
	HANDLE hStdOutput;
	hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwWriteByte;
	WriteConsoleA(hStdOutput, s, lstrlenA(s), &dwWriteByte, NULL);
	WriteConsoleA(hStdOutput, "\n", 1, &dwWriteByte, NULL);
	return;
}

#define WIN32_PRINT_SYNC_ENTER_CSECT 1
#define WIN32_PRINT_SYNC_LEAVE_CSECT 0

#if 0x1
static void sync_enter_or_leave(DWORD dwEnterOrLeave)
{
	static bool s_initialized = false;
	static CRITICAL_SECTION s_crit_sec;
	if(!s_initialized)
	{
		InitializeCriticalSection(&s_crit_sec);
		s_initialized = true;
	}
	switch(dwEnterOrLeave)
	{
	case WIN32_PRINT_SYNC_ENTER_CSECT:
		EnterCriticalSection(&s_crit_sec);
		break;
	case WIN32_PRINT_SYNC_LEAVE_CSECT:
		LeaveCriticalSection(&s_crit_sec);
		break;
	default:
		assert(0);
		break;
	}
	return;
}
#else
static std::recursive_mutex *l_mutex = NULL;
static ALIGNED_ARRAY_DECL(char, l_mutex_memory, sizeof(*l_mutex), 16);
static void sync_enter_or_leave(DWORD dwEnterOrLeave)
{
	if(!l_mutex)
	{
		l_mutex = new (l_mutex_memory) std::recursive_mutex();
	}
	switch(dwEnterOrLeave)
	{
	case WIN32_PRINT_SYNC_ENTER_CSECT:
		l_mutex->lock();
		break;
	case WIN32_PRINT_SYNC_LEAVE_CSECT:
		l_mutex->unlock();
		break;
	default:
		assert(0);
		break;
	}
	return;
}
#endif

static int win32_write_consoleA(HANDLE hconsole, const char *format, va_list args)
{
	char v_buffer[1024+1];
	v_buffer[1024] = 0;
	int len = wvsprintfA((LPSTR)v_buffer, format, args); // Win32 API
	//int len = lstrlenA(v_buffer);
	for(int i=0; i<len; i++)
	{
		if(v_buffer[i]==0)
		{
			v_buffer[i]='@';
		}
	}
	DWORD dwWriteByte;
	WriteConsoleA(hconsole, v_buffer, len, &dwWriteByte, NULL);
    //OutputDebugStringA(v_buffer);
	return len;
}

/* extern */
int win32_printfA(const char *format, ...)
{
    //qDebug() << "win32_printfA(start)";
    sync_enter_or_leave(WIN32_PRINT_SYNC_ENTER_CSECT);
	va_list args;
	va_start(args, format);
	int len = win32_write_consoleA(GetStdHandle(STD_OUTPUT_HANDLE), format, args);
	va_end(args);
    sync_enter_or_leave(WIN32_PRINT_SYNC_LEAVE_CSECT);
    //qDebug() << "win32_printfA(end)";
    return len;
}

/* extern */
int win32_vprintfA(const char *format, va_list args)
{
	sync_enter_or_leave(WIN32_PRINT_SYNC_ENTER_CSECT);
	int len = win32_write_consoleA(GetStdHandle(STD_OUTPUT_HANDLE), format, args);
	sync_enter_or_leave(WIN32_PRINT_SYNC_LEAVE_CSECT);
	return len;
}

/* extern */
int win32_errorfA(const char *format, ...)
{
	sync_enter_or_leave(WIN32_PRINT_SYNC_ENTER_CSECT);
	va_list args;
	va_start(args, format);
	int len = win32_write_consoleA(GetStdHandle(STD_ERROR_HANDLE), format, args);
	va_end(args);
	sync_enter_or_leave(WIN32_PRINT_SYNC_LEAVE_CSECT);
	return len;
}

/* extern */
int win32_verrorfA(const char *format, va_list args)
{
	sync_enter_or_leave(WIN32_PRINT_SYNC_ENTER_CSECT);
	int len = win32_write_consoleA(GetStdHandle(STD_ERROR_HANDLE), format, args);
	sync_enter_or_leave(WIN32_PRINT_SYNC_LEAVE_CSECT);
	return len;
}

static int win32_write_consoleW(HANDLE hconsole, const wchar_t *format, va_list args)
{
	wchar_t v_buffer[1024+1];
	v_buffer[1024] = 0;
	int len = wvsprintfW((LPWSTR)v_buffer, format, args); // Win32 API
	//int len = lstrlenW(v_buffer);
	for(int i=0; i<len; i++)
	{
		if(v_buffer[i]==0)
		{
			v_buffer[i]=L'@';
		}
	}
	DWORD dwWriteByte;
	WriteConsoleW(hconsole, v_buffer, len, &dwWriteByte, NULL);
	return len;
}

/* extern */
int win32_printfW(const wchar_t *format, ...)
{
	sync_enter_or_leave(WIN32_PRINT_SYNC_ENTER_CSECT);
	va_list args;
	va_start(args, format);
	int len = win32_write_consoleW(GetStdHandle(STD_OUTPUT_HANDLE), format, args);
	va_end(args);
	sync_enter_or_leave(WIN32_PRINT_SYNC_LEAVE_CSECT);
	return len;
}

/* extern */
int win32_vprintfW(const wchar_t *format, va_list args)
{
	sync_enter_or_leave(WIN32_PRINT_SYNC_ENTER_CSECT);
	int len = win32_write_consoleW(GetStdHandle(STD_OUTPUT_HANDLE), format, args);
	sync_enter_or_leave(WIN32_PRINT_SYNC_LEAVE_CSECT);
	return len;
}

/* extern */
int win32_errorfW(const wchar_t *format, ...)
{
	sync_enter_or_leave(WIN32_PRINT_SYNC_ENTER_CSECT);
	va_list args;
	va_start(args, format);
	int len = win32_write_consoleW(GetStdHandle(STD_ERROR_HANDLE), format, args);
	va_end(args);
	sync_enter_or_leave(WIN32_PRINT_SYNC_LEAVE_CSECT);
	return len;
}

/* extern */
int win32_verrorfW(const wchar_t *format, va_list args)
{
	sync_enter_or_leave(WIN32_PRINT_SYNC_ENTER_CSECT);
	int len = win32_write_consoleW(GetStdHandle(STD_ERROR_HANDLE), format, args);
	sync_enter_or_leave(WIN32_PRINT_SYNC_LEAVE_CSECT);
	return len;
}

static std::wstring wstring_printf_body(size_t max_length, const wchar_t *format, va_list argptr)
{
	std::vector<wchar_t> v_buffer;
	v_buffer.resize(max_length+1);
	STATIC_ASSERT(sizeof(v_buffer[0])==sizeof(wchar_t), "A message");
	memset(&v_buffer[0], 0, sizeof(v_buffer[0]) * v_buffer.size());
	int v_len = _vsnwprintf(&v_buffer[0], max_length, format, argptr); // CRT API
	for(int i=0; i<v_len; i++)
	{
		if(v_buffer[i]==0)
		{
			v_buffer[i]=L'@';
		}
	}
	std::wstring v_result = &v_buffer[0];
	return v_result;
}

/* extern */
std::wstring wstring_printf(size_t max_length, const wchar_t *format, ...)
{
	va_list args;
	va_start(args, format);
	std::wstring ret = wstring_printf_body(max_length, format, args);
	va_end(args);
	return ret;
}

/* extern */
std::wstring wstring_printf(const wchar_t *format, ...)
{
	va_list args;
	va_start(args, format);
	std::wstring ret = wstring_printf_body(1024, format, args);
	va_end(args);
	return ret;
}
