#undef NDEBUG
#include <assert.h>

#include "win32_vector.h"
#include "win32.h"
#include "win32_print.h"
#include <stdint.h>

#define DBG(format, ...) win32_errorfA("[WIN32_VECTOR] " format "\n", ## __VA_ARGS__)

/* extern */
void   win32_vector_clear(HANDLE heap, DWORD width, char **vector)
{
	win32_vector_resize(heap, width, vector, 0);
}

/* extern */
DWORD  win32_vector_size(HANDLE heap, DWORD width, char **vector)
{
	assert(width > 0);
	if(!vector) return 0;
	if(!*vector) return 0;
	size_t v_byte_size = win32_heap_memlen(heap, *vector);
	DWORD v_width_size = v_byte_size / width;
	return v_width_size;
}

/* extern */
void   win32_vector_resize(HANDLE heap, DWORD width, char **vector, DWORD size)
{
	assert(width > 0);
	if(!vector) return;
	if(!size)
	{
		if(*vector)
		{
			win32_heap_free(heap, *vector);
			*vector = NULL;
		}
		return;
	}
	if(!*vector)
	{
		*vector = (char *)win32_heap_malloc(heap, width*size);
	}
	else
	{
		*vector = (char *)win32_heap_realloc(heap, *vector, width*size);
	}
}

/* extern */
void   win32_vector_reserve(HANDLE heap, DWORD width, char **vector, DWORD size)
{
	assert(width > 0);
	if(!vector) return;
	DWORD v_old_size = win32_vector_size(heap, width, vector);
	if(v_old_size >= size) return;
	win32_vector_resize(heap, width, vector, size);
}

/* extern */
void   win32_vector_get(HANDLE heap, DWORD width, char **vector, DWORD pos, char *val)
{
	assert(width > 0);
	if(!val) return;
	ZeroMemory(val, width);
	if(!vector) return;
	if(!*vector) return;
	DWORD v_old_size = win32_vector_size(heap, width, vector);
	if(pos >= v_old_size) return;
	char *v_src = &(*vector)[width*pos];
	CopyMemory(val, v_src, width);
}

/* extern */
void   win32_vector_set(HANDLE heap, DWORD width, char **vector, DWORD pos, char *val)
{
	assert(width > 0);
	if(!vector) return;
	win32_vector_reserve(heap, width, vector, pos+1);
	char *v_dest = &(*vector)[width*pos];
	if(val)
	{
		CopyMemory(v_dest, val, width);
	}
	else
	{
		ZeroMemory(v_dest, width);
	}
}

/* extern */
void   win32_vector_push_back(HANDLE heap, DWORD width, char **vector, char *val)
{
	assert(width > 0);
	if(!vector) return;
	DWORD v_old_size = win32_vector_size(heap, width, vector);
	win32_vector_set(heap, width, vector, v_old_size, val);
}

/* extern */
void   win32_vector_erase(HANDLE heap, DWORD width, char **vector, DWORD pos, char *val)
{
	assert(width > 0);
	if(val) ZeroMemory(val, width);
	if(!vector) return;
	if(!*vector) return;
	DWORD v_old_size = win32_vector_size(heap, width, vector);
	if(pos >= v_old_size) return;
	if(val)
	{
		char *v_src = &(*vector)[width*pos];
		CopyMemory(val, v_src, width);
	}
	for(int64_t i=pos+1; i<v_old_size; i++)
	{
		CopyMemory(&(*vector)[width*(i-1)], &(*vector)[width*i], width);
	}
	win32_vector_resize(heap, width, vector, v_old_size-1);
}

