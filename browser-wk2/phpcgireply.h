#ifndef PHPCGIREPLY_H
#define PHPCGIREPLY_H

#include "networkaccessmanager.h"

#include <QtCore>
#include <QtNetwork>

extern quint16 g_localPort;

class PhpCgiReply : public QNetworkReply
{
    Q_OBJECT
protected:
    QNetworkReply *m_reply = NULL;
    QNetworkAccessManager *m_nam = NULL;
    //QIODevice *m_outgoingData = NULL;
    //PhpCgiBuffer m_buffer;
    //PhpCgiProcess m_proc;
public:
    explicit PhpCgiReply(NetworkAccessManager *manager,
                         QNetworkAccessManager::Operation op,
                         const QNetworkRequest & req,
                         QIODevice * outgoingData,
                         QObject *parent = 0): QNetworkReply(parent), m_nam(manager)
    {
        this->setOperation(op);
        this->setRequest(req);
        this->setUrl(req.url());
        QNetworkRequest request = req; // copy so we can modify
        // this is a temporary hack until we properly use the pipelining flags from QtWebkit
        // pipeline everything! :)
        QUrl v_url = request.url();
        v_url.setHost("127.0.0.1");
        v_url.setPort(g_localPort);
        request.setUrl(v_url);
        request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
        m_reply = manager->createRequest(op, request, outgoingData);
        QIODevice::setOpenMode(QIODevice::ReadOnly);
        connect(m_reply, SIGNAL(metaDataChanged()), this, SLOT(on_metaDataChanged()));
        connect(m_reply, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
        connect(m_reply, SIGNAL(finished()), this, SLOT(on_finished()));
        connect(m_reply, SIGNAL(readChannelFinished()), this, SIGNAL(readChannelFinished()));
        connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(downloadProgress(qint64,qint64)));
        connect(m_reply, SIGNAL(aboutToClose()), this, SIGNAL(aboutToClose()));
    }
public slots:
    void on_metaDataChanged()
    {
        qDebug() << "[on_metaDataChanged]";
        this->setError(QNetworkReply::NoError, "OK");
        for(int i=(int)QNetworkRequest::HttpStatusCodeAttribute; i<=QNetworkRequest::EmitAllUploadProgressSignalsAttribute; i++)
        {
            QVariant v_attr = m_reply->attribute((QNetworkRequest::Attribute)i);
            if(v_attr.isValid())
            {
                qDebug() << "[valid]" << i << (QNetworkRequest::Attribute)i << v_attr;
                this->setAttribute((QNetworkRequest::Attribute)i, v_attr);
            }
        }
        //this->setAttribute(QNetworkRequest::HttpStatusCodeAttribute, m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute));
        //this->setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, m_reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute));

        QList<QByteArray> v_names = m_reply->rawHeaderList();
        for(int i=0; i<v_names.size(); i++)
        {
            QByteArray v_name = v_names[i];
            QByteArray v_value = m_reply->rawHeader(v_name);
            if(v_name=="Host")
            {
                v_value = "www.youtube.box:80";
            }
            qDebug() << v_name << v_value;
            this->setRawHeader(v_name, v_value);
        }
        qDebug() << this->hasRawHeader("Content-type");
        qDebug() << this->header(QNetworkRequest::ContentTypeHeader);
        emit metaDataChanged();
    }
    void on_finished()
    {
        this->setFinished(true);
        emit finished();
    }

public:
    virtual bool isSequential() const override
    {
        qDebug() << "isSequential()";
        return m_reply->isSequential();
    }
    virtual bool open(OpenMode mode) override
    {
        qDebug() << "open()";
        //QIODevice::setOpenMode(QIODevice::ReadOnly);
        //return true;
        return m_reply->open(mode);
    }
    virtual void close() override
    {
        qDebug() << "close()";
        m_reply->close();
    }
    virtual void abort() override
    {
        qDebug() << "[abort()]";
        m_reply->abort();
    }
    virtual qint64 bytesAvailable() const override
    {
        qint64 available = m_reply->bytesAvailable();
        qDebug() << "[available]" << available;
        return available;
    }
    virtual qint64 PhpCgiReply::readData(char *data, qint64 maxlen)
    {
        qDebug() << "[maxlen]" << maxlen;
        qint64 readLen = m_reply->read(data, maxlen);
        qDebug() << "[readLen]" << readLen;
        //QByteArray v_data = QByteArray(data, readLen);
        //qDebug() << v_data;
        return readLen;
    }
#if 0x1
    virtual qint64 pos() const override
    {
        qDebug() << "pos()";
        return m_reply->pos();
    }
    virtual qint64 size() const override
    {
        qDebug() << "size()";
        return m_reply->size();
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
        return m_reply->atEnd();
    }
    virtual bool reset() override
    {
        qDebug() << "reset()";
        //return false;
        return m_reply->reset();
    }
};




#endif // PHPCGIREPLY_H
