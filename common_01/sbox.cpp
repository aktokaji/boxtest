#undef NDEBUG
#include <assert.h>

#include "sbox.h"
#include "strconv.h"
#include <imagehlp.h> // ImageDirectoryEntryToData()

#define SBOX_DBG(format, ...) win32_printf("[SBOX] " format "\n", ## __VA_ARGS__)

/*
    typedef struct _IMAGE_TLS_DIRECTORY32 {
      DWORD StartAddressOfRawData;
      DWORD EndAddressOfRawData;
      DWORD AddressOfIndex;
      DWORD AddressOfCallBacks;
      DWORD SizeOfZeroFill;
      DWORD Characteristics;
    } IMAGE_TLS_DIRECTORY32;
    typedef IMAGE_TLS_DIRECTORY32 *PIMAGE_TLS_DIRECTORY32;
*/

#if 0x0
static void sbox_puts(const char* s)
{
	HANDLE hStdOutput;
	hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwWriteByte;
	WriteConsoleA(hStdOutput, s, lstrlenA(s), &dwWriteByte, NULL);
	WriteConsoleA(hStdOutput, "\n", 1, &dwWriteByte, NULL);
	return;
}
#endif

SBOX_PROCESS::SBOX_PROCESS()
{
	f_hmodule = GetModuleHandleW(NULL);
	f_teb = WineNtCurrentTeb();
	f_root_module_list = WineEnumModuleInfos();
	SBOX_DBG("f_root_module_list.size()=%u", f_root_module_list.size());
	LONG v_max_tls_index = -1;
	for(ULONG i=0; i<f_root_module_list.size(); i++)
	{
		WineModuleInfo &v_wmi = f_root_module_list[i];
		SBOX_DBG("  f_root_module_list[%u]=%s", i, WIDE_TO_ANSI(f_root_module_list[i].ToString()));
		if(v_wmi.TlsIndex > v_max_tls_index)
		{
			v_max_tls_index = v_wmi.TlsIndex;
		}
	}
	f_num_implicit_tls = v_max_tls_index + 1;
	//f_num_extended_tls = 64; // FIXME
	f_num_extended_tls = 16; // FIXME
	SBOX_DBG("SBOX_PROCESS created: 0x%08x (IMPLICIT_TLS=%u)", f_teb, f_num_implicit_tls);
}
SBOX_PROCESS::~SBOX_PROCESS()
{
	if(f_root_thread)
	{
		delete f_root_thread;
		f_root_thread = NULL;
	}
	SBOX_DBG("SBOX_PROCESS deleted: 0x%08x (IMPLICIT_TLS=%u)", f_teb, f_num_implicit_tls);
}
bool SBOX_PROCESS::register_module(HMODULE hModule)
{
	assert(sizeof(hModule)==4);
	SBOX_DBG("SBOX_PROCESS::register_module(0x%08x)", hModule);
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		SBOX_DBG("SBOX_PROCESS::register_module(0x%08x): DOS Signature invalid", hModule);
		return false;
	}
	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hModule) + pDosHeader->e_lfanew);
	if(pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
		SBOX_DBG("SBOX_PROCESS::register_module(0x%08x): NT Signature mismatch", hModule);
		return false;
	}
	//std::list<SBOX_MODULE> f_sbox_module_list;
	SBOX_MODULE v_sbox_module;
	v_sbox_module.f_hmodule = hModule;
	ULONG v_tls_dir_size;
	PIMAGE_TLS_DIRECTORY v_tls_dir = (PIMAGE_TLS_DIRECTORY)ImageDirectoryEntryToData(
		(PVOID)hModule,
		TRUE,
		IMAGE_DIRECTORY_ENTRY_TLS,
		&v_tls_dir_size
	);
	if(v_tls_dir)
	{
		assert(v_tls_dir->AddressOfIndex);
		v_sbox_module.f_tls_dir = v_tls_dir;
		//((DWORD *)v_tls_dir->AddressOfIndex)[0] = f_num_implicit_tls + f_sbox_module_list.size();
		//v_sbox_module.f_tls_index = (LONG)((DWORD *)v_tls_dir->AddressOfIndex)[0];
		v_sbox_module.f_tls_index = f_num_implicit_tls + f_sbox_module_list.size();
		((DWORD *)v_tls_dir->AddressOfIndex)[0] = v_sbox_module.f_tls_index;
		SBOX_DBG("SBOX_PROCESS::register_module(): v_sbox_module.f_tls_index=%d", v_sbox_module.f_tls_index);
		if(v_tls_dir->StartAddressOfRawData)
		{
			SBOX_DBG("SBOX_PROCESS::register_module(): v_tls_dir->StartAddressOfRawData=0x%08x", v_tls_dir->StartAddressOfRawData);
			SBOX_DBG("SBOX_PROCESS::register_module(): v_tls_dir->EndAddressOfRawData=0x%08x", v_tls_dir->EndAddressOfRawData);
			SBOX_DBG("SBOX_PROCESS::register_module(): v_tls_dir->SizeOfZeroFill=%u", v_tls_dir->SizeOfZeroFill);
			size_t v_tls_raw_data_len = v_tls_dir->EndAddressOfRawData - v_tls_dir->StartAddressOfRawData;
			//size_t v_tls_raw_data_len = v_tls_dir->EndAddressOfRawData - v_tls_dir->StartAddressOfRawData + 1; // FIXME
			SBOX_DBG("SBOX_PROCESS::register_module(): v_tls_raw_data_len=%u", v_tls_raw_data_len);
			assert(v_tls_raw_data_len > 0);
			assert(sizeof(BYTE)==1);
			v_sbox_module.f_tls_raw_data.resize(v_tls_raw_data_len + v_tls_dir->SizeOfZeroFill);
			SBOX_DBG("SBOX_PROCESS::register_module(): v_sbox_module.f_tls_raw_data.size()=%u", v_sbox_module.f_tls_raw_data.size());
			SBOX_DBG("SBOX_PROCESS::register_module(): &v_sbox_module.f_tls_raw_data[0]=0x%08x", &v_sbox_module.f_tls_raw_data[0]);
			memcpy(&v_sbox_module.f_tls_raw_data[0], (const void *)v_tls_dir->StartAddressOfRawData, v_tls_raw_data_len);
			memset(&v_sbox_module.f_tls_raw_data[v_tls_raw_data_len], 0, v_tls_dir->SizeOfZeroFill);
		}
		if(v_tls_dir->AddressOfCallBacks)
		{
			SBOX_TLS_CALLBACK_PROC *v_callback_array = (SBOX_TLS_CALLBACK_PROC *)v_tls_dir->AddressOfCallBacks;
			while(*v_callback_array)
			{
				SBOX_DBG("SBOX_PROCESS::register_module(): *v_callback_array=0x%08x", *v_callback_array);
				v_sbox_module.f_tls_callback_list.push_back(*v_callback_array);
				v_callback_array++;
			}
		}
	}
	SBOX_DBG("SBOX_PROCESS::register_module(): v_sbox_module.f_tls_callback_list.size()=%u", v_sbox_module.f_tls_callback_list.size());
	f_sbox_module_list.push_back(v_sbox_module);
	SBOX_DBG("SBOX_PROCESS::register_module(): f_sbox_module_list.size()=%u", f_sbox_module_list.size());
	return true;
}
void SBOX_PROCESS::alloc_main_thread()
{
	assert(f_teb == WineNtCurrentTeb());
	g_sbox_thread = new SBOX_THREAD();
	f_root_thread = g_sbox_thread;
}
SBOX_PROCESS *g_sbox_process = new SBOX_PROCESS();

