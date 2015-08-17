#ifndef PHPCGIREPLY_H
#define PHPCGIREPLY_H

#include <QtCore>
#include <QtNetwork>

class PhpCgiBuffer : public QBuffer
{
public:
    virtual qint64 readData(char *data, qint64 maxlen) override
    {
        return QBuffer::readData(data, maxlen);
    }
};

class PhpCgiProcess : public QProcess
{
public:
    virtual qint64 readData(char *data, qint64 maxlen) override
    {
        return QProcess::readData(data, maxlen);
    }
};

class PhpCgiReply : public QNetworkReply
{
    Q_OBJECT
protected:
    QNetworkAccessManager *m_nam = NULL;
    //PhpCgiBuffer m_buffer;
    PhpCgiProcess m_proc;
public:
    void addToEnv(QStringList &env, const QString &name, const QVariant &val)
    {
        env << QString("%1=%2").arg(name).arg(val.toString());
    }
    explicit PhpCgiReply(QNetworkAccessManager *manager, QNetworkAccessManager::Operation op, const QNetworkRequest & req, QIODevice * outgoingData, QObject *parent = 0): QNetworkReply(parent), m_nam(manager)
    {
        this->setOperation(op);
        this->setRequest(req);
        this->setUrl(req.url());

        QUrl v_url = req.url();
        QDir v_dir = "E:\\phpdesktop-chrome-31.8-php-5.6.1\\www";
        qDebug() << "[v_dir]" << v_dir;

        QString v_dir_path = v_dir.absolutePath().replace("/", "\\");

        qDebug() << "[v_url.path()]" << v_url.path();
        QString v_url_path = v_url.path();

        QFile v_script_file = v_dir.absolutePath() + v_url_path;
        qDebug() << "[v_script_file]" << v_script_file.fileName();
        qDebug() << "[v_script_file(native)]" << v_script_file.fileName().replace("/", "\\");

        QString v_script_path = v_script_file.fileName().replace("/", "\\");


        this->setAttribute(QNetworkRequest::HttpStatusCodeAttribute, 200);
        this->setError(NetworkError::NoError, "OK");
        //this->setRawHeader("Content-Type", "text/html; charset=utf-8");
        this->setHeader(QNetworkRequest::ContentTypeHeader, "text/html; charset=utf-8");
        QString text = QString::fromWCharArray(L"<h1>test テスト</h1>");
        //this->m_buffer.setData(text.toUtf8());
        //this->setRawHeader("Content-Length", QString("%1").arg(m_buffer.bytesAvailable()).toLatin1());
        //this->setRawHeader("Content-Length", QByteArray::number(m_buffer.bytesAvailable()));
        ////this->setHeader(QNetworkRequest::ContentLengthHeader, m_buffer.bytesAvailable());
        //this->m_buffer.open(QIODevice::ReadOnly);
        QIODevice::setOpenMode(QIODevice::ReadOnly | QIODevice::Unbuffered);
        connect(&m_proc, SIGNAL(finished(int)), this, SIGNAL(finished()));
        m_proc.setReadChannel(QProcess::StandardOutput);
        //m_proc.setReadChannel(QProcess::StandardError);
#if 0x1
        //SERVER_NAME
        QStringList env = QProcess::systemEnvironment();
        if(true)
        {
            addToEnv(env, "SERVER_NAME", req.url().host());
            addToEnv(env, "SERVER_ROOT", v_dir_path);
            addToEnv(env, "DOCUMENT_ROOT", v_dir_path);
            env << "SERVER_SOFTWARE=Mongoose/3.9c";
            env << "GATEWAY_INTERFACE=CGI/1.1";
            env << "SERVER_PROTOCOL=HTTP/1.1";
            env << "REDIRECT_STATUS=200";
            env << "SERVER_PORT=80";
            env << "REQUEST_METHOD=GET";
            env << "REMOTE_ADDR=127.0.0.1";
            env << "REMOTE_PORT=0";
            addToEnv(env, "REQUEST_URI", v_url_path);
            addToEnv(env, "SCRIPT_NAME", v_url_path);
            addToEnv(env, "SCRIPT_FILENAME", v_script_path);
            //env << "SCRIPT_FILENAME=E:\\phpdesktop-chrome-31.8-php-5.6.1\\www\\phpinfo.php";
            addToEnv(env, "PATH_TRANSLATED", v_script_path);
            //env << "PATH_TRANSLATED=E:\\phpdesktop-chrome-31.8-php-5.6.1\\www\\phpinfo.php";
            env << "HTTPS=off";
            env << QString("HTTP_HOST=%1:%2").arg(req.url().host()).arg(80);
        }
        QList<QByteArray> v_list = req.rawHeaderList();
        if(true)
        {
            for(int i=0; i<v_list.size(); i++)
            {
                QByteArray v_name = v_list[i];
                qDebug() << "[v_name]" << v_name;

            }
        }
#if 0x1
        if(v_list.contains("Accept"))
        {
            env << QString("HTTP_ACCEPT=%1").arg(QString::fromLatin1(req.rawHeader("Accept")));
        }
        if(v_list.contains("User-Agent"))
        {
            env << QString("HTTP_USER_AGENT=%1").arg(QString::fromLatin1(req.rawHeader("User-Agent")));
        }
        if(v_list.contains("Referer"))
        {
            env << QString("HTTP_REFERER=%1").arg(QString::fromLatin1(req.rawHeader("Referer")));
        }
#endif
        m_proc.setEnvironment(env);
#endif
        if(false)
        {
            for(int i=0; i<v_list.size(); i++)
            {
                QByteArray v_name = v_list[i];
                QByteArray v_val = req.rawHeader(v_name);
                QByteArray v_line = v_name + ": " + v_val;
                qDebug() << "[v_name]" << v_name;
                m_proc.write(v_line);
                m_proc.write("\n");
            }
            m_proc.write("\n");
        }
        QStringList arguments;
        //arguments << "-e";
        //arguments << "E:\\phpdesktop-chrome-31.8-php-5.6.1\\www\\phpinfo.php";
        //arguments << "E:\\phpdesktop-chrome-31.8-php-5.6.1\\www\\index.php";
        arguments << v_script_path;
        m_proc.start("E:\\phpdesktop-chrome-31.8-php-5.6.1\\php\\php-cgi.exe", arguments);

        //QTimer::singleShot( 10, this, SLOT(onReady()) );
        //this->setFinished(true);
    }
    virtual bool isSequential() const override
    {
        qDebug() << "isSequential()";
        return true;
    }
    virtual bool open(OpenMode mode) override
    {
        qDebug() << "open()";
        QIODevice::setOpenMode(QIODevice::ReadOnly);
        return true;
    }
    virtual void close() override
    {
        qDebug() << "close()";
    }
    virtual void abort() override
    {
        qDebug() << "abort()";
    }
    virtual qint64 bytesAvailable() const override
    {
        //qint64 available = m_buffer.bytesAvailable();
        //qint64 available = m_buffer.size();
        qint64 available = m_proc.bytesAvailable();
        qDebug() << "[available]" << available;
        return available;
    }
    virtual qint64 PhpCgiReply::readData(char *data, qint64 maxlen)
    {
#if 0x0
        qDebug() << "[maxlen]" << maxlen;
        qint64 readLen = m_buffer.readData(data, maxlen);
        qDebug() << "[readLen]" << readLen;
        if(readLen>0) m_buffer.seek(m_buffer.pos()+readLen);
        qDebug() << "[m_buffer.pos()]" << m_buffer.pos();
        //if(m_buffer.atEnd()) QTimer::singleShot(0, this, SIGNAL(finished()));
        return readLen;
#else
        //qDebug() << "[maxlen]" << maxlen;
        qint64 readLen = m_proc.readData(data, maxlen);
        //qDebug() << "[readLen]" << readLen;
        return readLen;
#endif
    }
#if 0x0
    virtual qint64 pos() const override
    {
        qDebug() << "pos()";
        return m_buffer.pos();
    }
    virtual qint64 size() const override
    {
        qDebug() << "size()";
        return m_buffer.data().size();
    }
    virtual bool seek(qint64 pos) override
    {
        qDebug() << "seek() called" << pos;
        return false;
    }
#endif
    virtual bool atEnd() const override
    {
        qDebug() << "atEnd()";
        return m_proc.atEnd();
    }
    virtual bool reset() override
    {
        qDebug() << "reset()";
        return false;
    }
protected slots:
    void onReady()
    {
        emit metaDataChanged();
        emit readyRead();
        emit finished();
    }
};




#endif // PHPCGIREPLY_H
