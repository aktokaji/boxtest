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

//#ifdef _DEBUG
//#define DBG(format, ...) win32_printfA("[WMAIN(A)] " format "\n", ## __VA_ARGS__)
//#define DBGW(format, ...) win32_printfW(L"[WMAIN(W)] " format L"\n", ## __VA_ARGS__)
//#else
//#define DBG(format, ...) (void)0;
//#define DBGW(format, ...) (void)0;
//#endif

void myMsgHandler(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    OutputDebugStringW(msg.toStdWString().c_str());
    OutputDebugStringW(L"\n");
    std::cout << msg.toLocal8Bit().constData() << std::endl;
}

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

    RunFromMemory("E:\\browser.exe");
    //RunFromMemory("E:\\testbed\\tlscb.exe");

    //DBG("wmain(end))");

	return 0;
}

#if 0x0
extern "C" static LPWSTR __stdcall _GetCommandLineW(VOID)
{
    qDebug() << "[_GetCommandLineW(VOID)]";
    return (LPWSTR)L"browser.exe http://www.google.com";
}
#endif

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
    handle = MemoryLoadLibraryEx(data, SBOX_LoadLibrary, SBOX_GetProcAddress, SBOX_FreeLibrary, NULL);
#endif
	if (handle == NULL)
	{
	_tprintf(_T("Can't load library from memory.\n"));
	goto exit;
	}
#if 0x1
    {
    QFileInfo fi(fileName);
    g_sbox_process->register_module(handle, fi.fileName());
    g_sbox_process->alloc_main_thread();
#endif
	result = MemoryCallEntryPoint(handle);
    }
	if (result < 0) {
	_tprintf(_T("Could not execute entry point: %d\n"), result);
	}
	MemoryFreeLibrary(handle);
	exit:
	if (data)
	free(data);
	return result;
}

