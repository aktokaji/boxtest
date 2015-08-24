#undef NDEBUG
#include <assert.h>

#include "sbox.h"

#include <QtCore>
#include <QtGlobal>
#include <QtScript>
#include <iostream>

//#include "win32_print.h"
//#ifdef _DEBUG
//#define DBG(format, ...) win32_printfA("[WMAIN(A)] " format "\n", ## __VA_ARGS__)
//#define DBGW(format, ...) win32_printfW(L"[WMAIN(W)] " format L"\n", ## __VA_ARGS__)
//#else
//#define DBG(format, ...) (void)0;
//#define DBGW(format, ...) (void)0;
//#endif

class MyLoader : public QObject
{
    Q_OBJECT
public:
    MyLoader();
public slots:
    int calculate(int value) const;
    void addLoadPath(const QString &path)
    {
        qDebug().noquote() << "[addLoadPath()]" << path;
        this->loadPathList.append(path);
    }
public:
    QList<QDir> loadPathList;
};

MyLoader::MyLoader()
{
}

int MyLoader::calculate(int value) const
{
    int total = 0;
    for (int i = 0; i <= value; ++i)
        total += i;
    return total;
}

static
QStringList myArgList()
{
    QStringList result;
    LPWSTR cmdLine = GetCommandLineW();
    int numArgs = 0;
    LPWSTR *args = CommandLineToArgvW(cmdLine, &numArgs);
    for(int i=0; i<numArgs; i++)
    {
        result << QString::fromWCharArray(args[i]);
    }
    return result;
}

static
void myMsgHandler(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    OutputDebugStringW(L"[DEBUG] ");
    OutputDebugStringW(msg.toStdWString().c_str());
    OutputDebugStringW(L"\n");
    //std::cout << msg.toLocal8Bit().constData() << std::endl;
}

//int wmain(int argc, wchar_t *argv[])
int main(int argc, char *argv[])
//int main(int, char *[])
{
    qInstallMessageHandler(myMsgHandler);
    MyLoader loader;
    {
        QCoreApplication app(argc, argv);
        Q_INIT_RESOURCE(main);
        QFile scriptFile(":/main.qs");
        scriptFile.open(QIODevice::ReadOnly);
        QByteArray scriptBytes = scriptFile.readAll();
        qDebug().noquote() << "[scriptBytes]" << scriptBytes;

        QScriptEngine engine;
        QObject *someObject = &loader;
        QScriptValue objectValue = engine.newQObject(someObject);
        engine.globalObject().setProperty("loader", objectValue);

        qDebug() << "myObject's calculate() function returns"
                 << engine.evaluate("loader.calculate(10)").toNumber();
        engine.evaluate(QString::fromUtf8(scriptBytes));
    }

    //QStringList args = app.arguments();

    //qDebug() << "[main(1)]" << args.size() << args;

    qDebug().noquote() << QString::fromWCharArray(L"漢字テスト");

    QStringList args = myArgList();
    qDebug().noquote() << args;

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

    for(int i=0; i<loader.loadPathList.size(); i++)
    {
        QDir dir = loader.loadPathList[i];
        // 対象のファイル名フィルタ
        QStringList nameFilters;
        nameFilters << "*.dll";
        // ファイルフィルタ
        //QDir::Filters filters = QDir::Files | QDir::Dirs | QDir::Hidden;
        QDir::Filters filters = QDir::Files | QDir::Hidden;
        // 対象フラグ
        QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags;
        // サブディレクトリを探索するかどうか
        bool canSerchSubDir= true;
        if(canSerchSubDir)
        {
            flags = QDirIterator::Subdirectories;
        }
        // イテレーターの生成
        QDirIterator it(dir.absolutePath(), nameFilters, filters, flags);
        QStringList files;
        while (it.hasNext())
        {
            QString file = it.next();
            if(file.endsWith("/.")) continue;
            if(file.endsWith("/..")) continue;
            //qDebug().noquote() << dir.relativeFilePath(file) << file;
#if 0x1
            g_sbox_process->register_dll_location(file);
#else
            QFileInfo fi(file);
            if(fi.fileName().toLower().startsWith("qt5webkit"))
            {
                g_sbox_process->register_dll_location(file);
            }
#endif
            files << file;
        }
    }
    //QString dirPath = QString::fromWCharArray(L"E:/testbed/圧縮テスト");
    //QDir dir(dirPath);

    //RunFromMemory("E:\\browser.exe");
    //RunFromMemory("E:\\testbed\\tlscb.exe");
    //RunFromMemory("E:\\testbed\\browser-wk2.exe");
    RunFromMemory("E:\\testbed\\browser486.exe");


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

#include "main.moc"
