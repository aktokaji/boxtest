#ifndef _SBOX_H_
#define _SBOX_H_

// [MemoryModule] https://github.com/fancycode/MemoryModule

#include <windows.h>
#include "wine.h"
#include "win32_print.h"
#include "vardecl.h"
#include "MemoryModule.h"
#include <QtCore>

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
    HMEMORYMODULE f_hmodule = NULL;
    QString f_basename;
    bool f_is_dll = false;
    FARPROC f_main_addr = NULL;
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
    QMap<QString, QString> f_dll_loc_map;
	explicit SBOX_PROCESS();
	virtual ~SBOX_PROCESS();
    bool register_dll_location(const QString &fullPath);
    bool register_module(HMEMORYMODULE hModule, const QString &baseName);
    void alloc_main_thread();
};
extern SBOX_PROCESS *g_sbox_process;

HCUSTOMMODULE SBOX_LoadLibrary(LPCSTR filename, void *userdata);
FARPROC SBOX_GetProcAddress(HCUSTOMMODULE module, LPCSTR name, void *userdata);
void SBOX_FreeLibrary(HCUSTOMMODULE module, void *userdata);

int RunFromMemory(const QString &fileName);

#endif /* __cplusplus */

#endif /* _SBOX_H_ */
