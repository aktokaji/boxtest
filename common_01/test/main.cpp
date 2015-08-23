#undef NDEBUG
#include <assert.h>

#ifdef UNICODE
//#define _UNICODE
#endif

#include <windows.h>
#include "win32.h"
#include "win32_print.h"
#include "win32_vector.h"
#include "vardecl.h"

#include "MemoryModule.h"

#include "sbox.h"
#include "wine.h"

#include <tchar.h>

//#include <mutex>

#include <QtCore>
#include <QtGlobal>

#include <iostream>

#ifdef _DEBUG
#define DBG(format, ...) win32_printfA("[WMAIN(A)] " format "\n", ## __VA_ARGS__)
#define DBGW(format, ...) win32_printfW(L"[WMAIN(W)] " format L"\n", ## __VA_ARGS__)
#else
#define DBG(format, ...) (void)0;
#define DBGW(format, ...) (void)0;
#endif

void myMsgHandler(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    OutputDebugStringW(msg.toStdWString().c_str());
    OutputDebugStringW(L"\n");
    std::cout << msg.toLocal8Bit().constData() << std::endl;
}

struct win32_module
{
	HMODULE BaseAddress = 0;
	DWORD   SizeOfImage;
	LPVOID  EntryPoint;
	LONG    TlsIndex = -1;
	win32::global_wstring FileFullPath = L"AAA";
	win32::global_wstring FileLocalName = L"BBB";
	win32::global_string f_str = "[f_str]:漢字";
	explicit win32_module()
	{
		DBG("win32_module()");
	}
	virtual ~win32_module()
	{
		DBG("~win32_module()");
	}
};
typedef win32::global_vector<win32_module> win32_module_vector;

struct main_struct
{
public:
	const char *f_label;
	CRITICAL_SECTION f_csect;
	int f_a = 0;
    win32::global_string f_str = "[f_str]:漢字";
	explicit main_struct(const char* label): f_label(label)
	{
		//DBG("main_struct(1): %s (f_str=%s)", f_label, f_str.c_str());
		DBG("main_struct(1): %s", f_label);
		f_a = 5678;
		DBG("main_struct(2): %s", f_label);
		InitializeCriticalSection(&f_csect);
		DBG("main_struct(3): %s", f_label);
	}
	virtual ~main_struct()
	{
		DBG("~main_struct(1): %s", f_label);
		DeleteCriticalSection(&f_csect);
		DBG("~main_struct(2): %s", f_label);
	}
};
////main_struct l_main_struct("l_main_struct");
main_struct *l_main_struct2 = NULL;

main_struct *l_main_struct3 = NULL;
ALIGNED_ARRAY_DECL(char, l_main_struct_area_3, sizeof(*l_main_struct3), 16);