SBOX_THREAD::SBOX_THREAD()
{
	assert(g_sbox_process);
	assert(g_sbox_process->f_teb);
	f_teb = WineNtCurrentTeb();
	assert(f_teb);
	if(f_teb == g_sbox_process->f_teb)
	{
		f_is_root = true;
	}
	else
	{
		f_is_root = false;
	}
	//assert(f_teb->ThreadLocalStoragePointer); // f_teb->ThreadLocalStoragePointer might be NULL;
	f_orig_tlsp = f_teb->ThreadLocalStoragePointer;
	f_sbox_tlsp = NULL;
	assert(g_sbox_process->f_num_extended_tls >= g_sbox_process->f_num_implicit_tls);
	size_t v_byte_size = sizeof(LPVOID) * g_sbox_process->f_num_extended_tls;
	f_sbox_tlsp = (PVOID)_aligned_malloc(v_byte_size, 16);
	memset(f_sbox_tlsp, 0, v_byte_size);
	if(g_sbox_process->f_num_implicit_tls)
	{
		assert(f_orig_tlsp);
		for(DWORD i=0; i<g_sbox_process->f_num_implicit_tls; i++)
		{
			((DWORD *)f_sbox_tlsp)[i] = ((DWORD *)f_orig_tlsp)[i];
		}
	}
	f_teb->ThreadLocalStoragePointer = f_sbox_tlsp;
	SBOX_DBG("SBOX_THREAD: f_sbox_tlsp=0x%08x", f_sbox_tlsp);
	for(size_t i=0; i<g_sbox_process->f_sbox_module_list.size(); i++)
	{
		//SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u]", i);
		SBOX_MODULE &v_sbox_module = g_sbox_process->f_sbox_module_list[i];
		if(v_sbox_module.f_tls_index >= 0)
		{
			SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](A)", i);
			PVOID v_tls_raw_data = _aligned_malloc(v_sbox_module.f_tls_raw_data.size(), 16);
			//PVOID v_tls_raw_data = _aligned_malloc(v_sbox_module.f_tls_raw_data.size(), 4096); // FIXME
			SBOX_DBG("SBOX_THREAD: v_tls_raw_data=0x%08x", v_tls_raw_data);
			f_sbox_tls_raw_data_list.push_back(v_tls_raw_data);
			SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](C)", i);
			memcpy(v_tls_raw_data, &v_sbox_module.f_tls_raw_data[0], v_sbox_module.f_tls_raw_data.size());
			SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](D)", i);
