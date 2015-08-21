//#include <QCoreApplication>
#include <QtCore>
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QFile v_file("E:/boxedappsdk__demo__3_3_5_27.zip");
    //v_file.open(QIODevice::ReadOnly);
    QuaZip v_zip(&v_file);
    v_zip.open(QuaZip::mdUnzip);
    // boxedappsdk__demo__3_3_5_27.zip
    auto x = v_zip.getFileInfoList64();
    qDebug() << x.size();
    for(int i=0; i<x.size(); i++)
    {
        auto y = x[i];
        qDebug() << y.name << y.dateTime;
    }
    qDebug() << v_zip.goToFirstFile();
    QuaZipFileInfo64 v_info;
    qDebug() << v_zip.getCurrentFileInfo(&v_info);
    qDebug() << v_info.uncompressedSize;
    qDebug() << v_info.name;
    for(int i=0; i<x.size(); i++)
    {
        auto y = x[i];
        qDebug() << y.name << y.uncompressedSize << y.dateTime;
        if(!y.name.endsWith("/"))
        {
            qDebug() << v_zip.setCurrentFile(y.name);
            QuaZipFile v_qfile(&v_zip);
            qDebug() << v_qfile.open(QIODevice::ReadOnly);
            QByteArray v_bytes = v_qfile.readAll();
            qDebug() << v_bytes.size();
            //qDebug() << v_bytes;
            break;
        }
    }


    return a.exec();
}
