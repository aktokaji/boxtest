#undef NDEBUG
#include <assert.h>

#include "sbox.h"
#include "strconv.h"
#include <imagehlp.h> // ImageDirectoryEntryToData()

#include "MemoryModule.cpp"

//#ifdef _DEBUG
#if 0x1
//#define SBOX_DBG(format, ...) win32_printfA("[SBOX] " format "\n", ## __VA_ARGS__)
#define SBOX_DBG(format, ...) qDebug("[SBOX] " format, ## __VA_ARGS__)
#else
#define SBOX_DBG(format, ...) (void)0
#endif

//static HCUSTOMMODULE l_mydll = 0;

HCUSTOMMODULE SBOX_LoadLibrary(LPCSTR filename, void *userdata)
{
    Q_UNUSED(userdata);
    QString v_filename = filename;
    v_filename = v_filename.toLower();
    qDebug() << "[SBOX_LoadLibrary()]" << v_filename;
    if(!v_filename.startsWith("qt5") && ::GetModuleHandleA(filename) != NULL)
    //if(::GetModuleHandleA(filename) != NULL)
    {
        qDebug().noquote() << "  by LoadLibraryA(1)";
        return LoadLibraryA(filename);
    }
    for(size_t i=0; i<g_sbox_process->f_sbox_module_list.size(); i++)
    {
        SBOX_MODULE &module = g_sbox_process->f_sbox_module_list[i];
        if(v_filename == module.f_basename)
        {
            qDebug().noquote() << "  from Module List";
            PMEMORYMODULE pModule = (PMEMORYMODULE)module.f_hmodule;
            return pModule->codeBase;
        }
    }
    if(g_sbox_process->f_dll_loc_map.contains(v_filename))
    {
        QString fullPath = g_sbox_process->f_dll_loc_map.value(v_filename);
        qDebug().noquote() << "  from Dll Map" << fullPath;
        QFile v_file(fullPath);
        if(!v_file.open(QIODevice::ReadOnly)) return NULL;
#if 0x0
        QByteArray v_bytes = v_file.readAll();
        HMEMORYMODULE handle = MemoryLoadLibraryEx(v_bytes.constData(), SBOX_LoadLibrary, SBOX_GetProcAddress, SBOX_FreeLibrary, NULL);
#else
        QByteArray *v_bytes = new QByteArray();
        *v_bytes = v_file.readAll();
        HMEMORYMODULE handle = MemoryLoadLibraryEx(v_bytes->constData(), SBOX_LoadLibrary, SBOX_GetProcAddress, SBOX_FreeLibrary, NULL);
#endif
        g_sbox_process->register_module(handle, v_filename);
        PMEMORYMODULE pModule = (PMEMORYMODULE)handle;
        qDebug().noquote() << "  from Dll Map" << fullPath << (void *)pModule->codeBase;
        return pModule->codeBase;
    }
#if 0x0
    if(v_filename.toLower() == "tlsdll.dll")
    {
        QFile v_file("E:/testbed/tlsdll.dll");
        if(!v_file.open(QIODevice::ReadOnly)) return NULL;
        QByteArray v_bytes = v_file.readAll();
        HMEMORYMODULE handle = MemoryLoadLibraryEx(v_bytes.constData(), SBOX_LoadLibrary, SBOX_GetProcAddress, SBOX_FreeLibrary, NULL);
        l_mydll = (HCUSTOMMODULE)handle;
        QFileInfo fi(v_filename);
        g_sbox_process->register_module(l_mydll, fi.fileName());
        return l_mydll;
    }
#endif
    qDebug().noquote() << "  by LoadLibraryA(2)";
    HMODULE result = LoadLibraryA(filename);
    if (result == NULL) {
        return NULL;
    }
    return (HCUSTOMMODULE) result;
}

static
HMODULE
WINAPI
_LoadLibraryA(LPCSTR lpLibFileName)
{
    //HMODULE hModule = LoadLibraryA(lpLibFileName);
    QString maybeFullpath = QString::fromLocal8Bit(lpLibFileName);
    QFileInfo fi(maybeFullpath);
    HMODULE hModule = (HMODULE)SBOX_LoadLibrary(fi.fileName().toLocal8Bit().constData(), NULL);
    qDebug() << "[_LoadLibraryA()]" << fi.fileName() << QString::fromLocal8Bit(lpLibFileName) << hModule;
    g_sbox_process->attach_after_load();
    return hModule;
}

