#ifndef _WIN32_H_
#define _WIN32_H_

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif
	/* DEBUG */
	void win32_debug_set(bool b);
	bool win32_debug_get(void);
	/* TLS */
	const char *win32_tls_callback_reason_label(DWORD dwReason);
	/* Raw Memory */
	void  *win32_heap_malloc(HANDLE heap, size_t size);
	void  *win32_heap_realloc(HANDLE heap, void *block, size_t size);
	size_t win32_heap_memlen(HANDLE heap, void *block);
	bool win32_heap_free(HANDLE heap, void *block);
	/* Global Heap */
	HANDLE win32_global_heap(void);
	//void *win32_global_malloc(size_t size);
	#define win32_global_malloc(...) win32_heap_malloc(win32_global_heap(), ## __VA_ARGS__)
	//void *win32_global_realloc(void *block, size_t size);
	#define win32_global_realloc(...) win32_heap_realloc(win32_global_heap(), ## __VA_ARGS__)
	//size_t win32_global_memlen(void *block);
	#define win32_global_memlen(...) win32_heap_memlen(win32_global_heap(), ## __VA_ARGS__)
	//void win32_global_free(void *block);
	#define win32_global_free(...) win32_heap_free(win32_global_heap(), ## __VA_ARGS__)
	/* Thread Heap */
	HANDLE win32_thread_heap(void);
	//void *win32_thread_malloc(size_t size);
	#define win32_thread_malloc(...) win32_heap_malloc(win32_thread_heap(), ## __VA_ARGS__)
	//void *win32_thread_realloc(void *block, size_t size);
	#define win32_thread_realloc(...) win32_heap_realloc(win32_thread_heap(), ## __VA_ARGS__)
	//size_t win32_thread_memlen(void *block);
	#define win32_thread_memlen(...) win32_heap_memlen(win32_thread_heap(), ## __VA_ARGS__)
	//void win32_thread_free(void *block);
	#define win32_thread_free(...) win32_heap_free(win32_thread_heap(), ## __VA_ARGS__)
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus // ***** C++ only *****
#include <memory>
#include <string>
#include <vector>
#include "vardecl.h"
#include <QtCore>
namespace win32
{
	template<class T>
	class global_allocator : public std::allocator<T>
	{
	public:
		typedef size_t   size_type;
		typedef T*       pointer;
		typedef const T* const_pointer;
		global_allocator() { }
		template<class U>
		global_allocator(const global_allocator<U>& x) { UNUSED_PARAMETER(x); }
		template<class U>
		struct rebind { typedef global_allocator<U> other; };
		pointer allocate(size_type n, const_pointer hint = 0)
		{
			UNUSED_PARAMETER(hint);
			return (pointer)win32_global_malloc(sizeof(T) * n);
		}
		void deallocate(pointer ptr, size_type n)
		{
			UNUSED_PARAMETER(n);
			win32_global_free(ptr);
		}
	};
	typedef std::basic_string< char, std::char_traits<char>, global_allocator<char> > global_string;
	typedef std::basic_string< wchar_t, std::char_traits<wchar_t>, global_allocator<wchar_t> > global_wstring;
	template<typename T> class global_vector : public std::vector< T, global_allocator<T> > { };
    QDebug operator<<(QDebug out, const global_string& str);
};
#endif /* __cplusplus */

#endif /* _WIN32_H_ */
