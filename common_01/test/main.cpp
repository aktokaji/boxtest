#undef NDEBUG
#include <assert.h>

#include "sbox.h"

#include <QtCore>
#include <QtGlobal>
#include <iostream>

//#include "win32_print.h"
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

//int wmain(int argc, wchar_t *argv[])
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qInstallMessageHandler(myMsgHandler);

    QStringList args = app.arguments();

    qDebug() << "[main(1)]" << args.size() << args;

    qDebug().noquote() << L"漢字テスト";

    //win32_printfA("A:args[0]=%s\n", args[0].toLatin1().constData());
    //printf("B:args[0]=%s\n", args[0].toLatin1().constData());

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
    qDebug().noquote() << "main(restored)";

	return 0;
}

#if 0x0
extern "C" static LPWSTR __stdcall _GetCommandLineW(VOID)
{
    qDebug() << "[_GetCommandLineW(VOID)]";
    return (LPWSTR)L"browser.exe http://www.google.com";
}
#endif

