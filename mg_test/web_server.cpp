// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "web_server.h"

#include <Windows.h>
#include <stdio.h>
#include <wchar.h>

#include <QtCore>
#include "mongoose.h"

#if 0x0
#include "executable.h"
#include "file_utils.h"
#include "log.h"
#include "mongoose.h"
#include "settings.h"
#include "string_utils.h"
#include "version.h"
#include "temp_dir.h"
#endif

QDebug operator<<(QDebug out, const std::string& str)
{
    out << QString::fromStdString(str);
    return out;
}

int g_webServerPort = 0;
std::string g_webServerIpAddress = "";
std::string g_webServerUrl = "";
std::string g_wwwDirectory = "";
std::string g_cgiInterpreter = "";

struct mg_context* g_mongooseContext = 0;
////extern std::string g_cgiEnvironmentFromArgv;

// Called when mongoose is about to log a message. If callback returns
// non-zero, mongoose does not log anything.
static int log_message(const struct mg_connection* conn, const char *message) {
    //LOG_WARNING << message;
    qDebug() << message;
    return 0;
}

// Called when mongoose has finished processing request.
static void end_request(const struct mg_connection* conn, int reply_status_code) {
    mg_request_info* request = mg_get_request_info(const_cast<mg_connection*>(conn));
    std::string message;
    message.append(request->request_method);
    message.append(" ");
    message.append(std::to_string(reply_status_code));
    message.append(" ");
    message.append(request->uri);
    if (request->query_string) {
        message.append("?");
        message.append(request->query_string);
    }
    //LOG_INFO << message;
    qDebug() << message;
}

