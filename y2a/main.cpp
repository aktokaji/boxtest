#include <QtCore>
#include <QtNetwork>

class MyObject : public QObject
{
    Q_OBJECT
public:
    MyObject()
    {
    }
public slots:
    void on_sslErrors(QNetworkReply* reply, QList<QSslError> errors)
    {
        reply->ignoreSslErrors(errors);
    }
    void on_reply_finished()
    {
        QNetworkReply *reply = (QNetworkReply *)sender();
        qDebug() << reply->isFinished();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        QByteArray bytes = reply->readAll();
        qDebug().noquote() << bytes;
        auto jsonDoc = QJsonDocument::fromJson(bytes);
        auto json = jsonDoc.object();
        qDebug() << json.value("kind");
        qDebug() << json["pageInfo"];
        qDebug() << json["pageInfo"].toObject()["totalResults"];
        //qDebug() << json["items"].toArray()[1].toObject()["snippet"].toObject()["title"];
        for(int i=0; i<json["items"].toArray().size(); i++)
        {
            QString title = json["items"].toArray()[i].toObject()["snippet"].toObject()["title"].toString();
            QString videoId = json["items"].toArray()[i].toObject()["snippet"].toObject()["resourceId"].toObject()["videoId"].toString();
            //qDebug() << i << json["items"].toArray()[i].toObject()["snippet"].toObject()["title"] << json["items"].toArray()[i].toObject()["snippet"].toObject()["resourceId"].toObject()["videoId"];
            qDebug().noquote() << i << videoId << title;
        }
        QCoreApplication::exit(0);
    }

};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MyObject mine;
    qDebug() << "(1)";
    QNetworkAccessManager nam;
    QObject::connect(&nam, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), &mine, SLOT(on_sslErrors(QNetworkReply*,QList<QSslError>)));
    qDebug() << "(2)";
    // [DOC] https://developers.google.com/youtube/v3/docs/playlistItems/list
    //QString urlStr = "https://www.googleapis.com/youtube/v3/playlistItems?part=id%2Csnippet&maxResults=5&playlistId=FLjOQQ9Fp_WC_2e6o0MLZXKg&key=AIzaSyBXLSLtMVJ8KOGPmDr29NIVrV1neCVxx-w";
    QString urlStr = "https://www.googleapis.com/youtube/v3/playlistItems?part=id,snippet&maxResults=5&playlistId=FLjOQQ9Fp_WC_2e6o0MLZXKg&key=AIzaSyBXLSLtMVJ8KOGPmDr29NIVrV1neCVxx-w";
    QUrl url(urlStr);
    QNetworkRequest request(url);
    qDebug() << "(3)";
    QNetworkReply *reply = nam.get(request);
    QObject::connect(reply, SIGNAL(finished()), &mine, SLOT(on_reply_finished()));
    //reply->ignoreSslErrors();
    qDebug() << "(4)";
#if 0x0
    while(reply->waitForReadyRead(-1))
    {
        qDebug() << "(5)";
        QByteArray bytes = reply->readAll();
        qDebug() << bytes;
        qDebug() << "(6)";
    }
    qDebug() << "(7)";
#endif

    return a.exec();
}

#include "main.moc"
