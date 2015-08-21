#ifndef _WINE_H_
#define _WINE_H_

#include <windows.h>
#include "wine_teb.h"

#ifdef __cplusplus
extern "C" {
#endif
PWINE_TEB /*WINAPI*/ WineNtCurrentTeb(void);
PWINE_PEB /*WINAPI*/ WineNtCurrentPeb(void);
//void WineTestRoutine(void);
void WineTestRoutine(PVOID a_tlsp);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus // ***** C++ only *****
#include <string>
#include <list>
#include <vector>
class WineSplitPath
{
public:
	explicit WineSplitPath(const std::wstring &Original);
	std::wstring Original;
	std::wstring Drive;
	std::wstring Dir;
	std::wstring FName;
	std::wstring Ext;
};
class WineModuleInfo
{
public:
	HMODULE BaseAddress;
	DWORD   SizeOfImage;
	LPVOID  EntryPoint;
	LONG    TlsIndex = -1;
	std::wstring FileFullPath;
	std::wstring FileLocalName;
	std::wstring ToString(void) const;
};
std::vector<HMODULE> WineEnumModuleHandles(void);
std::vector<WineModuleInfo> WineEnumModuleInfos(void);
std::wstring WineGetModuleFileName(HMODULE hModule);
#endif /* __cplusplus */

#endif /* _WINE_H_ */
