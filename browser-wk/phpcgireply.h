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
    explicit PhpCgiReply(QNetworkAccessManager *manager, QNetworkAccessManager::Operation op, const QNetworkRequest & req, QIODevice * outgoingData, QObject *parent = 0): QNetworkReply(parent), m_nam(manager)
    {
        this->setOperation(op);
        this->setRequest(req);
        this->setUrl(req.url());
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
#if 0x0
        QTimer::singleShot( 10, this, SIGNAL(metaDataChanged()));
        QTimer::singleShot( 20, this, SIGNAL(readyRead()) );
        QTimer::singleShot( 30, this, SIGNAL(finished()) );
#endif
        connect(&m_proc, SIGNAL(finished(int)), this, SIGNAL(finished()));
        m_proc.setReadChannel(QProcess::StandardOutput);
        QStringList arguments;
        //arguments << "--help";
        arguments << "E:\\phpdesktop-chrome-31.8-php-5.6.1\\www\\phpinfo.php";
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
        qDebug() << "[maxlen]" << maxlen;
        qint64 readLen = m_proc.readData(data, maxlen);
        qDebug() << "[readLen]" << readLen;
        //if(readLen>0) m_buffer.seek(m_buffer.pos()+readLen);
        //qDebug() << "[m_buffer.pos()]" << m_buffer.pos();
        //if(m_buffer.atEnd()) QTimer::singleShot(0, this, SIGNAL(finished()));
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
