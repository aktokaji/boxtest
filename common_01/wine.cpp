#undef NDEBUG
#include <assert.h>

#include "wine.h"
#include <windows.h>
#include <psapi.h>
#include <imagehlp.h>
#include <sstream>
#include "strconv.h"
#include "win32_print.h"

#define WINE_DBG(format, ...) win32_printf("[WINE] " format "\n", ## __VA_ARGS__)

PWINE_TEB /*WINAPI*/ WineNtCurrentTeb(void)
{
	return (PWINE_TEB)__readfsdword(0x18);
}

PWINE_PEB /*WINAPI*/ WineNtCurrentPeb(void)
{
	PWINE_TEB v_teb = WineNtCurrentTeb();
	if(!v_teb) return NULL;
	return v_teb->Peb;
}

// ***** C++ only *****
WineSplitPath::WineSplitPath(const std::wstring &Original)
{
	this->Original = Original;
	WCHAR szDrive[_MAX_DRIVE];
	WCHAR szDir[_MAX_DIR];
	WCHAR szFName[_MAX_FNAME];
	WCHAR szExt[_MAX_EXT];
	_wsplitpath(this->Original.c_str(), szDrive, szDir, szFName, szExt);
	this->Drive = szDrive;
	this->Dir   = szDir;
	this->FName = szFName;
	this->Ext   = szExt;
}

std::wstring WineModuleInfo::ToString(void) const
{
	std::wstringstream v_stream;
	//WINE_DBG("WineModuleInfo::ToString(): this->BaseAddress=0x%08x", this->BaseAddress);
	//std::wstring v_addr_str = wstring_printf(L"0x%08x", this->BaseAddress);
	//WINE_DBG("WineModuleInfo::ToString(): v_addr_str=%s", WIDE_TO_ANSI(v_addr_str));
	v_stream << this->FileLocalName.c_str() << " @ " << WSTRING_PRINTF(L"0x%08x", this->BaseAddress);
	v_stream << " (TLS_INDEX: " << this->TlsIndex << ")";
	return v_stream.str();
}

std::vector<HMODULE> WineEnumModuleHandles(void)
{
	std::vector<HMODULE> v_result;
	HMODULE hMods[1024];
	DWORD cbNeeded;
	HANDLE hProcess = GetCurrentProcess();
	if(EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for ( unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
		{
			MODULEINFO mi;
			GetModuleInformation(hProcess, hMods[i], &mi, sizeof(MODULEINFO));
			v_result.push_back((HMODULE)mi.lpBaseOfDll);
		}
	}
	return v_result;
}

std::wstring WineGetModuleFileName(HMODULE hModule)
{
	WCHAR szPath[_MAX_PATH+1];
	DWORD dwRet = GetModuleFileNameW(hModule, szPath, _MAX_PATH);
	if(dwRet == 0) {
		return L"";
	}
	szPath[dwRet] = L'\0';
	return szPath;
}

std::vector<WineModuleInfo> WineEnumModuleInfos(void)
{
	std::vector<WineModuleInfo> v_result;
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

#define RtlGetProcessHeap() (WineNtCurrentPeb()->ProcessHeap)

// V:\QtBuild\+wine\wine-1.7.35\dlls\ntdll\tests\reg.c
//void WineTestRoutine(void)
void WineTestRoutine(PVOID a_tlsp)
{
	WINE_DBG("WineTestRoutine(0)");
/*
NTSYSAPI 
PVOID
NTAPI
RtlAllocateHeap(
  IN PVOID                HeapHandle,
  IN ULONG                Flags,
  IN ULONG                Size );
*/
	HMODULE hntdll = GetModuleHandleW(L"ntdll.dll");
	WINE_DBG("hntdll=0x%08x", hntdll);

	typedef LPVOID (WINAPI *procRtlReAllocateHeap)(PVOID,ULONG,PVOID,ULONG);
	procRtlReAllocateHeap RtlReAllocateHeap = (procRtlReAllocateHeap)GetProcAddress(hntdll, "RtlReAllocateHeap");
	WINE_DBG("RtlReAllocateHeap=0x%08x", RtlReAllocateHeap);

	typedef BOOLEAN (WINAPI *procRtlFreeHeap)(HANDLE,ULONG,PVOID);
	procRtlFreeHeap RtlFreeHeap = (procRtlFreeHeap)GetProcAddress(hntdll, "RtlFreeHeap");
	WINE_DBG("RtlFreeHeap=0x%08x", RtlFreeHeap);

	LPVOID v_proc_heap = RtlGetProcessHeap();
	WINE_DBG("v_proc_heap=0x%08x", v_proc_heap);

	BOOLEAN v_success = RtlFreeHeap(RtlGetProcessHeap(), 0, a_tlsp);
	WINE_DBG("v_success=%d", v_success);

	WINE_DBG("WineTestRoutine(End)");
	exit(0);
}