static
HMODULE
WINAPI
_LoadLibraryW(LPCWSTR lpLibFileName)
{
    //HMODULE hModule = LoadLibraryW(lpLibFileName);
    QString maybeFullpath = QString::fromWCharArray(lpLibFileName);
    QFileInfo fi(maybeFullpath);
    QString fname = fi.fileName().toLower();
    //HMODULE hModule = (HMODULE)SBOX_LoadLibrary(fi.fileName().toLocal8Bit().constData(), NULL);
    HMODULE hModule = (HMODULE)SBOX_LoadLibrary(fi.fileName().toLocal8Bit().constData(), (void *)fname.toLocal8Bit().constData());
    qDebug() << "[_LoadLibraryW()]" << fi.fileName() << QString::fromWCharArray(lpLibFileName) << hModule;
    g_sbox_process->attach_after_load();
    return hModule;
}

static
FARPROC
WINAPI
My_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    //qDebug() << "[My_GetProcAddress()]" << lpProcName;
    //FARPROC proc = GetProcAddress(hModule, lpProcName);
    FARPROC proc = SBOX_GetProcAddress((HCUSTOMMODULE)hModule, lpProcName, NULL, "?");
    //qDebug() << "[My_GetProcAddress()]" << lpProcName << (void *)proc;
    ////if(proc == NULL) exit(-1);
    return proc;
}

FARPROC SBOX_GetProcAddress(HCUSTOMMODULE module, LPCSTR name, void *userdata, LPCSTR from)
{
    Q_UNUSED(userdata); Q_UNUSED(from);
    //qDebug() << "[SBOX_GetProcAddress()]" << module << name;
#if 0x1
    for(size_t i=0; i<g_sbox_process->f_sbox_module_list.size(); i++)
    {
        SBOX_MODULE &sbox_module = g_sbox_process->f_sbox_module_list[i];
        PMEMORYMODULE pModule = (PMEMORYMODULE)sbox_module.f_hmodule;
        if((DWORD)module == (DWORD)pModule->codeBase)
        {
            if((quint32)name < 1000)
            {
                qDebug() << "exit!";
                exit(-1);
            }
            if(g_sbox_process->f_debug_module_list.contains(sbox_module.f_basename))
            {
                qDebug() << "[SBOX_GetProcAddress()]" << sbox_module.f_basename << module << name;
            }
#if 0x0
            QFileInfo fi(from);
            QString fname = fi.fileName().toLower();
            if(false)//if(g_sbox_process->f_debug_module_list.contains(fname))
            {
                qDebug() << "[SBOX_GetProcAddress()] <from>" << fname << module << name;
            }
            //if(userdata)
            {
                QString userstr = (const char *)userdata;
                //if(g_sbox_process->f_debug_module_list.contains(userstr))
                {
                    qDebug() << "[SBOX_GetProcAddress()] <from>(2)" << userstr << fname << module << name;
                }
            }
#endif
            return MemoryGetProcAddress(sbox_module.f_hmodule, name);
        }
    }
#endif
    if((quint32)name < 1000)
    {
        return (FARPROC) GetProcAddress((HMODULE) module, name);
    }
    QString v_name = name;
    if(v_name=="LoadLibraryA")
    {
        return (FARPROC) _LoadLibraryA;
    }
    if(v_name=="LoadLibraryW")
    {
        return (FARPROC) _LoadLibraryW;
    }
    if(v_name=="GetProcAddress")
    {
        qDebug() << "    GetProcAddress() requested";
        return (FARPROC) My_GetProcAddress;
    }
    return (FARPROC) GetProcAddress((HMODULE) module, name);
}

void SBOX_FreeLibrary(HCUSTOMMODULE module, void *userdata)
{
    Q_UNUSED(userdata);
    Q_UNUSED(module); //FreeLibrary((HMODULE) module);
}