static void on_process_attach(void)
{
    qInstallMessageHandler(myMsgHandler);
	{
		LPVOID v_addr = VirtualAlloc(
			(LPVOID)0x400000, //LPVOID lpAddress,        // 予約またはコミットしたい領域
			//256*1024*1024, //SIZE_T dwSize,           // 領域のサイズ
			//128*1024*1024, //SIZE_T dwSize,           // 領域のサイズ
			//1*1024*1024, //SIZE_T dwSize,           // 領域のサイズ
			4096, //SIZE_T dwSize,           // 領域のサイズ
			MEM_RESERVE, //DWORD flAllocationType,  // 割り当てのタイプ
			PAGE_READWRITE //DWORD flProtect          // アクセス保護のタイプ
		);
		DBG("v_addr=0x%08x", v_addr);
	}
	DBG("DLL_PROCESS_ATTACH(2)");
	{
		void *v_memory = win32_global_malloc(sizeof(main_struct));
		l_main_struct2 = new (v_memory) main_struct("l_main_struct2");
		l_main_struct2->~main_struct();
		win32_global_free(v_memory);
	}
	{
		DBG("l_main_struct_area_3=0x%08x", l_main_struct_area_3);
		l_main_struct3 = new (l_main_struct_area_3) main_struct("l_main_struct3");
		l_main_struct3->~main_struct();
	}
	DBG("DLL_PROCESS_ATTACH(3)");
	{
		DBG("DLL_PROCESS_ATTACH(4)");
		main_struct v_main_struct("v_main_struct");
		DBG("DLL_PROCESS_ATTACH(5)");
		v_main_struct.f_a = 1111;
		DBG("DLL_PROCESS_ATTACH(6)");
	}
	{
		DBG("DLL_PROCESS_ATTACH(7)");
		std::basic_string< char, std::char_traits<char>, win32::global_allocator<char> > mys;
		mys = "xyz";
		DBG("mys=%s", mys.c_str());
		win32::global_string mys2 = "漢字";
		DBG("mys2=%s", mys2.c_str());
		win32::global_vector<int> v;
		v.push_back(1111);
		v.push_back(2222);
		v.push_back(3333);
		DBG("v.size()=%u", v.size());
		win32_module_vector v2;
		win32_module v_added;
		v2.push_back(v_added);
		DBG("v2[0].f_str.c_str()=%s", v2[0].f_str.c_str());
		DBG("v2.size()=%u", v2.size());
        //std::recursive_mutex v_mutex;
		DBG("DLL_PROCESS_ATTACH(8)");
	}
}

#define TLS_CALLBACK_SECT ".CRT$XLB"
static void NTAPI main_tls_callback(PVOID hModule, DWORD dwReason, PVOID pReserved)
{
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(pReserved);
	DBG("main_tls_callback(%s): dwReason=0x%08x (%s)", TLS_CALLBACK_SECT, dwReason, win32_tls_callback_reason_label(dwReason));
    ////win32::global_string f_str = "[f_str]:漢字";
    ////qDebug() << f_str;
    switch(dwReason){
	case DLL_PROCESS_ATTACH:
		DBG("DLL_PROCESS_ATTACH(start)");
		win32_debug_set(true);
		on_process_attach();
		DBG("DLL_PROCESS_ATTACH(end)");
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		DBG("DLL_THREAD_DETACH");
		break;
	default:
		DBG("default: %u", dwReason);
		break;
	}
	DBG("main_tls_callback(%s)[end]: dwReason=0x%08x (%s)", TLS_CALLBACK_SECT, dwReason, win32_tls_callback_reason_label(dwReason));
}
TLS_CALLBACK_DECL(TLS_CALLBACK_SECT, __main_tls_callback__, main_tls_callback);

//static int RunFromMemory(void);
static int RunFromMemory(const QString &fileName);

