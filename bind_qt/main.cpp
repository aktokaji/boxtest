#include <QCoreApplication>
#include <QtCore>
#include <QtNetwork>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTcpSocket v_socket;
    if(v_socket.bind(0))
    {
        quint16 v_port = v_socket.localPort();
        qDebug() << v_port;
        //v_socket.close();
        //v_socket.abort();
        QString v_php = QString("E:\\phpdesktop-chrome-31.8-php-5.6.1\\php\\php.exe -S 127.0.0.1:%1 -t E:\\phpdesktop-chrome-31.8-php-5.6.1\\www").arg(v_port);
        system(v_php.toLocal8Bit().constData());
    }
    //return a.exec();
    return 0;
}
