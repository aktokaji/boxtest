#ifndef _SBOX_H_
#define _SBOX_H_

#include <windows.h>
#include "wine.h"
#include "win32_print.h"
#include "tlsdecl.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus // ***** C++ only *****
typedef void (NTAPI *SBOX_TLS_CALLBACK_PROC)(PVOID hModule, DWORD dwReason, PVOID pReserved);
class SBOX_MODULE
{
public:
	HMODULE f_hmodule = NULL;
	PIMAGE_TLS_DIRECTORY f_tls_dir = NULL;
	LONG f_tls_index = -1;
	std::vector<BYTE> f_tls_raw_data;
	std::vector<SBOX_TLS_CALLBACK_PROC> f_tls_callback_list;
#if 0x0
	DWORD StartAddressOfRawData;
	DWORD SizeOfRawData;
	DWORD SizeOfZeroFill;
#endif
};
class SBOX_THREAD
{
private:
	LPVOID f_orig_tlsp = NULL;
	LPVOID f_sbox_tlsp = NULL;
	std::vector<PVOID> f_sbox_tls_raw_data_list;
public:
	PWINE_TEB f_teb = NULL;
	bool f_is_root = false; 
	explicit SBOX_THREAD();
	virtual ~SBOX_THREAD();
};
extern TLS_VARIABLE_DECL SBOX_THREAD *g_sbox_thread;
class SBOX_PROCESS
{
private:
	std::vector<WineModuleInfo> f_root_module_list;
public:
	HMODULE f_hmodule = NULL;
	PWINE_TEB f_teb = NULL;
	SBOX_THREAD *f_root_thread = NULL;
	DWORD f_num_implicit_tls = 0;
	DWORD f_num_extended_tls = 0;
	std::vector<SBOX_MODULE> f_sbox_module_list;
	explicit SBOX_PROCESS();
	virtual ~SBOX_PROCESS();
	bool register_module(HMODULE hModule);
	void alloc_main_thread();
};
extern SBOX_PROCESS *g_sbox_process;
#endif /* __cplusplus */

#endif /* _SBOX_H_ */
