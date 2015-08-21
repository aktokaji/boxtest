#undef NDEBUG
#include <assert.h>

#include "win32.h"
#include "win32_print.h"
#include "vardecl.h"
#include <stdint.h>

#define DBG(format, ...) _debug_printA("[WIN32] " format "\n", ## __VA_ARGS__)

#define WIN32_GLOBAL_HEAP_INITIAL_SIZE 4096
#define WIN32_THREAD_HEAP_INITIAL_SIZE 4096

static bool l_debug_flag = false;
static HANDLE l_global_heap = NULL;
static size_t l_global_heap_alloc_total = 0;
static TLS_VARIABLE_DECL HANDLE l_thread_heap = NULL;
static TLS_VARIABLE_DECL size_t l_thread_heap_alloc_total = 0;

static void _debug_printA(const char *format, ...)
{
	if(!l_debug_flag) return;
	va_list args;
	va_start(args, format);
	//win32_vprintfA(format, args);
	win32_verrorfA(format, args);
	va_end(args);
}

static void win32_on_detach(void)
{
	DBG("win32_on_detach()");
	if(l_thread_heap)
	{
		HeapDestroy(
			l_thread_heap // HANDLE hHeap   // ヒープのハンドル
		);
		l_thread_heap = NULL;
		DBG("  l_thread_heap_alloc_total=%d (0x%08x)", l_thread_heap_alloc_total, l_thread_heap_alloc_total);
	}
}

struct win32_module
{
	HMODULE BaseAddress;
	DWORD   SizeOfImage;
	LPVOID  EntryPoint;
	LONG    TlsIndex = -1;
	WCHAR   FileFullPath[_MAX_PATH+1];
	WCHAR   FileLocalName[_MAX_PATH+1];
};

#if 0x0
static
void win32_module_vector_clear(win32_module **vector)
{
	win32_thread_vector_resize(sizeof(**vector), (char **)vector, 0);
}

static
DWORD win32_module_vector_size(win32_module **vector)
{
	return win32_thread_vector_size(sizeof(**vector), (char **)vector);
}

static
void win32_module_vector_push_back(win32_module **vector, win32_module *val)
{
	win32_thread_vector_push_back(sizeof(**vector), (char **)vector, (char *)val);
}
#endif

