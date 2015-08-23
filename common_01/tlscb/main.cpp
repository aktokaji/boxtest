#undef NDEBUG
#include <assert.h>

#include <windows.h>
//#include "win32.h"
#include "win32_print.h"
//#include "win32_vector.h"
#include "vardecl.h"

//#include "MemoryModule.h"

//#include "sbox.h"
//#include "wine.h"

#include <tchar.h>

#include <QtCore>
#include <QtGlobal>

#include <iostream>
#include <thread>

#ifdef _DEBUG
#define DBG(format, ...) win32_printfA("[TLSCB(A)] " format "\n", ## __VA_ARGS__)
#define DBGW(format, ...) win32_printfW(L"[TLSCB(W)] " format L"\n", ## __VA_ARGS__)
#else
#define DBG(format, ...) (void)0;
#define DBGW(format, ...) (void)0;
#endif

TLS_VARIABLE_DECL int tv_int = 1234;

void myMsgHandler(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    OutputDebugStringW(msg.toStdWString().c_str());
    OutputDebugStringW(L"\n");
    std::cout << msg.toLocal8Bit().constData() << std::endl;
}

static void on_process_attach(void)
{
    qInstallMessageHandler(myMsgHandler);
    //DBG("DLL_PROCESS_ATTACH()");
}

static const char *win32_tls_callback_reason_label(DWORD dwReason)
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
static void NTAPI main_tls_callback(PVOID hModule, DWORD dwReason, PVOID pReserved)
{
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(pReserved);
	DBG("main_tls_callback(%s): dwReason=0x%08x (%s)", TLS_CALLBACK_SECT, dwReason, win32_tls_callback_reason_label(dwReason));
    printf("main_tls_callback(%s): dwReason=0x%08x (%s)\n", TLS_CALLBACK_SECT, dwReason, win32_tls_callback_reason_label(dwReason));
    switch(dwReason){
	case DLL_PROCESS_ATTACH:
		on_process_attach();
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
        printf("tv_int=%d\n", tv_int++);
		break;
	case DLL_THREAD_DETACH:
		break;
	default:
		DBG("default: %u", dwReason);
		break;
	}
    //DBG("main_tls_callback(%s)[end]: dwReason=0x%08x (%s)", TLS_CALLBACK_SECT, dwReason, win32_tls_callback_reason_label(dwReason));
}
TLS_CALLBACK_DECL(TLS_CALLBACK_SECT, __main_tls_callback__, main_tls_callback);

int main(int argc, char *argv[])
{
    //UNREFERENCED_PARAMETER(argc);
    //UNREFERENCED_PARAMETER(argv);

    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();

    qDebug() << "[main(1)]" << args.size() << args;

    qDebug() << "[main(start)]: tv_int++" << tv_int++;

    win32_printfA("A:args[0]=%s\n", args[0].toLatin1().constData());
    printf("B:args[0]=%s\n", args[0].toLatin1().constData());

    int i=0;
    printf("C:i=%d\n", i);
    std::thread t1([&i](int x) {
        printf("[tv_int](1)=%d\n", tv_int);
            int counter = 0;
            while(counter++ < x) {
                i += counter;
            }
        }, 10);
    t1.join();
    printf("D:i=%d\n", i);

    int j=0;
    printf("E:j=%d\n", j);
    std::thread t2([&j](int x) {
        printf("[tv_int](2)=%d\n", tv_int);
            int counter = 0;
            while(counter++ < x) {
                j += counter;
            }
        }, 11);
    t2.join();
    printf("F:j=%d\n", j);

    qDebug() << "[main(end)]: tv_int++" << tv_int++;

    return 0;
}
