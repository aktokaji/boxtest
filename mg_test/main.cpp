#include "widget.h"
#include <QApplication>
#include "web_server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    StartWebServer();

    Widget w;
    w.show();

    //return a.exec();
    int exitCode = a.exec();
    StopWebServer();
    return exitCode;
}
