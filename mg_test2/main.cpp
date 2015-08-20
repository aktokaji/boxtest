#include "widget.h"
#include <QApplication>

#include <QtCore>

#include "mongoose.h"

// Start a browser and hit refresh couple of times. The replies will
// come from both server instances.
static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
#if 0x0
  if (ev == MG_REQUEST)
  {
    mg_send_header(conn, "Content-Type", "text/plain");
    mg_printf_data(conn, "This is a reply from server instance # %s",
                   (char *) conn->server_param);
    return MG_TRUE;
  }
  else
#else
    if (ev == MG_REQUEST)
    {
        qDebug() << conn->request_method << conn->uri << conn->query_string;
        return MG_FALSE;
    }
    else
#endif
  if (ev == MG_AUTH)
  {
    return MG_TRUE;
  }
  else
  {
    return MG_FALSE;
  }
}

static void *serve(void *server) {
  for (;;) mg_poll_server((struct mg_server *) server, 1000);
  return NULL;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    std::string cgiInterpreter = "E:\\phpdesktop-chrome-31.8-php-5.6.1\\php\\php-cgi.exe";
    ////std::string wwwDirectory = "E:\\phpdesktop-chrome-31.8-php-5.6.1\\www";
    QString documentRoot = "E:\\phpdesktop-chrome-31.8-php-5.6.1\\www";
    //QString documentRoot = QString::fromWCharArray(L"E:\\phpdesktop-chrome-31.8-php-5.6.1\\ウェブ");
    qDebug() << "[documentRoot]" << documentRoot;

    struct mg_server *server1 = NULL;

    server1 = mg_create_server((void *) "1", ev_handler);

    // Make both server1 and server2 listen on the same sockets
    mg_set_option(server1, "listening_port", "8080");


    mg_set_option(server1, "cgi_interpreter", cgiInterpreter.c_str());
    //mg_set_option(server1, "document_root", wwwDirectory.c_str());
    mg_set_option(server1, "document_root", documentRoot.toUtf8().constData());
    //mg_set_option(server1, "document_root", documentRoot.toLocal8Bit().constData());

    // server1 goes to separate thread, server 2 runs in main thread.
    // IMPORTANT: NEVER LET DIFFERENT THREADS HANDLE THE SAME SERVER.
    mg_start_thread(serve, server1);

    Widget w;
    w.show();

    return a.exec();
}