int RunFromMemory(const QString &fileName)
{
    qDebug() << "[RunFromMemory()]" << fileName;
    int result = -1;
    QFile v_file(fileName);
    if(!v_file.open(QIODevice::ReadOnly)) return result;
    QByteArray v_bytes = v_file.readAll();
    HMEMORYMODULE handle = MemoryLoadLibraryEx(v_bytes.constData(), SBOX_LoadLibrary, SBOX_GetProcAddress, SBOX_FreeLibrary, NULL);
    qDebug() << "[RunFromMemory()]" << fileName << handle;
    if (handle == NULL)
    {
        qDebug().noquote() << "Can't load library from memory.";
        goto exit;
    }
    {
    QFileInfo fi(fileName);
    g_sbox_process->register_module(handle, fi.fileName());
    g_sbox_process->alloc_main_thread();
    result = MemoryCallEntryPoint(handle);
    }
    if (result < 0) {
        qDebug().noquote() << "Could not execute entry point: " << result;
    }
    MemoryFreeLibrary(handle);
    exit:
    //if (data)
    //free(data);
    return result;
}

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
    f_num_extended_tls = 64; // FIXME
    //f_num_extended_tls = 16; // FIXME
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
bool SBOX_PROCESS::register_dll_location(const QString &fullPath)
{
    QFileInfo fi(fullPath);
    QString fileName = fi.fileName().toLower();
    this->f_dll_loc_map.insert(fileName, fullPath);
    ////qDebug().noquote() << this->f_dll_loc_map;
    return true;
}
bool SBOX_PROCESS::register_module(HMEMORYMODULE hModule, const QString &baseName)
{
    SBOX_DBG("SBOX_PROCESS::register_module(0x%08x): %s", hModule, baseName.toLocal8Bit().constData());
    PMEMORYMODULE pModule = (PMEMORYMODULE)hModule;
    unsigned char *codeBase = pModule->codeBase;
    assert(sizeof(pModule)==4);
#if 0x0
    SBOX_DBG("SBOX_PROCESS::register_module(0x%08x)", pModule);
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)codeBase;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        SBOX_DBG("SBOX_PROCESS::register_module(0x%08x): DOS Signature invalid", pModule);
		return false;
	}
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(((DWORD)codeBase) + pDosHeader->e_lfanew);
	if(pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
        SBOX_DBG("SBOX_PROCESS::register_module(0x%08x): NT Signature mismatch", pModule);
		return false;
	}
	//std::list<SBOX_MODULE> f_sbox_module_list;
#endif
	SBOX_MODULE v_sbox_module;
    v_sbox_module.f_hmodule = pModule;
    v_sbox_module.f_basename = baseName;
#if 0x1
    v_sbox_module.f_is_dll = pModule->isDLL;
    if (pModule->headers->OptionalHeader.AddressOfEntryPoint != 0) {
        v_sbox_module.f_main_addr = (FARPROC) (codeBase + pModule->headers->OptionalHeader.AddressOfEntryPoint);
    } else {
        v_sbox_module.f_main_addr = NULL;
    }
    SBOX_DBG("SBOX_PROCESS::register_module(0x%08x): %s is_dll=%i main_addr=0x%08x", hModule, baseName.toLocal8Bit().constData(), v_sbox_module.f_is_dll, v_sbox_module.f_main_addr);