/* extern */
void   win32_vector_insert(HANDLE heap, DWORD width, char **vector, DWORD pos, char *val)
{
	assert(width > 0);
	if(!vector) return;
	win32_vector_reserve(heap, width, vector, pos+1);
	DWORD v_old_size = win32_vector_size(heap, width, vector);
	win32_vector_resize(heap, width, vector, v_old_size+1);
	for(int64_t i=v_old_size; i>=pos; i--)
	{
		CopyMemory(&(*vector)[width*(i+1)], &(*vector)[width*i], width);
	}
	char *v_dest = &(*vector)[width*pos];
	if(val)
	{
		CopyMemory(v_dest, val, width);
	}
	else
	{
		ZeroMemory(v_dest, width);
	}
}

/* extern */
void   win32_thread_char_clear(char **vector)
{
	win32_thread_vector_resize(sizeof(**vector), (char **)vector, 0);
}

/* extern */
DWORD   win32_thread_char_size(char **vector)
{
	return win32_thread_vector_size(sizeof(**vector), (char **)vector);
}

/* extern */
void   win32_thread_char_resize(char **vector, DWORD size)
{
	win32_thread_vector_resize(sizeof(**vector), (char **)vector, size);
}

/* extern */
void   win32_thread_char_reserve(char **vector, DWORD size)
{
	win32_thread_vector_reserve(sizeof(**vector), (char **)vector, size);
}

/* extern */
char   win32_thread_char_get(char **vector, DWORD pos)
{
	char val;
	win32_thread_vector_get(sizeof(**vector), (char **)vector, pos, (char *)&val);
	return val;
}

/* extern */
void   win32_thread_char_set(char **vector, DWORD pos, char val)
{
	win32_thread_vector_set(sizeof(**vector), (char **)vector, pos, (char *)&val);
}

/* extern */
void   win32_thread_char_push_back(char **vector, char val)
{
	win32_thread_vector_push_back(sizeof(**vector), (char **)vector, (char *)&val);
}

/* extern */
char   win32_thread_char_erase(char **vector, DWORD pos)
{
	char val;
	win32_thread_vector_erase(sizeof(**vector), (char **)vector, pos, (char *)&val);
	return val;
}

/* extern */
void   win32_thread_char_insert(char **vector, DWORD pos, char val)
{
	win32_thread_vector_insert(sizeof(**vector), (char **)vector, pos, (char *)&val);
}

/* extern */
void    win32_thread_wchar_clear(wchar_t **vector)
{
	win32_thread_vector_clear(sizeof(**vector), (char **)vector);
}

/* extern */
DWORD   win32_thread_wchar_size(wchar_t **vector)
{
	return win32_thread_vector_size(sizeof(**vector), (char **)vector);
}

/* extern */
void    win32_thread_wchar_resize(wchar_t **vector, DWORD size)
{
	win32_thread_vector_resize(sizeof(**vector), (char **)vector, size);
}

/* extern */
void    win32_thread_wchar_reserve(wchar_t **vector, DWORD size)
{
	win32_thread_vector_reserve(sizeof(**vector), (char **)vector, size);
}

/* extern */
wchar_t win32_thread_wchar_get(wchar_t **vector, DWORD pos)
{
	wchar_t val;
	win32_thread_vector_get(sizeof(**vector), (char **)vector, pos, (char *)&val);
	return val;
}

/* extern */
void    win32_thread_wchar_set(wchar_t **vector, DWORD pos, wchar_t val)
{
	win32_thread_vector_set(sizeof(**vector), (char **)vector, pos, (char *)&val);
}

/* extern */
void    win32_thread_wchar_push_back(wchar_t **vector, wchar_t val)
{
	win32_thread_vector_push_back(sizeof(**vector), (char **)vector, (char *)&val);
}

/* extern */
wchar_t win32_thread_wchar_erase(wchar_t **vector, DWORD pos)
{
	wchar_t val;
	win32_thread_vector_erase(sizeof(**vector), (char **)vector, pos, (char *)&val);
	return val;
}

/* extern */
void    win32_thread_wchar_insert(wchar_t **vector, DWORD pos, wchar_t val)
{
	win32_thread_vector_insert(sizeof(**vector), (char **)vector, pos, (char *)&val);
}
