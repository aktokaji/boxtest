#undef NDEBUG
#include <assert.h>

#include "PeLdr.h"
#include "Debug.h"
#include "PEB.h"

#include "sbox.h"
#include "wine.h"

//#include <strsafe.h>

#ifndef NTSTATUS
#define NTSTATUS	LONG
#endif

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS  ((NTSTATUS)0x00000000L)
#endif

#pragma warning(disable: 4995)

static
BOOL PeLdrApplyImageRelocations(DWORD dwImageBase, UINT_PTR iRelocOffset)
{
	PIMAGE_DOS_HEADER			pDosHeader;
	PIMAGE_NT_HEADERS			pNtHeaders;
	DWORD						x;
	DWORD						dwTmp;
	PIMAGE_BASE_RELOCATION		pBaseReloc;
	PIMAGE_RELOC				pReloc;

	fprintf(stderr, "[+] Applying Image Relocation (Base: 0x%08x RelocOffset: 0x%08x)\n",
		dwImageBase, iRelocOffset);

	pDosHeader = (PIMAGE_DOS_HEADER) dwImageBase;
	pNtHeaders = (PIMAGE_NT_HEADERS) (dwImageBase + pDosHeader->e_lfanew);

	pBaseReloc = (PIMAGE_BASE_RELOCATION) 
		(dwImageBase + 
		pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

	while(pBaseReloc->SizeOfBlock) {
		x = dwImageBase + pBaseReloc->VirtualAddress;
		dwTmp = (pBaseReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(IMAGE_RELOC);
		pReloc = (PIMAGE_RELOC) (((DWORD) pBaseReloc) + sizeof(IMAGE_BASE_RELOCATION));

		while(dwTmp--) {
			switch(pReloc->type) {
				case IMAGE_REL_BASED_DIR64:
					*((UINT_PTR*)(x + pReloc->offset)) += iRelocOffset;
					break;	
				case IMAGE_REL_BASED_HIGHLOW:
					*((DWORD*)(x + pReloc->offset)) += (DWORD) iRelocOffset;
					break;

				case IMAGE_REL_BASED_HIGH:
					*((WORD*)(x + pReloc->offset)) += HIWORD(iRelocOffset);
					break;

				case IMAGE_REL_BASED_LOW:
					*((WORD*)(x + pReloc->offset)) += LOWORD(iRelocOffset);
					break;

				case IMAGE_REL_BASED_ABSOLUTE:
					break;

				default:
					fprintf(stderr, "[+] Unknown relocation type: 0x%08x\n", pReloc->type);
					break;
			}

			pReloc += 1;
		}

		pBaseReloc = (PIMAGE_BASE_RELOCATION)(((DWORD) pBaseReloc) + pBaseReloc->SizeOfBlock);
	}

	return TRUE;
}

static
BOOL PeLdrProcessIAT(DWORD dwImageBase)
{
	BOOL						ret = FALSE;
	PIMAGE_DOS_HEADER			pDosHeader;
	PIMAGE_NT_HEADERS			pNtHeaders;
	PIMAGE_IMPORT_DESCRIPTOR	pImportDesc;
	PIMAGE_THUNK_DATA			pThunkData;
	PIMAGE_THUNK_DATA			pThunkDataOrig;
	PIMAGE_IMPORT_BY_NAME		pImportByName;
	PIMAGE_EXPORT_DIRECTORY		pExportDir;
	DWORD						flError = 0;
	DWORD						dwTmp;
	BYTE						*pLibName;
	HMODULE						hMod;

	fprintf(stderr, "[+] Processing IAT (Image Base: 0x%08x)\n", dwImageBase);

	pDosHeader = (PIMAGE_DOS_HEADER) dwImageBase;
	pNtHeaders = (PIMAGE_NT_HEADERS) (dwImageBase + pDosHeader->e_lfanew);

	do {
		pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)(dwImageBase +
			pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

		if(!pImportDesc) {
			fprintf(stderr, "[+] IAT not found\n");
			break;
		}

		while((pImportDesc->Name != 0) && (!flError)) {
			pLibName = (BYTE*) (dwImageBase + pImportDesc->Name);
			fprintf(stderr, "[+] Loading Library and processing Imports: %s\n", (CHAR*) pLibName);

			if(pImportDesc->ForwarderChain != -1) {
				fprintf(stderr, "[+] FIXME: Cannot handle Import Forwarding\n");
				//flError = 1;
				//break;
			}

			hMod = LoadLibraryA((CHAR*) pLibName);
			if(!hMod) {
				fprintf(stderr, "[+] Failed to load library: %s\n", pLibName);
				flError = 1;
				break;
			}

			pThunkData = (PIMAGE_THUNK_DATA)(dwImageBase + pImportDesc->FirstThunk);
			if(pImportDesc->Characteristics == 0)
				/* Borland compilers doesn't produce Hint Table */
				pThunkDataOrig = pThunkData;
			else
				/* Hint Table */
				pThunkDataOrig = (PIMAGE_THUNK_DATA)(dwImageBase + pImportDesc->Characteristics);

			while(pThunkDataOrig->u1.AddressOfData != 0) {
				if(pThunkDataOrig->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
					/* Import via. Export Ordinal */
					PIMAGE_DOS_HEADER		_dos;
					PIMAGE_NT_HEADERS		_nt;

					_dos = (PIMAGE_DOS_HEADER) hMod;
					_nt = (PIMAGE_NT_HEADERS) (((DWORD) hMod) + _dos->e_lfanew);

					pExportDir = (PIMAGE_EXPORT_DIRECTORY) 
						(((DWORD) hMod) + _nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
					dwTmp = (((DWORD) hMod) + pExportDir->AddressOfFunctions) + (((IMAGE_ORDINAL(pThunkDataOrig->u1.Ordinal) - pExportDir->Base)) * sizeof(DWORD));
					dwTmp = ((DWORD) hMod) + *((DWORD*) dwTmp);
					fprintf(stderr, "[+] dwTmp=0x%08x\n", dwTmp);
					pThunkData->u1.Function = dwTmp;
				}
				else {
					pImportByName = (PIMAGE_IMPORT_BY_NAME)
						(dwImageBase + pThunkDataOrig->u1.AddressOfData);
					pThunkData->u1.Function = (DWORD) GetProcAddress(hMod, (LPCSTR) pImportByName->Name);
					//fprintf(stderr, "[+] Resolving API: %s!%s\n", (CHAR*)pLibName, (CHAR*)pImportByName->Name);
					if(!pThunkData->u1.Function) {
						fprintf(stderr, "[+] Failed to resolve API: %s!%s\n", 
							(CHAR*)pLibName, (CHAR*)pImportByName->Name);
						flError = 1;
						break;
					}
				}

				pThunkDataOrig++;
				pThunkData++;
			}

			pImportDesc++;
		}

		if(!flError)
			ret = TRUE;

	} while(0);
	return ret;
}

static
BOOL PeLdrNeedSelfRelocation(PE_LDR_PARAM *pe)
{
	DWORD				dwMyBase;
	PIMAGE_DOS_HEADER	pMyDosHeader;
	PIMAGE_NT_HEADERS	pMyNtHeaders;

	fprintf(stderr, "[+] Checking for self relocation\n");

	dwMyBase = (DWORD) GetModuleHandle(NULL);
	if(!dwMyBase) {
		fprintf(stderr, "[-] Failed to get our loaded address\n");
		return FALSE;
	}
	
	pMyDosHeader = (PIMAGE_DOS_HEADER) dwMyBase;
	pMyNtHeaders = (PIMAGE_NT_HEADERS) (dwMyBase + pMyDosHeader->e_lfanew);

	if(pMyNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
		fprintf(stderr, "[-] Failed to find our own headers\n");
		return FALSE;
	}

	fprintf(stderr, "[+] MyBase: 0x%08x MySize: %d\n", dwMyBase, pMyNtHeaders->OptionalHeader.SizeOfImage);

	if((pe->pNtHeaders->OptionalHeader.ImageBase >= dwMyBase) &&
		(pe->pNtHeaders->OptionalHeader.ImageBase < (dwMyBase + pMyNtHeaders->OptionalHeader.SizeOfImage)))
	{
		fprintf(stderr, "[+] Self relocation required\n");
		return TRUE;
	}

	return FALSE;
}

static
BOOL PeLdrRelocateAndContinue(PE_LDR_PARAM *pe, VOID *pContFunc, VOID *pParam)
{
	PIMAGE_DOS_HEADER	pMyDosHeader;
	PIMAGE_NT_HEADERS	pMyNtHeaders;
	DWORD				dwNewBase;
	DWORD				dwMyBase;
	DWORD				dwAddr;
	UINT_PTR			iRelocOffset;

	fprintf(stderr, "[+] Relocating loader image (Continue Function: 0x%08x)\n", (DWORD) pContFunc);

	dwMyBase = (DWORD) GetModuleHandle(NULL);
	if(!dwMyBase) {
		fprintf(stderr, "[-] Failed to get our loaded address\n");
		return FALSE;
	}
	
	pMyDosHeader = (PIMAGE_DOS_HEADER) dwMyBase;
	pMyNtHeaders = (PIMAGE_NT_HEADERS) (dwMyBase + pMyDosHeader->e_lfanew);

	dwNewBase = (DWORD) VirtualAlloc(NULL, pMyNtHeaders->OptionalHeader.SizeOfImage + 1,
		MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if(!dwNewBase) {
		fprintf(stderr, "[-] Failed to allocate memory for self relocation\n");
		return FALSE;
	}

	fprintf(stderr, "[+] New Loader Base: 0x%08x\n", dwNewBase);
	pe->dwLoaderRelocatedBase = dwNewBase;

	CopyMemory((VOID*) dwNewBase, (VOID*) dwMyBase,
		pMyNtHeaders->OptionalHeader.SizeOfImage);

	if(!PeLdrProcessIAT(dwNewBase)) {
		fprintf(stderr, "[-] Failed to process IAT for relocated image\n");
		return FALSE;
	}

	iRelocOffset = dwNewBase - dwMyBase;
	if(!PeLdrApplyImageRelocations(dwNewBase, iRelocOffset)) {
		fprintf(stderr, "[-] Failed to apply relocations on relocated image\n");
		return FALSE;
	}

	pe->dwLoaderBase = dwNewBase;

	dwAddr = ((DWORD) pContFunc) - dwMyBase;
	dwAddr += dwNewBase;

	fprintf(stderr, "[+] Jumping to relocated image (Relocated Continue Function: 0x%08x)\n", dwAddr);
#ifdef __GNUC__
	//asm ("		mov %eax, pParam");
	asm ("		movl %0, %%eax" ::"m" (pParam));
	asm ("		push %eax");
	//asm ("		mov %eax, dwAddr");
	asm ("		movl %0, %%eax" ::"m" (dwAddr));
	asm ("		call %eax");
#else
	__asm {
		mov eax, pParam
		push eax
		mov eax, dwAddr
		call eax
	}
#endif
	return TRUE;
}

// Protection flags for memory pages (Executable, Readable, Writeable)
static int ProtectionFlags[2][2][2] = {
	{
		// not executable
		{PAGE_NOACCESS, PAGE_WRITECOPY},
		{PAGE_READONLY, PAGE_READWRITE},
	}, {
		// executable
		{PAGE_EXECUTE, PAGE_EXECUTE_WRITECOPY},
		{PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE},
	},
};

static
BOOL PeLdrProtectSections(PE_LDR_PARAM *pe)
{
	fprintf(stderr, "[+] Protecting Sections\n");
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pe->pNtHeaders);
	for(UINT i = 0; i < pe->pNtHeaders->FileHeader.NumberOfSections; i++) {
		PIMAGE_SECTION_HEADER section = &pSectionHeader[i];
		//LPVOID sectionAddr = (LPVOID)section->Misc.PhysicalAddress;
		LPVOID sectionAddr = (LPVOID)(pe->dwMapBase + section->VirtualAddress);
		//fprintf(stderr, "[+]   Protecting Section: %.8s\n", (CHAR*)pSectionHeader[i].Name);
		fprintf(stderr, "[+]   Protecting Section: %.8s\n", (CHAR*)section->Name);
		DWORD protect, oldProtect, size;
		int executable = (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
		int readable =   (section->Characteristics & IMAGE_SCN_MEM_READ) != 0;
		int writeable =  (section->Characteristics & IMAGE_SCN_MEM_WRITE) != 0;
		fprintf(stderr, "[+]   Protecting Section: %.8s E=%d R=%d W=%d\n", (CHAR*)section->Name, executable, readable, writeable);
		if (section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE)
		{
			// section is not needed any more and can safely be freed
			//VirtualFree(sectionAddr, section->SizeOfRawData, MEM_DECOMMIT);
			continue;
		}
		// determine protection flags based on characteristics
		protect = ProtectionFlags[executable][readable][writeable];
		if (section->Characteristics & IMAGE_SCN_MEM_NOT_CACHED)
			protect |= PAGE_NOCACHE;
		// determine size of region
		size = section->SizeOfRawData;
		//xxx
		fprintf(stderr, "[+]    size(1)=%u\n", size);
		if (size == 0)
		{
			if (section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
			{
				fprintf(stderr, "[+]    IMAGE_SCN_CNT_INITIALIZED_DATA\n");
				size = pe->pNtHeaders->OptionalHeader.SizeOfInitializedData;
			}
			else if (section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
			{
				fprintf(stderr, "[+]    IMAGE_SCN_CNT_UNINITIALIZED_DATA\n");
				size = pe->pNtHeaders->OptionalHeader.SizeOfUninitializedData;
			}
		}
		
		fprintf(stderr, "[+]    size(2)=%u\n", size);
		if (size > 0)
		{
			// change memory access flags
			if (VirtualProtect(sectionAddr, section->SizeOfRawData, protect, &oldProtect) == 0)
			{
				fprintf(stderr, "[+] Error protecting memory page\n");
			}
		}
	}
	return TRUE;
}

static
BOOL PeLdrExecuteEP(PE_LDR_PARAM *pe)
{
	DWORD	dwOld;
	DWORD	dwEP;
	//_PPEB	peb;

	// TODO: Fix permission as per section flags
#if 0x1
	if(!VirtualProtect((LPVOID) pe->dwMapBase, pe->pNtHeaders->OptionalHeader.SizeOfImage,
		PAGE_EXECUTE_READWRITE, &dwOld)) {
		fprintf(stderr, "[+] Failed to change mapping protection\n");
		return FALSE;
	}
#else
	if(!VirtualProtect((LPVOID) pe->dwMapBase, pe->pNtHeaders->OptionalHeader.SizeOfImage,
		PAGE_READWRITE, &dwOld)) {
		fprintf(stderr, "[+] Failed to change mapping protection\n");
		return FALSE;
	}

	if(!PeLdrProtectSections(pe))
	{
		fprintf(stderr, "[+] Failed to change mapping protection\n");
		return FALSE;
	}
#endif

	assert(g_sbox_process);
	assert(sizeof(HMODULE)==sizeof(pe->dwMapBase));
	//if(0) g_sbox_process->register_module((HMODULE)pe->dwMapBase); // FIXME
	g_sbox_process->register_module((HMODULE)pe->dwMapBase);
	g_sbox_process->alloc_main_thread();
	assert(g_sbox_thread);
	assert(g_sbox_process->f_root_thread == g_sbox_thread);

	PWINE_TEB v_teb = g_sbox_thread->f_teb;
	for(ULONG i=0; i<g_sbox_process->f_num_extended_tls; i++)
	{
		win32_printf("[Main] v_teb->ThreadLocalStoragePointer[%02d]=0x%08x\n", i, ((DWORD *)v_teb->ThreadLocalStoragePointer)[i]);
	}

	//fprintf(stderr, "[+] Fixing Image Base address in PEB\n");
	//peb = (_PPEB)__readfsdword(0x30);
	//peb->lpImageBaseAddress = (LPVOID) pe->dwMapBase;

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pe->dwMapBase;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		fprintf(stderr, "[+] DOS Signature invalid\n");
		return FALSE;
	}

	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(((DWORD)pe->dwMapBase) + pDosHeader->e_lfanew);
	if(pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
		fprintf(stderr, "[+] NT Signature mismatch\n");
		return FALSE;
	}

	//dwEP = pe->dwMapBase + pe->pNtHeaders->OptionalHeader.AddressOfEntryPoint;
	dwEP = pe->dwMapBase + pNtHeaders->OptionalHeader.AddressOfEntryPoint;
	fprintf(stderr, "[+] Executing Entry Point: 0x%08x\n", dwEP);
#ifdef __GNUC__
	asm ("		movl %0, %%eax" ::"m" (dwEP));
	asm ("		jmp %eax");
#else
	__asm {
		mov eax, dwEP
		jmp eax
	}
#endif
	return TRUE;
}

static
BOOL PeLdrApplyRelocations(PE_LDR_PARAM *pe)
{
	UINT_PTR	iRelocOffset;

	if(pe->dwMapBase == pe->pNtHeaders->OptionalHeader.ImageBase) {
		fprintf(stderr, "[+] Relocation not required\n");
		return TRUE;
	}

	if(!pe->pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size) {
		fprintf(stderr, "[+] PE required relocation but no relocatiom information found\n");
		return FALSE;
	}

	iRelocOffset = pe->dwMapBase - pe->pNtHeaders->OptionalHeader.ImageBase;
	return PeLdrApplyImageRelocations(pe->dwMapBase, iRelocOffset);
}

static
BOOL PeLdrMapImage(PE_LDR_PARAM *pe)
{
	DWORD						i;
	MEMORY_BASIC_INFORMATION	mi;
	PIMAGE_SECTION_HEADER		pSectionHeader;
	BOOL						ret = FALSE;

	NTSTATUS	(NTAPI *NtUnmapViewOfSection)
					(HANDLE, LPVOID) = NULL;
	if(!pe)
		return ret;

	fprintf(stderr, "[+] Mapping Target PE File\n");
	fprintf(stderr, "[+] Loader Base Orig: 0x%08x New: 0x%08x\n", 
		pe->dwLoaderBase, pe->dwLoaderRelocatedBase);

	NtUnmapViewOfSection = 
		(NTSTATUS (NTAPI *)(HANDLE, LPVOID))
			GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "ZwUnmapViewOfSection");
	if(!NtUnmapViewOfSection)
		fprintf(stderr, "[+] Failed to resolve address of NtUnmapViewOfSection\n");

	do {
		fprintf(stderr, "[+] Target PE Load Base: 0x%08x Image Size: 0x%08x\n",
			pe->pNtHeaders->OptionalHeader.ImageBase,
			pe->pNtHeaders->OptionalHeader.SizeOfImage);

		// Find the size of our mapping
		i = pe->dwLoaderBase;
		while(VirtualQuery((LPVOID) i, &mi, sizeof(mi))) {
			if(mi.State == MEM_FREE)
				break;

			i += mi.RegionSize;
		}

		if((pe->pNtHeaders->OptionalHeader.ImageBase >= pe->dwLoaderBase) && 
			(pe->pNtHeaders->OptionalHeader.ImageBase < i)) {
			if(NtUnmapViewOfSection) {
				fprintf(stderr, "[+] Unmapping original loader mapping\n");
				if(NtUnmapViewOfSection(GetCurrentProcess(), (VOID*) pe->dwLoaderBase) == STATUS_SUCCESS) {
					pe->dwMapBase = (DWORD) VirtualAlloc((LPVOID) pe->pNtHeaders->OptionalHeader.ImageBase,
										pe->pNtHeaders->OptionalHeader.SizeOfImage + 1,
										MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
				}
				else {
					fprintf(stderr, "[-] Failed to unmap original loader mapping\n");
				}
			}
		}
		
		pe->dwMapBase = (DWORD) VirtualAlloc((LPVOID) pe->pNtHeaders->OptionalHeader.ImageBase,
			pe->pNtHeaders->OptionalHeader.SizeOfImage + 1,
			MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		if(!pe->dwMapBase)
			fprintf(stderr, "[-] Failed to allocate PE ImageBase: 0x%08x\n", 
				pe->pNtHeaders->OptionalHeader.ImageBase);

		if(!pe->dwMapBase) {
			fprintf(stderr, "[+] Attempting to allocate new memory\n");
#if 0x0
			if(!pe->pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size) {
				fprintf(stderr, "[-] Failed to map required memory address, need relocation to continue\n");
				fprintf(stderr, "[-] [WARNING] Forcing re-use of mapped memory\n");
				
				pe->dwMapBase = (DWORD) pe->pNtHeaders->OptionalHeader.ImageBase;
			}
			else {
				pe->dwMapBase = (DWORD) VirtualAlloc(NULL, 
					pe->pNtHeaders->OptionalHeader.SizeOfImage + 1,
					MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			}
#else
			pe->dwMapBase = (DWORD) VirtualAlloc(NULL, 
				pe->pNtHeaders->OptionalHeader.SizeOfImage + 1,
				MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#endif
		}

		if(!pe->dwMapBase) {
			fprintf(stderr, "[-] Failed to map memory for Target PE\n");
			break;
		}

		fprintf(stderr, "[+] Allocated memory for Target PE: 0x%08x\n", pe->dwMapBase);
		
		fprintf(stderr, "[+] Copying Headers\n");
		CopyMemory((LPVOID) pe->dwMapBase, (LPVOID) pe->dwImage,
			pe->pNtHeaders->OptionalHeader.SizeOfHeaders);

		fprintf(stderr, "[+] Copying Sections\n");
		////PIMAGE_NT_HEADERS old_headers = (PIMAGE_NT_HEADERS)&((const unsigned char *)(data))[dos_header->e_lfanew];
		////PIMAGE_NT_HEADERS old_headers = pe->pNtHeaders;
		pSectionHeader = IMAGE_FIRST_SECTION(pe->pNtHeaders);
		for(i = 0; i < pe->pNtHeaders->FileHeader.NumberOfSections; i++) {
			PIMAGE_SECTION_HEADER section = &pSectionHeader[i];
			//fprintf(stderr, "[+]   Copying Section: %s\n", (CHAR*) pSectionHeader[i].Name);
			fprintf(stderr, "[+]   Copying Section: %.8s\n", (CHAR*)section->Name);
			//xxx
#if 0x0			
			if (section->SizeOfRawData == 0)
			{
				fprintf(stderr, "[+]   Copying Section(1): %.8s\n", (CHAR*)section->Name);
				// section doesn't contain data in the dll itself, but may define
				// uninitialized data
				DWORD size = old_headers->OptionalHeader.SectionAlignment;
				fprintf(stderr, "[+]   Copying Section(1.1): %.8s size=%u\n", (CHAR*)section->Name, size);
				if (size > 0)
				{
					fprintf(stderr, "[+]   Copying Section(1.2): %.8s\n", (CHAR*)section->Name);
					//section->Misc.PhysicalAddress = (DWORD)(pe->dwMapBase + section->VirtualAddress);
					fprintf(stderr, "[+]   Copying Section(1.3): %.8s\n", (CHAR*)section->Name);
					ZeroMemory((LPVOID)(pe->dwMapBase + section->VirtualAddress), size);
					fprintf(stderr, "[+]   Copying Section(1.4): %.8s\n", (CHAR*)section->Name);
				}
				fprintf(stderr, "[+]   Copying Section(1.5): %.8s\n", (CHAR*)section->Name);
				// section is empty
				continue;
			}
#endif
			fprintf(stderr, "[+]   Copying Section(2): %.8s\n", (CHAR*)section->Name);
			CopyMemory(
				(LPVOID)(pe->dwMapBase + pSectionHeader[i].VirtualAddress),
				(LPVOID)(pe->dwImage + pSectionHeader[i].PointerToRawData),
				pSectionHeader[i].SizeOfRawData
			);
			//section->Misc.PhysicalAddress = (DWORD)(pe->dwMapBase + section->VirtualAddress);
			fprintf(stderr, "[+]   Copying Section(3): %.8s\n", (CHAR*)section->Name);
		}

		ret = TRUE;
	} while(0);

	return ret;
}

static 
BOOL PeLdrLoadImage(PE_LDR_PARAM *pe)
{
	HANDLE	hFile	= NULL;
	HANDLE	hMap	= NULL;
	BOOL	ret = FALSE;
	//_PPEB	peb;

	if(!pe)
		goto out;

	fprintf(stderr, "[+] Mapping PE File\n");

	if(!pe->bLoadFromBuffer) {
		fprintf(stderr, "[+] Creating Map View of File\n");

		hFile = CreateFile(pe->pTargetPath, GENERIC_READ, 
			FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE) {
			fprintf(stderr, "[+] Failed to open PE File\n");
			goto out;
		}

		pe->dwImageSizeOnDisk = GetFileSize(hFile, NULL);
		hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if(hMap == NULL) {
			fprintf(stderr, "[+] Failed to create file mapping for PE File\n");
			goto out;
		}

		pe->dwImage = (DWORD) MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
		if(!pe->dwImage) {
			fprintf(stderr, "[+] Failed to obtain a map view of PE File\n");
			goto out;
		}
	}

	fprintf(stderr, "[+] Map View of File created: pe->dwImage=0x%08x\n", pe->dwImage);

	pe->pDosHeader = (PIMAGE_DOS_HEADER) pe->dwImage;
	if(pe->pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		fprintf(stderr, "[+] DOS Signature invalid\n");
		goto out;
	}

	pe->pNtHeaders = (PIMAGE_NT_HEADERS)(((DWORD) pe->dwImage) + pe->pDosHeader->e_lfanew);
	if(pe->pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
		fprintf(stderr, "[+] NT Signature mismatch\n");
		goto out;
	}

	//peb = (_PPEB)__readfsdword(0x30);
	//pe->dwLoaderBase = (DWORD) peb->lpImageBaseAddress;

	ret = TRUE;

out:
	if(hMap)
		CloseHandle(hMap);
	if(hFile)
		CloseHandle(hFile);

	return ret;
}

static
BOOL PeLdrRunImage(PE_LDR_PARAM *pe)
{
	if(!PeLdrMapImage(pe))
		return FALSE;
	if(!PeLdrProcessIAT(pe->dwMapBase))
		return FALSE;
	if(!PeLdrApplyRelocations(pe))
		return FALSE;
	if(!PeLdrExecuteEP(pe))
		return FALSE;

	return TRUE;
}

BOOL PeLdrStart(PE_LDR_PARAM *pe)
{
	fprintf(stderr, "[+] PeLdrStart(1)\n");
	if(!PeLdrLoadImage(pe))
		return FALSE;

#if 0x1
	if(PeLdrNeedSelfRelocation(pe))
	{
		fprintf(stderr, "[+] PeLdrStart(2)\n");
		return PeLdrRelocateAndContinue(pe, (VOID*) PeLdrRunImage, (VOID*) pe);
	}
	else
#endif
	{
		fprintf(stderr, "[+] PeLdrStart(3)\n");
		return PeLdrRunImage(pe);
	}
}

BOOL PeLdrSetExecutableBuffer(PE_LDR_PARAM *pe, PVOID pExecutable, DWORD dwLen)
{
	if(!pe)
		return FALSE;

	pe->dwImageSizeOnDisk = dwLen;
	pe->dwImage = (DWORD) pExecutable;
	pe->bLoadFromBuffer = TRUE;

	return TRUE;
}

BOOL PeLdrSetExecutablePath(PE_LDR_PARAM *pe, TCHAR *pExecutable)
{
	if(!pe)
		return FALSE;

	pe->pTargetPath = (TCHAR*) HeapAlloc(GetProcessHeap(), 
		HEAP_ZERO_MEMORY, (lstrlen(pExecutable) + 1) * sizeof(TCHAR));
	if(!pe->pTargetPath) {
		fprintf(stderr, "[+] Failed to allocate memory for pTargetPath\n");
		return FALSE;
	}

	lstrcpy(pe->pTargetPath, pExecutable);
	return TRUE;
}

VOID PeLdrInit(PE_LDR_PARAM *pe)
{
	ZeroMemory(pe, sizeof(PE_LDR_PARAM));
}