#if 0x0 // FIXME: Testing
			size_t v_tls_raw_data_size = v_sbox_module.f_tls_dir->EndAddressOfRawData - v_sbox_module.f_tls_dir->StartAddressOfRawData;
			SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](D.1)", i);
			assert(v_tls_raw_data_size <= v_sbox_module.f_tls_raw_data.size());
			SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](D.2)", i);
			memcpy(v_tls_raw_data, (void *)v_sbox_module.f_tls_dir->StartAddressOfRawData, v_tls_raw_data_size);
			SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](D.3)", i);
#endif
			assert(sizeof(v_tls_raw_data)==sizeof(DWORD));
			SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](E)v_sbox_module.f_tls_index=%d", i, v_sbox_module.f_tls_index);
			((DWORD *)f_teb->ThreadLocalStoragePointer)[v_sbox_module.f_tls_index] = (DWORD)v_tls_raw_data;
			SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](F)", i);
			//if(!f_is_root)
			{
				for(size_t i_callback=0; i_callback<v_sbox_module.f_tls_callback_list.size(); i_callback++)
				{
					SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](G) i_callback=%u", i, i_callback);
					if(f_is_root)
					{
						v_sbox_module.f_tls_callback_list[i_callback]((PVOID)g_sbox_process->f_hmodule, DLL_PROCESS_ATTACH, 0);
						//v_sbox_module.f_tls_callback_list[i_callback]((PVOID)v_sbox_module.f_hmodule, DLL_PROCESS_ATTACH, 0);
					}
					else
					{
						v_sbox_module.f_tls_callback_list[i_callback]((PVOID)g_sbox_process->f_hmodule, DLL_THREAD_ATTACH, 0);
						//v_sbox_module.f_tls_callback_list[i_callback]((PVOID)v_sbox_module.f_hmodule, DLL_THREAD_ATTACH, 0);
					}
				}
			}
		}
	}
	//SBOX_DBG("SBOX_THREAD created: 0x%08x ORIG_TLSP=0x%08x SBOX_TLSP=0x%08x%s", f_teb, f_orig_tlsp, f_sbox_tlsp, f_is_root?" (ROOT)":"");
	SBOX_DBG("SBOX_THREAD created: 0x%08x%s", f_teb, f_is_root?" (ROOT)":"");
}
SBOX_THREAD::~SBOX_THREAD()
{
	//SBOX_DBG("SBOX_THREAD deleted: 0x%08x ORIG_TLSP=0x%08x SBOX_TLSP=0x%08x%s", f_teb, f_orig_tlsp, f_sbox_tlsp, f_is_root?" (ROOT)":"");
	SBOX_DBG("SBOX_THREAD deleted: 0x%08x%s", f_teb, f_is_root?" (ROOT)":"");
	for(size_t i=0; i<g_sbox_process->f_sbox_module_list.size(); i++)
	{
		//SBOX_DBG("~SBOX_THREAD: unprepare for g_sbox_process->f_sbox_module_list[%u]", i);
		SBOX_MODULE &v_sbox_module = g_sbox_process->f_sbox_module_list[i];
		if(v_sbox_module.f_tls_index >= 0)
		{
			//if(!f_is_root)
			//{
			for(size_t i_callback=0; i_callback<v_sbox_module.f_tls_callback_list.size(); i_callback++)
			{
				//SBOX_DBG("~SBOX_THREAD: unprepare for g_sbox_process->f_sbox_module_list[%u](G) i_callback=%u", i, i_callback);
				if(f_is_root)
				{
					v_sbox_module.f_tls_callback_list[i_callback]((PVOID)g_sbox_process->f_hmodule, DLL_PROCESS_DETACH, 0);
					//v_sbox_module.f_tls_callback_list[i_callback]((PVOID)v_sbox_module.f_hmodule, DLL_PROCESS_DETACH, 0);
				}
				else
				{
					v_sbox_module.f_tls_callback_list[i_callback]((PVOID)g_sbox_process->f_hmodule, DLL_THREAD_DETACH, 0);
					//v_sbox_module.f_tls_callback_list[i_callback]((PVOID)v_sbox_module.f_hmodule, DLL_THREAD_DETACH, 0);
				}
			}
			//}
		}
	}
	assert(f_teb);
	f_teb->ThreadLocalStoragePointer = f_orig_tlsp;
	if(f_sbox_tlsp)
	{
		_aligned_free(f_sbox_tlsp);
	}
	for(size_t i=0; i<f_sbox_tls_raw_data_list.size(); i++)
	{
		if(0) _aligned_free(f_sbox_tls_raw_data_list[i]); //FIXME
	}
}
TLS_VARIABLE_DECL SBOX_THREAD *g_sbox_thread = NULL;

static const char *tls_callback_reason_label(DWORD dwReason)
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

#define TLS_CALLBACK_SECT ".CRT$XLB"
static void NTAPI sbox_tls_callback(PVOID hModule, DWORD dwReason, PVOID pReserved)
{
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(pReserved);
	SBOX_DBG("sbox_tls_callback(%s): dwReason=0x%08x (%s)", TLS_CALLBACK_SECT, dwReason, tls_callback_reason_label(dwReason));
	switch(dwReason){
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		if(g_sbox_process)
		{
			delete g_sbox_process;
			g_sbox_process = NULL;
		}
		break;
	case DLL_THREAD_ATTACH:
		assert(g_sbox_thread == NULL);
		g_sbox_thread = new SBOX_THREAD();
		break;
	case DLL_THREAD_DETACH:
		if(g_sbox_thread)
		{
			delete g_sbox_thread;
			g_sbox_thread = NULL;
		}
		break;
	default:
		break;
	}
}
TLS_CALLBACK_DECL(TLS_CALLBACK_SECT, __sbox_tls_callback__, sbox_tls_callback);