#endif
    PIMAGE_TLS_DIRECTORY v_tls_dir = NULL;

    qDebug() << "[pModule->headers->OptionalHeader.NumberOfRvaAndSizes]" << pModule->headers->OptionalHeader.NumberOfRvaAndSizes;

    PIMAGE_DATA_DIRECTORY v_dir = GET_HEADER_DICTIONARY(pModule, IMAGE_DIRECTORY_ENTRY_TLS);
    if (v_dir->VirtualAddress != 0) {
        qDebug() << "[v_dir->VirtualAddress]" << v_dir->VirtualAddress;
        v_tls_dir = (PIMAGE_TLS_DIRECTORY) (codeBase + v_dir->VirtualAddress);
    }
    SBOX_DBG("SBOX_PROCESS::register_module(0x%08x): v_tls_dir=0x%08x", pModule, v_tls_dir);
    if(v_tls_dir)
	{
		assert(v_tls_dir->AddressOfIndex);
		v_sbox_module.f_tls_dir = v_tls_dir;
		//((DWORD *)v_tls_dir->AddressOfIndex)[0] = f_num_implicit_tls + f_sbox_module_list.size();
		//v_sbox_module.f_tls_index = (LONG)((DWORD *)v_tls_dir->AddressOfIndex)[0];
		v_sbox_module.f_tls_index = f_num_implicit_tls + f_sbox_module_list.size();
#if 0x0
		((DWORD *)v_tls_dir->AddressOfIndex)[0] = v_sbox_module.f_tls_index;
#else
        DWORD *v_addr_of_idx = (DWORD *)v_tls_dir->AddressOfIndex;
        *v_addr_of_idx = v_sbox_module.f_tls_index;
#endif
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
            if(v_tls_dir->SizeOfZeroFill)
            {
                memset(&v_sbox_module.f_tls_raw_data[v_tls_raw_data_len], 0, v_tls_dir->SizeOfZeroFill);
            }
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
void SBOX_PROCESS::attach_after_load()
{
    PWINE_TEB f_teb = WineNtCurrentTeb();
    assert(f_teb);
    SBOX_THREAD *thread = f_thread_list[f_teb];
    SBOX_DBG("SBOX_PROCESS::attach_after_load(): thread=0x%08x", thread);
    thread->attach_explicit(true);
}
SBOX_MODULE *SBOX_PROCESS::find_mdule_by_name(const QString &path)
{
    QFileInfo fi(path);
    QString fname = fi.fileName().toLower();
    for(size_t i=0; i<f_sbox_module_list.size(); i++)
    {
        SBOX_MODULE &module = f_sbox_module_list[i];
        if(fname == module.f_basename)
        {
            return &module;
        }
    }
    return NULL;
}
SBOX_MODULE *SBOX_PROCESS::find_mdule_by_codebase(PVOID codeBase)
{
    for(size_t i=0; i<f_sbox_module_list.size(); i++)
    {
        SBOX_MODULE &module = f_sbox_module_list[i];
        PMEMORYMODULE pModule = (PMEMORYMODULE)module.f_hmodule;
        if(codeBase == pModule->codeBase)
        {
            return &module;
        }
    }
    return NULL;
}
SBOX_MODULE *SBOX_PROCESS::find_mdule_by_handle(HMEMORYMODULE handle)
{
    for(size_t i=0; i<f_sbox_module_list.size(); i++)
    {
        SBOX_MODULE &module = f_sbox_module_list[i];
        if(handle == module.f_hmodule)
        {
            return &module;
        }
    }
    return NULL;
}
SBOX_PROCESS *g_sbox_process = new SBOX_PROCESS();

int SBOX_THREAD::attach_explicit(bool as_process)
{
    assert(g_sbox_process);
    assert(g_sbox_process->f_teb);
    f_teb = WineNtCurrentTeb();
    assert(f_teb);
    as_process = as_process | f_is_root;
    SBOX_DBG("SBOX_THREAD: f_sbox_tlsp=0x%08x", f_sbox_tlsp);
    SBOX_DBG("SBOX_THREAD: g_sbox_process->f_sbox_module_list.size()=%u", g_sbox_process->f_sbox_module_list.size());
    for(size_t i=0; i<g_sbox_process->f_sbox_module_list.size(); i++)
    {
        SBOX_DBG("SBOX_THREAD: prepare? for g_sbox_process->f_sbox_module_list[%u]", i);
        SBOX_MODULE &v_sbox_module = g_sbox_process->f_sbox_module_list[i];
        if(v_sbox_module.isAttached)
        {
            continue;
        }
        /* DllMain() */
        SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](G): name=%s is_dll=%d main_addr=0x%08x", i, v_sbox_module.f_basename.toLocal8Bit().constData(), v_sbox_module.f_is_dll, v_sbox_module.f_main_addr);
        if(v_sbox_module.f_is_dll && v_sbox_module.f_main_addr)
        {
            SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](H): name=%s is_dll=%d main_addr=0x%08x", i, v_sbox_module.f_basename.toLocal8Bit().constData(), v_sbox_module.f_is_dll, v_sbox_module.f_main_addr);
            PMEMORYMODULE pModule = (PMEMORYMODULE)v_sbox_module.f_hmodule;
            unsigned char *codeBase = pModule->codeBase;
            DllEntryProc DllEntry = (DllEntryProc) v_sbox_module.f_main_addr;
            BOOL successfull = FALSE;
            successfull = (*DllEntry)((HINSTANCE)codeBase, DLL_PROCESS_ATTACH, 0);
            if(successfull)
            {
                v_sbox_module.isAttached = true;
            }
        }
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
            assert(sizeof(v_tls_raw_data)==sizeof(DWORD));
            SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](E)v_sbox_module.f_tls_index=%d", i, v_sbox_module.f_tls_index);
            ((DWORD *)f_teb->ThreadLocalStoragePointer)[v_sbox_module.f_tls_index] = (DWORD)v_tls_raw_data;
            SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](F)", i);
            for(size_t i_callback=0; i_callback<v_sbox_module.f_tls_callback_list.size(); i_callback++)
            {
                SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](G) i_callback=%u", i, i_callback);
                v_sbox_module.f_tls_callback_list[i_callback]((PVOID)g_sbox_process->f_hmodule, DLL_PROCESS_ATTACH, 0);
                SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](H) i_callback=%u", i, i_callback);
            }
        }
    }
    //SBOX_DBG("SBOX_THREAD created: 0x%08x ORIG_TLSP=0x%08x SBOX_TLSP=0x%08x%s", f_teb, f_orig_tlsp, f_sbox_tlsp, f_is_root?" (ROOT)":"");
    SBOX_DBG("SBOX_THREAD created: 0x%08x%s", f_teb, f_is_root?" (ROOT)":"");
    return 0;
}
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
    g_sbox_process->f_thread_list.insert(f_teb, this);
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
    SBOX_DBG("SBOX_THREAD: g_sbox_process->f_sbox_module_list.size()=%u", g_sbox_process->f_sbox_module_list.size());
    for(size_t i=0; i<g_sbox_process->f_sbox_module_list.size(); i++)
	{
        SBOX_DBG("SBOX_THREAD: prepare? for g_sbox_process->f_sbox_module_list[%u]", i);
		SBOX_MODULE &v_sbox_module = g_sbox_process->f_sbox_module_list[i];
        /* DllMain() */
        SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](G): name=%s is_dll=%d main_addr=0x%08x", i, v_sbox_module.f_basename.toLocal8Bit().constData(), v_sbox_module.f_is_dll, v_sbox_module.f_main_addr);
        if(v_sbox_module.f_is_dll && v_sbox_module.f_main_addr)
        {
            SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](H): name=%s is_dll=%d main_addr=0x%08x", i, v_sbox_module.f_basename.toLocal8Bit().constData(), v_sbox_module.f_is_dll, v_sbox_module.f_main_addr);
            PMEMORYMODULE pModule = (PMEMORYMODULE)v_sbox_module.f_hmodule;
            unsigned char *codeBase = pModule->codeBase;
            DllEntryProc DllEntry = (DllEntryProc) v_sbox_module.f_main_addr;
            BOOL successfull = FALSE;
            if(f_is_root)
            {
                successfull = (*DllEntry)((HINSTANCE)codeBase, DLL_PROCESS_ATTACH, 0);
                if(successfull)
                {
                    v_sbox_module.isAttached = true;
                }
                else
                {
                    exit(1234);
                }
            }
            else
            {
                successfull = (*DllEntry)((HINSTANCE)codeBase, DLL_THREAD_ATTACH, 0);
            }
        }
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
			assert(sizeof(v_tls_raw_data)==sizeof(DWORD));
			SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](E)v_sbox_module.f_tls_index=%d", i, v_sbox_module.f_tls_index);
			((DWORD *)f_teb->ThreadLocalStoragePointer)[v_sbox_module.f_tls_index] = (DWORD)v_tls_raw_data;
			SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](F)", i);
            for(size_t i_callback=0; i_callback<v_sbox_module.f_tls_callback_list.size(); i_callback++)
            {
                SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](G) i_callback=%u", i, i_callback);
                if(f_is_root)
                {
                    v_sbox_module.f_tls_callback_list[i_callback]((PVOID)g_sbox_process->f_hmodule, DLL_PROCESS_ATTACH, 0);
                }
                else
                {
                    v_sbox_module.f_tls_callback_list[i_callback]((PVOID)g_sbox_process->f_hmodule, DLL_THREAD_ATTACH, 0);
                }
                SBOX_DBG("SBOX_THREAD: prepare for g_sbox_process->f_sbox_module_list[%u](H) i_callback=%u", i, i_callback);
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
        if(v_sbox_module.f_is_dll && v_sbox_module.f_main_addr)
        {
            SBOX_DBG("~SBOX_THREAD: unprepare for g_sbox_process->f_sbox_module_list[%u](H): name=%s is_dll=%d main_addr=0x%08x", i, v_sbox_module.f_basename.toLocal8Bit().constData(), v_sbox_module.f_is_dll, v_sbox_module.f_main_addr);
            PMEMORYMODULE pModule = (PMEMORYMODULE)v_sbox_module.f_hmodule;
            unsigned char *codeBase = pModule->codeBase;
            DllEntryProc DllEntry = (DllEntryProc) v_sbox_module.f_main_addr;
            BOOL successfull = FALSE;
            if(f_is_root)
            {
                successfull = (*DllEntry)((HINSTANCE)codeBase, DLL_PROCESS_DETACH, 0);
            }
            else
            {
                successfull = (*DllEntry)((HINSTANCE)codeBase, DLL_THREAD_DETACH, 0);
            }
        }
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
    g_sbox_process->f_thread_list.remove(f_teb);
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
