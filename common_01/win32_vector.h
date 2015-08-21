#ifndef _WIN32_VECTOR_H_
#define _WIN32_VECTOR_H_

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif
	/* Vector */
	void   win32_vector_clear(HANDLE heap, DWORD width, char **vector);
	DWORD  win32_vector_size(HANDLE heap, DWORD width, char **vector);
	void   win32_vector_resize(HANDLE heap, DWORD width, char **vector, DWORD size);
	void   win32_vector_reserve(HANDLE heap, DWORD width, char **vector, DWORD size);
	void   win32_vector_get(HANDLE heap, DWORD width, char **vector, DWORD pos, char *val);
	void   win32_vector_set(HANDLE heap, DWORD width, char **vector, DWORD pos, char *val);
	void   win32_vector_push_back(HANDLE heap, DWORD width, char **vector, char *val);
	void   win32_vector_erase(HANDLE heap, DWORD width, char **vector, DWORD pos, char *val);
	void   win32_vector_insert(HANDLE heap, DWORD width, char **vector, DWORD pos, char *val);

	//void   win32_global_vector_clear(DWORD width, char **vector);
	#define win32_global_vector_clear(...) win32_vector_clear(win32_global_heap(), ## __VA_ARGS__)
	//DWORD  win32_global_vector_size(DWORD width, char **vector);
	#define win32_global_vector_size(...) win32_vector_size(win32_global_heap(), ## __VA_ARGS__)
	//void   win32_global_vector_resize(DWORD width, char **vector, DWORD size);
	#define win32_global_vector_resize(...) win32_vector_resize(win32_global_heap(), ## __VA_ARGS__)
	//void   win32_global_vector_reserve(DWORD width, char **vector, DWORD size);
	#define win32_global_vector_reserve(...) win32_vector_reserve(win32_global_heap(), ## __VA_ARGS__)
	//void   win32_global_vector_get(DWORD width, char **vector, DWORD pos, char *val);
	#define win32_global_vector_get(...) win32_vector_get(win32_global_heap(), ## __VA_ARGS__)
	//void   win32_global_vector_set(DWORD width, char **vector, DWORD pos, char *val);
	#define win32_global_vector_set(...) win32_vector_set(win32_global_heap(), ## __VA_ARGS__)
	//void   win32_global_vector_push_back(DWORD width, char **vector, char *val);
	#define win32_global_vector_push_back(...) win32_vector_push_back(win32_global_heap(), ## __VA_ARGS__)
	//void   win32_global_vector_erase(DWORD width, char **vector, DWORD pos, char *val);
	#define win32_global_vector_erase(...) win32_vector_erase(win32_global_heap(), ## __VA_ARGS__)
	//void   win32_global_vector_insert(DWORD width, char **vector, DWORD pos, char *val);
	#define win32_global_vector_insert(...) win32_vector_insert(win32_global_heap(), ## __VA_ARGS__)

	//void   win32_thread_vector_clear(DWORD width, char **vector);
	#define win32_thread_vector_clear(...) win32_vector_clear(win32_thread_heap(), ## __VA_ARGS__)
	//DWORD  win32_thread_vector_size(DWORD width, char **vector);
	#define win32_thread_vector_size(...) win32_vector_size(win32_thread_heap(), ## __VA_ARGS__)
	//void   win32_thread_vector_resize(DWORD width, char **vector, DWORD size);
	#define win32_thread_vector_resize(...) win32_vector_resize(win32_thread_heap(), ## __VA_ARGS__)
	//void   win32_thread_vector_reserve(DWORD width, char **vector, DWORD size);
	#define win32_thread_vector_reserve(...) win32_vector_reserve(win32_thread_heap(), ## __VA_ARGS__)
	//void   win32_thread_vector_get(DWORD width, char **vector, DWORD pos, char *val);
	#define win32_thread_vector_get(...) win32_vector_get(win32_thread_heap(), ## __VA_ARGS__)
	//void   win32_thread_vector_set(DWORD width, char **vector, DWORD pos, char *val);
	#define win32_thread_vector_set(...) win32_vector_set(win32_thread_heap(), ## __VA_ARGS__)
	//void   win32_thread_vector_push_back(DWORD width, char **vector, char *val);
	#define win32_thread_vector_push_back(...) win32_vector_push_back(win32_thread_heap(), ## __VA_ARGS__)
	//void   win32_thread_vector_erase(DWORD width, char **vector, DWORD pos, char *val);
	#define win32_thread_vector_erase(...) win32_vector_erase(win32_thread_heap(), ## __VA_ARGS__)
	//void   win32_thread_vector_insert(DWORD width, char **vector, DWORD pos, char *val);
	#define win32_thread_vector_insert(...) win32_vector_insert(win32_thread_heap(), ## __VA_ARGS__)

	/* char vector */
	void   win32_thread_char_clear(char **vector);
	DWORD  win32_thread_char_size(char **vector);
	void   win32_thread_char_resize(char **vector, DWORD size);
	void   win32_thread_char_reserve(char **vector, DWORD size);
	char   win32_thread_char_get(char **vector, DWORD pos);
	void   win32_thread_char_set(char **vector, DWORD pos, char val);
	void   win32_thread_char_push_back(char **vector, char val);
	char   win32_thread_char_erase(char **vector, DWORD pos);
	void   win32_thread_char_insert(char **vector, DWORD pos, char val);
	/* wchar_t vector */
	void    win32_thread_wchar_clear(wchar_t **vector);
	DWORD   win32_thread_wchar_size(wchar_t **vector);
	void    win32_thread_wchar_resize(wchar_t **vector, DWORD size);
	void    win32_thread_wchar_reserve(wchar_t **vector, DWORD size);
	wchar_t win32_thread_wchar_get(wchar_t **vector, DWORD pos);
	void    win32_thread_wchar_set(wchar_t **vector, DWORD pos, wchar_t val);
	void    win32_thread_wchar_push_back(wchar_t **vector, wchar_t val);
	wchar_t win32_thread_wchar_erase(wchar_t **vector, DWORD pos);
	void    win32_thread_wchar_insert(wchar_t **vector, DWORD pos, wchar_t val);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus // ***** C++ only *****
#endif /* __cplusplus */

#endif /* _WIN32_VECTOR_H_ */