bool StartWebServer() {
    //LOG_INFO << "Starting Mongoose " << mg_version() << " web server";
    qDebug() << "Starting Mongoose " << mg_version() << " web server";

    ////json_value* appSettings = GetApplicationSettings();

    // 404_handler
    ////std::string _404_handler = (*appSettings)["web_server"]["404_handler"];
    std::string _404_handler = "/pretty-urls.php";

    // Ip address and port. If port was set to 0, then real port
    // will be known only after the webserver was started.
    ////std::string ipAddress = (*appSettings)["web_server"]["listen_on"][0];
    std::string ipAddress = "127.0.0.1";
    ////std::string port = (*appSettings)["web_server"]["listen_on"][1];
    std::string port = "8080";
#if 0x0
    long portInt = (*appSettings)["web_server"]["listen_on"][1];
    if (portInt)
        port = IntToString(portInt);
    if (ipAddress.empty()) {
        ipAddress = "127.0.0.1";
    }
    if (port.empty()) {
        port = "0";
    }
#endif

    // WWW directory from settings.
#if 0x0
    std::string wwwDirectory = (*appSettings)["web_server"]["www_directory"];
    if (wwwDirectory.empty()) {
        wwwDirectory = "www";
    }
    wwwDirectory = GetAbsolutePath(wwwDirectory);
#else
    std::string wwwDirectory = "E:\\phpdesktop-chrome-31.8-php-5.6.1\\www";
#endif
    //LOG_INFO << "WWW directory: " << wwwDirectory;
    qDebug() << "WWW directory: " << wwwDirectory;

    // Index files from settings.
#if 0x0
    const json_value indexFilesArray = (*appSettings)["web_server"]["index_files"];
    std::string indexFiles;
    for (int i = 0; i < 32; i++) {
        const char* file = indexFilesArray[i];
        if (strlen(file)) {
            if (indexFiles.length())
                indexFiles.append(",");
            indexFiles.append(file);
        }
    }
    if (indexFiles.empty())
        indexFiles = "index.html,index.php";
#else
    std::string indexFiles = "index.html,index.php";
#endif
    //LOG_INFO << "Index files: " << indexFiles;
    qDebug() << "Index files: " << indexFiles;

    // CGI interpreter from settings.
#if 0x0
    std::string cgiInterpreter = (*appSettings)["web_server"]["cgi_interpreter"];
    if (cgiInterpreter.empty()) {
        cgiInterpreter = "php\\php-cgi.exe";
    }
    cgiInterpreter = GetAbsolutePath(cgiInterpreter);
#else
    std::string cgiInterpreter = "E:\\phpdesktop-chrome-31.8-php-5.6.1\\php\\php-cgi.exe";
#endif
    //LOG_INFO << "CGI interpreter: " << cgiInterpreter;
    qDebug() << "CGI interpreter: " << cgiInterpreter;

    // CGI extensions from settings.
#if 0x0
    const json_value cgiExtensions =
            (*appSettings)["web_server"]["cgi_extensions"];
    std::string cgiPattern;
    for (int i = 0; i < 32; i++) {
        const char* extension = cgiExtensions[i];
        if (strlen(extension)) {
            if (cgiPattern.length())
                cgiPattern.append("|");
            cgiPattern.append("**.").append(extension).append("$");
        }
    }
    if (cgiPattern.empty())
        cgiPattern = "**.php$";
#else
    std::string cgiPattern = "**.php$";
#endif
    //LOG_INFO << "CGI pattern: " << cgiPattern;
    qDebug() << "CGI pattern: " << cgiPattern;

    // Hide files patterns.
#if 0x0
    const json_value hide_files = (*appSettings)["web_server"]["hide_files"];
    std::string hide_files_patterns = "";
    for (int i = 0; i < 100; i++) {
        const char* pattern = hide_files[i];
        if (strlen(pattern)) {
            if (hide_files_patterns.length())
                hide_files_patterns.append("|");
            hide_files_patterns.append("**/").append(pattern).append("$");
        }
    }
#else
    std::string hide_files_patterns = "";
#endif
    //LOG_INFO << "Hide files patterns: " << hide_files_patterns;
    qDebug() << "Hide files patterns: " << hide_files_patterns;

    // Temp directory.
    ////std::string cgi_temp_dir = (*appSettings)["web_server"]["cgi_temp_dir"];
    ////cgi_temp_dir = GetAbsolutePath(cgi_temp_dir);
    std::string cgi_temp_dir = "";
    QDir cgi_temp_dir_q(QString::fromStdString(cgi_temp_dir));
    ////if (!cgi_temp_dir.length() || !DirectoryExists(cgi_temp_dir)) {
    if (!cgi_temp_dir.length() || !cgi_temp_dir_q.exists()) {
        if (cgi_temp_dir.length()) {
            //LOG_WARNING << "cgi_temp_dir directory does not exist: " << cgi_temp_dir;
            qDebug() << "cgi_temp_dir directory does not exist: " << cgi_temp_dir;
        }
        ////cgi_temp_dir.assign(GetAnsiTempDirectory());
        cgi_temp_dir = QDir::tempPath().toStdString();
    }

    // CGI environment variables.
    std::string cgiEnvironment = "";
    cgiEnvironment.append("TMP=").append(cgi_temp_dir).append(",");
    cgiEnvironment.append("TEMP=").append(cgi_temp_dir).append(",");
    cgiEnvironment.append("TMPDIR=").append(cgi_temp_dir).append(",");
    // Mongoose sets SERVER_NAME to "mydomain.com"
    cgiEnvironment.append("SERVER_NAME=").append(ipAddress).append(",");
#if 0x0
    // Let users identify whether web app runs in a normal browser
    // or a phpdesktop browser.
    cgiEnvironment.append("PHPDESKTOP_VERSION=").append(GetPhpDesktopVersion());
    // Environment from application args
    if (g_cgiEnvironmentFromArgv.length()) {
        cgiEnvironment.append(",").append(g_cgiEnvironmentFromArgv);
    }
#endif
    //LOG_INFO << "CGI environment variables set: " << cgiEnvironment;
    qDebug() << "CGI environment variables set: " << cgiEnvironment;

    // Mongoose web server.
    std::string listening_ports = ipAddress + ":" + port;
    const char* options[] = {
        "document_root", wwwDirectory.c_str(),
        "listening_ports", listening_ports.c_str(),
        "index_files", indexFiles.c_str(),
        "cgi_interpreter", cgiInterpreter.c_str(),
        "cgi_pattern", cgiPattern.c_str(),
        "cgi_environment", cgiEnvironment.c_str(),
        "404_handler", _404_handler.c_str(),
        "hide_files_patterns", hide_files_patterns.c_str(),
        NULL
    };

    // Globals.
    g_wwwDirectory = wwwDirectory;
    g_cgiInterpreter = cgiInterpreter;

    mg_callbacks callbacks = {0};
    callbacks.log_message = &log_message;
    callbacks.end_request = &end_request;
    g_mongooseContext = mg_start(&callbacks, NULL, options);
    if (g_mongooseContext == NULL)
        return false;

    // When port was set to 0 then a random free port was assigned.
    g_webServerPort = mg_get_listening_port(g_mongooseContext);
    g_webServerIpAddress = ipAddress;
    g_webServerUrl = "http://" + ipAddress + ":" + std::to_string(g_webServerPort) + "/";
    //LOG_INFO << "Web server url: " << g_webServerUrl;
    qDebug() << "Web server url: " << g_webServerUrl;

    return true;
}

void StopWebServer() {
    if (g_mongooseContext) {
        //LOG_INFO << "Stopping Mongoose web server";
        qDebug() << "Stopping Mongoose web server";
        /*
        Stoppping Mongoose webserver freezes for about 30 seconds
        on Win7/MSIE if we call mg_stop(). Introduced new function
        mg_stop_immediately() that does not free mongoose resources.
        See Issue 61 for more details:
        https://code.google.com/p/phpdesktop/issues/detail?id=61
        -----------------------------------------------------------------
        This issue was only seen to happen when using the MSIE webbrowser
        control. But it won't hurt to secure ourselves against this bug
        happening in the future when used with the Chrome browser.
        */
        mg_stop_immediately(g_mongooseContext);
        //LOG_DEBUG << "Mongoose webserver stopped immediately";
        qDebug() << "Mongoose webserver stopped immediately";
    }
}

int GetWebServerPort() {
    return g_webServerPort;
}
std::string GetWebServerIpAddress() {
    return g_webServerIpAddress;
}
std::string GetWebServerUrl() {
    return g_webServerUrl;
}
std::string GetWwwDirectory() {
    return g_wwwDirectory;
}
std::string GetCgiInterpreter() {
    return g_cgiInterpreter;
}