//int wmain(int argc, wchar_t *argv[])
int main(int argc, char *argv[])
{
    //UNREFERENCED_PARAMETER(argc);
    //UNREFERENCED_PARAMETER(argv);

    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();

    qDebug() << "[main(1)]" << args.size() << args;

    qDebug().noquote() << L"漢字テスト";

    win32_printfA("A:args[0]=%s\n", args[0].toLatin1().constData());
    printf("B:args[0]=%s\n", args[0].toLatin1().constData());

#if 0x0
    if(args.size()<2)
    {
        //qDebug() << L"ファイル名を指定してください！";
        QProcess v_proc;
        v_proc.setNativeProgram(args[0]);
        v_proc.setNativeArguments("http://www.google.com");
        qDebug() << v_proc.nativeProgram();
        v_proc.start("dummy.exe");
        v_proc.waitForFinished(-1);
        QByteArray v_stdout = v_proc.readAll();
        QString v_string = QString::fromLocal8Bit(v_stdout);
        qDebug().noquote() << v_string;
        return 0;
    }
#endif

	DBG("wmain() called");
    win32::global_string f_str2 = "[f_str2]:漢字";
    DBG("f_str2=%s", f_str2.c_str());
    qDebug().noquote() << f_str2;
    qDebug().noquote() << QString::fromWCharArray(L"abcえーびーしーx©®");

	win32_debug_set(true);

#if 0x1
	HANDLE v_thread_heap = win32_thread_heap();
	DBG("wmain(): v_thread_heap=0x%08x", v_thread_heap);
	void *v_memory = win32_thread_malloc(1024);
	DBG("wmain(): v_memory=0x%08x", v_memory);
	void *v_memory2 = win32_thread_malloc(1024);
	DBG("wmain(): v_memory2=0x%08x", v_memory2);
	size_t v_size2 = win32_thread_memlen(v_memory2);
	DBG("wmain(): v_size2=%u", v_size2);
	void *v_memory3 = win32_thread_realloc(v_memory2, 1026);
	DBG("wmain(): v_memory3=0x%08x", v_memory3);
	size_t v_size3 = win32_thread_memlen(v_memory3);
	DBG("wmain(): v_size3=%u", v_size3);
	win32_thread_free(v_memory);
#endif
	static TLS_VARIABLE_DECL char *s_vector = NULL;
	size_t v_memlen1 = win32_thread_memlen(s_vector);
	DBG("wmain(): v_memlen1=%u", v_memlen1);
	DBG("wmain(): win32_thread_char_size(&s_vector)=%u", win32_thread_char_size(&s_vector));
	win32_thread_char_resize(&s_vector, 5);
	size_t v_memlen2 = win32_thread_memlen(s_vector);
	DBG("wmain(): v_memlen2=%u", v_memlen2);
	DBG("wmain(): win32_thread_char_size(&s_vector)=%u", win32_thread_char_size(&s_vector));
	static TLS_VARIABLE_DECL char *s_vector2 = NULL;
	win32_thread_char_push_back(&s_vector2, 'a');
	win32_thread_char_push_back(&s_vector2, 'b');
	win32_thread_char_push_back(&s_vector2, ' ');
	win32_thread_char_push_back(&s_vector2, 'c');
	win32_thread_char_push_back(&s_vector2, 0);
	win32_thread_char_insert(&s_vector2, 0, 'Z');
	DBG("wmain(): win32_thread_char_size(&s_vector2)=%u", win32_thread_char_size(&s_vector2));
	DBG("wmain(): s_vector2=%s", s_vector2);
	char erase1 = win32_thread_char_erase(&s_vector2, 0); UNREFERENCED_PARAMETER(erase1);
	char erase2 = win32_thread_char_erase(&s_vector2, 1); UNREFERENCED_PARAMETER(erase2);
	DBG("wmain(): s_vector2(after erase)=%s", s_vector2);
	DBG("wmain(): win32_thread_char_size(&s_vector2)=%u", win32_thread_char_size(&s_vector2));
	//win32_thread_char_resize(&s_vector2, 0);
	//win32_thread_char_clear(&s_vector2);
	DBG("ワイド文字列1＝%S", L"漢字");
	DBG("ワイド文字列2＝%s", L"漢字");
	DBGW(L"ワイド文字列3＝%s", L"漢字");
	DBG("zero char(1)='%c'", '\0');
	DBGW(L"zero char(2)='%c'", L'\0');
	//std::wstring wstring_printf(const wchar_t *format, ...);
	std::wstring v_zero_wstr = wstring_printf(L"zero char(3)='%c'", L'\0');
	DBGW(L"%s", v_zero_wstr.c_str());

	win32_thread_char_reserve(&s_vector2, 20);
	DBG("wmain(): win32_thread_char_size(&s_vector2)=%u", win32_thread_char_size(&s_vector2));
	win32_thread_char_reserve(&s_vector2, 15);
	DBG("wmain(): win32_thread_char_size(&s_vector2)=%u", win32_thread_char_size(&s_vector2));
	
	char c0 = win32_thread_char_get(&s_vector2, 0);
	DBG("wmain(): c0=%c", c0);
	win32_thread_char_set(&s_vector2, 4096, 'X');
	char c4096 = win32_thread_char_get(&s_vector2, 4096);
	DBG("wmain(): c4096=%d (%c)", c4096, c4096);
	DBG("wmain(): win32_thread_char_size(&s_vector2)=%u", win32_thread_char_size(&s_vector2));

	static TLS_VARIABLE_DECL wchar_t *s_wvector = NULL;
	win32_thread_wchar_push_back(&s_wvector, L'A');
	win32_thread_wchar_push_back(&s_wvector, L'B');
	win32_thread_wchar_push_back(&s_wvector, L'C');
	win32_thread_wchar_push_back(&s_wvector, 0);
	DBGW(L"wmain(): s_wvector=%s", s_wvector);
	wchar_t ec1 = win32_thread_wchar_erase(&s_wvector, 1);
	DBGW(L"wmain(): ec1=%c", ec1);
	DBGW(L"wmain(): s_wvector=%s", s_wvector);
	win32_thread_wchar_insert(&s_wvector, 1, L'漢');
	DBGW(L"wmain(): s_wvector=%s", s_wvector);

    RunFromMemory("E:\\browser.exe");
    //RunFromMemory("E:\\testbed\\tlscb.exe");

	DBG("wmain(end))");

	return 0;
}

