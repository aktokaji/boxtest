#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QTextEdit>
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    QApplication a(argc, argv);

    // ユーザーのPicturesフォルダパスを取得
    //QString dirPath = "E:/QtBuild5/openssl-1.0.1e";
    QString dirPath = QString::fromWCharArray(L"E:/testbed/圧縮テスト");
    QDir dir(dirPath);
    // 対象のファイル名フィルタ
    QStringList nameFilters;
    //nameFilters << "*.c" << "*.h" << "*.cpp";
    //nameFilters << "*.txt";
    nameFilters << "*";
    // ファイルフィルタ
    QDir::Filters filters = QDir::Files | QDir::Dirs | QDir::Hidden;
    // 対象フラグ
    QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags;
    // サブディレクトリを探索するかどうか
    bool canSerchSubDir= true;
    if(canSerchSubDir)
    {
        flags = QDirIterator::Subdirectories;
    }
    // イテレーターの生成
    QDirIterator it(dirPath, nameFilters, filters, flags);
    QStringList files;
    while (it.hasNext())
    {
        QString file = it.next();
        if(file.endsWith("/.")) continue;
        if(file.endsWith("/..")) continue;
        qDebug().noquote() << file;
        qDebug().noquote() << dir.relativeFilePath(file);
        files << file;
    }
    //return a.exec();

    QFile testFile("E:/testbed/foltree.zip");
    if (testFile.exists()) {
        if (!testFile.remove()) {
            qDebug() << "Couldn't remove existing archive to create a new one";
            exit(1);
        }
    }
    QuaZip testZip(&testFile);
    testZip.setZip64Enabled(true);
    //testZip.setFileNameCodec("Shift_JIS");
    //testZip.setFileNameCodec("utf-8");
    testZip.setFileNameCodec("UTF-8");
    //testZip.setFileNameCodec("cp65001");
    //testZip.setFileNameCodec("UTF-16");
    qDebug() << testZip.open(QuaZip::mdCreate);
    qDebug() << "[testFile.isOpen(1)]" << testFile.isOpen();
    //QString comment = "Test comment";
    //testZip.setComment(comment);
    for(int i=0; i<files.size(); i++)
    {
        QString filePath = files[i];
        QFile inFile(filePath);
        QFileInfo inFileInfo(inFile);
        QString fileName = dir.relativeFilePath(filePath);
        qDebug().noquote() << i << filePath << fileName << inFileInfo.isDir();
        if(inFileInfo.isDir())
        {
            QuaZipFile outFile(&testZip);
            QuaZipNewInfo v_new_info(fileName + "/", inFile.fileName());
            v_new_info.dateTime = inFileInfo.lastModified();
            qDebug() << outFile.open(QIODevice::WriteOnly, v_new_info);
            outFile.close();
        }
        else
        {
            if (!inFile.open(QIODevice::ReadOnly)) {
                qDebug("File name: %s", fileName.toUtf8().constData());
                exit(1);
            }
            QuaZipFile outFile(&testZip);
            QuaZipNewInfo v_new_info(fileName, inFile.fileName());
            //v_new_info.setFileDateTime(inFile.fileName());
            v_new_info.dateTime = inFileInfo.lastModified();
            //qDebug() << outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName, inFile.fileName()));
            qDebug() << outFile.open(QIODevice::WriteOnly, v_new_info);
            QByteArray v_content = inFile.readAll();
            outFile.write(v_content);
            inFile.close();
            outFile.close();
            qDebug() << outFile.getZipError();
        }
    }
    testZip.close();
    qDebug() << testZip.getZipError();

    /* 解凍 */
    QTextEdit textEdit;
    qDebug() << "[testFile.isOpen(2)]" << testFile.isOpen();
    QuaZip v_zip(&testFile);
    v_zip.setFileNameCodec("UTF-8");
    v_zip.open(QuaZip::mdUnzip);
    // boxedappsdk__demo__3_3_5_27.zip
    auto x = v_zip.getFileInfoList64();
    qDebug() << x.size();
    for(int i=0; i<x.size(); i++)
    {
        auto y = x[i];
        qDebug().noquote() << y.name << y.dateTime;
        textEdit.append(y.name);
        //textEdit.append("\n");
    }
    qDebug() << v_zip.goToFirstFile();
    QuaZipFileInfo64 v_info;
    qDebug().noquote() << v_zip.getCurrentFileInfo(&v_info);
    qDebug().noquote() << v_info.uncompressedSize;
    qDebug().noquote() << v_info.name;
    for(int i=0; i<x.size(); i++)
    {
        auto y = x[i];
        qDebug().noquote() << y.name << y.uncompressedSize << y.dateTime;
        if(!y.name.endsWith("/"))
        {
            qDebug().noquote() << v_zip.setCurrentFile(y.name);
            QuaZipFile v_qfile(&v_zip);
            qDebug().noquote() << v_qfile.open(QIODevice::ReadOnly);
            QByteArray v_bytes = v_qfile.readAll();
            qDebug().noquote() << v_bytes.size();
            //qDebug() << v_bytes;
            break;
        }
    }

    textEdit.show();
    return a.exec();
}