#if 0x0
static
//std::vector<WineModuleInfo> WineEnumModuleInfos(void)
void win32_find_implicit_tls(void)
{
	//std::vector<WineModuleInfo> v_result;
	HMODULE hMods[1024];
	DWORD cbNeeded;
	HANDLE hProcess = GetCurrentProcess();
	if(EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for ( unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
		{
			MODULEINFO mi;
			GetModuleInformation(hProcess, hMods[i], &mi, sizeof(MODULEINFO));
			WineModuleInfo wmi;
			wmi.BaseAddress = (HMODULE)mi.lpBaseOfDll;
			//WINE_DBG("wmi.BaseAddress=0x%08x", wmi.BaseAddress);
			wmi.SizeOfImage = mi.SizeOfImage;
			wmi.EntryPoint  = mi.EntryPoint;
			wmi.FileFullPath = WineGetModuleFileName(wmi.BaseAddress);
			WineSplitPath v_split_path(wmi.FileFullPath);
			wmi.FileLocalName = v_split_path.FName + v_split_path.Ext;
			ULONG v_size;
			PIMAGE_TLS_DIRECTORY v_tls_dir = (PIMAGE_TLS_DIRECTORY)ImageDirectoryEntryToData(
				(PVOID)wmi.BaseAddress,
				TRUE,
				IMAGE_DIRECTORY_ENTRY_TLS,
				&v_size
			);
			if(v_tls_dir)
			{
				//WINE_DBG("WineEnumModuleInfos(): AddressOfIndex=0x%08x", v_tls_dir->AddressOfIndex);
				assert(v_tls_dir->AddressOfIndex);
				wmi.TlsIndex = (LONG)((DWORD *)v_tls_dir->AddressOfIndex)[0];
			}
			else
			{
				wmi.TlsIndex = -1;
			}
			v_result.push_back(wmi);
		}
	}
	return v_result;
}
#endif

/* extern */
void win32_debug_set(bool b)
{
	l_debug_flag = b;
}

/* extern */
bool win32_debug_get(void)
{
	return l_debug_flag;
}

/* extern */
HANDLE win32_global_heap(void)
{
	if(!l_global_heap)
	{
		l_global_heap = 
			HeapCreate(
				0, // DWORD flOptions, // ヒープ割り当て方法の属性
				WIN32_GLOBAL_HEAP_INITIAL_SIZE, // SIZE_T dwInitialSize, // 初期のヒープサイズ
				0 // SIZE_T dwMaximumSize // 最大ヒープサイズ
			);
	}
	return l_global_heap;
}

/* extern */
HANDLE win32_thread_heap(void)
{
	if(!l_thread_heap)
	{
		l_thread_heap = 
			HeapCreate(
				HEAP_NO_SERIALIZE, // DWORD flOptions, // ヒープ割り当て方法の属性
				WIN32_THREAD_HEAP_INITIAL_SIZE, // SIZE_T dwInitialSize, // 初期のヒープサイズ
				0 // SIZE_T dwMaximumSize // 最大ヒープサイズ
			);
	}
	return l_thread_heap;
}

/* extern */
void *win32_heap_malloc(HANDLE heap, size_t size)
{
	LPVOID v_block = HeapAlloc(
	  heap, // HANDLE hHeap, // プライベートヒープブロックのハンドル
	  HEAP_ZERO_MEMORY, // DWORD dwFlags, // ヒープの割り当て方法の制御
	  size // SIZE_T dwBytes // 割り当てたいバイト数
	);
	if(v_block)
	{
		if(heap==l_global_heap)
		{
			l_global_heap_alloc_total += size;
		}
		if(heap==l_thread_heap)
		{
			l_thread_heap_alloc_total += size;
		}
	}
	return v_block;
}

/* extern */
void *win32_heap_realloc(HANDLE heap, void *block, size_t size)
{
	if(!block)
	{
		return win32_heap_malloc(heap, size);
	}
	size_t v_old_size = win32_heap_memlen(heap, block);
	LPVOID v_block = HeapReAlloc(
		heap, // HANDLE hHeap,   // ヒープブロックのハンドル
		HEAP_ZERO_MEMORY, // DWORD dwFlags,  // ヒープ再割り当てのオプション
		block, // LPVOID lpMem,   // 再割り当てしたいメモリへのポインタ
		size // SIZE_T dwBytes  // 再割り当てしたいバイト数
	);
	if(v_block)
	{
		if(heap==l_global_heap)
		{
			l_global_heap_alloc_total -= v_old_size;
			l_global_heap_alloc_total += size;
		}
		if(heap==l_thread_heap)
		{
			l_thread_heap_alloc_total -= v_old_size;
			l_thread_heap_alloc_total += size;
		}
	}
	return v_block;
}

/* extern */
size_t win32_heap_memlen(HANDLE heap, void *block)
{
	if(!block) return 0;
	DWORD v_len = HeapSize(
		heap, // HANDLE hHeap,  // ヒープのハンドル
		0, // DWORD dwFlags, // ヒープサイズに関するオプション
		block // LPCVOID lpMem  // メモリへのポインタ
	);
	return v_len;
}

/* extern */
bool win32_heap_free(HANDLE heap, void *block)
{
	size_t v_old_size = win32_heap_memlen(heap, block);
	BOOL v_success = HeapFree(
		heap, // HANDLE hHeap, // ヒープのハンドル
		0, // DWORD dwFlags, // ヒープ解放オプション
		block // LPVOID lpMem // メモリへのポインタ
	);
	if(v_success)
	{
		if(heap==l_global_heap)
		{
			l_global_heap_alloc_total -= v_old_size;
		}
		if(heap==l_thread_heap)
		{
			l_thread_heap_alloc_total -= v_old_size;
		}
	}
	return !!v_success;
}

QDebug win32::operator<<(QDebug out, const win32::global_string& str)
{
    QString v_str = QString::fromLocal8Bit(str.c_str());
    out << v_str;
    return out;
}

//static const char *tls_callback_reason_label(DWORD dwReason)
const char *win32_tls_callback_reason_label(DWORD dwReason)
{
#ifndef DLL_PROCESS_VERIFIER
#define DLL_PROCESS_VERIFIER 4
#endif
	switch(dwReason){
	case DLL_PROCESS_ATTACH:
		return "DLL_PROCESS_ATTACH";
	case DLL_PROCESS_DETACH:
		return "DLL_PROCESS_DETACH";
	case DLL_THREAD_ATTACH:
		return "DLL_THREAD_ATTACH";
	case DLL_THREAD_DETACH:
		return "DLL_THREAD_DETACH";
	case DLL_PROCESS_VERIFIER:
		return "DLL_PROCESS_VERIFIER";
	}
	return "UNKNOWN";
}

#define TLS_CALLBACK_SECT ".CRT$XLX"
static void NTAPI win32_tls_callback(PVOID hModule, DWORD dwReason, PVOID pReserved)
{
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(pReserved);
	DBG("win32_tls_callback(%s): dwReason=0x%08x (%s)", TLS_CALLBACK_SECT, dwReason, win32_tls_callback_reason_label(dwReason));
	switch(dwReason){
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		//win32_on_detach();
		DBG("  l_thread_heap_alloc_total=%d (0x%08x)", l_thread_heap_alloc_total, l_thread_heap_alloc_total);
		DBG("  l_global_heap_alloc_total=%d (0x%08x)", l_global_heap_alloc_total, l_global_heap_alloc_total);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		win32_on_detach();
		break;
	default:
		break;
	}
}
TLS_CALLBACK_DECL(TLS_CALLBACK_SECT, __win32_tls_callback__, win32_tls_callback);