//#define EXE_FILE TEXT("V:\\QtBuild\\#svn\\qt5.4.0\\#labo\\tls_01\\release\\main.exe")
//#define EXE_FILE TEXT("V:\\QtBuild\\#svn\\qt5.4.0\\#labo\\browser5.4.0\\release\\browser.exe")
//#define EXE_FILE TEXT("E:\\browser.exe")
extern "C" static LPWSTR __stdcall _GetCommandLineW(VOID)
{
    qDebug() << "[_GetCommandLineW(VOID)]";
    return (LPWSTR)L"browser.exe http://www.google.com";
}

static HCUSTOMMODULE _LoadLibrary(LPCSTR filename, void *userdata)
{
    Q_UNUSED(userdata);
    HMODULE result = LoadLibraryA(filename);
    if (result == NULL) {
        return NULL;
    }

    return (HCUSTOMMODULE) result;
}

static FARPROC _GetProcAddress(HCUSTOMMODULE module, LPCSTR name, void *userdata)
{
    Q_UNUSED(userdata);
    QString v_name = name;
    if(v_name=="GetCommandLineW")
    {
        //GetCommandLineW();
        qDebug() << "[_GetProcAddress]" << (const char *)name << QString::fromStdWString(WineGetModuleFileName((HMODULE)module));
        return (FARPROC)_GetCommandLineW;
    }
    return (FARPROC) GetProcAddress((HMODULE) module, name);
}

static void _FreeLibrary(HCUSTOMMODULE module, void *userdata)
{
    Q_UNUSED(userdata);
    FreeLibrary((HMODULE) module);
}

int RunFromMemory(const QString &fileName)
{
	FILE *fp;
	unsigned char *data=NULL;
	size_t size;
	HMEMORYMODULE handle;
	int result = -1;
    //	fp = _tfopen(EXE_FILE, _T("rb"));
    fp = _tfopen(fileName.toStdWString().c_str(), _T("rb"));
    if (fp == NULL)
	{
    _tprintf(_T("Can't open executable \"%s\"."), fileName.toStdWString().c_str());
	goto exit;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	data = (unsigned char *)malloc(size);
	fseek(fp, 0, SEEK_SET);
	fread(data, 1, size, fp);
	fclose(fp);
#if 0x0
	handle = MemoryLoadLibrary(data);
#else
    handle = MemoryLoadLibraryEx(data, _LoadLibrary, _GetProcAddress, _FreeLibrary, NULL);
#endif
	if (handle == NULL)
	{
	_tprintf(_T("Can't load library from memory.\n"));
	goto exit;
	}
#if 0x1
    g_sbox_process->register_module((PMEMORYMODULE)handle);
    g_sbox_process->alloc_main_thread();
#endif
	result = MemoryCallEntryPoint(handle);
	if (result < 0) {
	_tprintf(_T("Could not execute entry point: %d\n"), result);
	}
	MemoryFreeLibrary(handle);
	exit:
	if (data)
	free(data);
	return result;
}

