#include "phpcgireply.h"

#if 0x0
PhpCgiReply::PhpCgiReply(QNetworkAccessManager *manager, QObject *parent): QNetworkReply(parent), m_nam(manager)
{
    this->setRawHeader("Content-Type", "text/html; eocoding=utf-8");
    QString text = QString::fromUtf8("<h1>test テスト</h1>");
    this->m_buffer.setData(text.toUtf8());
}

qint64 PhpCgiReply::readData(char *data, qint64 maxlen)
{
    return m_buffer.readData(data, maxlen);
}
#endif
