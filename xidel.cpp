#include "xidel.h"

#include <qdebug.h>

xidel::xidel()
{
}

void xidel::getSourceCode(QString url)
{
    QString detectedCharset;

    QString inputUrl = url;
    QUrl inputQurl(inputUrl);
    detectedCharset = "";

    if(!inputQurl.isValid() || inputQurl.isRelative())
    {
        emit printOutput(tr("URL is invalid. Please retry with a valid URL."),ERROR_STYLE);
        emit enableButtons();
        return;
    }

    QNetworkAccessManager *networkMgr = new QNetworkAccessManager();
    QNetworkRequest request(inputQurl);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:10.0.2) Gecko/20100101 Firefox/10.0.2");
    request.setRawHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8");
    request.setRawHeader("Pragma","no-cache");
    request.setRawHeader("DNT","1");
    request.setRawHeader("Upgrade-Insecure-Requests","1");
    request.setRawHeader("Cache-Control","no-cache");


    QNetworkReply *reply = networkMgr->get(request);

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    const QByteArray replyContent=reply->readAll();
    reply->deleteLater();

    if(this->encoding == "Auto")
    {
    QRegularExpression charsetRegExp("<meta.*?charset=[\"']?([^\"']+)[\"']?");
    QRegularExpressionMatch match = charsetRegExp.match(replyContent);
    if (match.hasMatch())
        detectedCharset = match.captured(1);

    // The html lang attribute
    if(detectedCharset.isEmpty())
    {
        QRegularExpression charsetRegExp("<html.*?lang=[\"']?([^\"']+)[\"']?");
        QRegularExpressionMatch match = charsetRegExp.match(replyContent);
        if (match.hasMatch())
        {
            if(match.captured(1).toLower() == "fr")
            detectedCharset = "ISO-8859-15";
        }
    }
    }
    else if(this->encoding == "Latin1")
        detectedCharset = "ISO-8859-15";

    QTextCodec *codec = (!detectedCharset.isEmpty() && QTextCodec::codecForName(detectedCharset.toUtf8())) ? QTextCodec::codecForName(detectedCharset.toUtf8()) : QTextCodec::codecForName("UTF-8");
    QString output = codec->toUnicode(replyContent);

    if(!reply->error())
        emit displaySourceCode(output);
    else
        emit printOutput(tr("Unable to get source code for ") + url,ERROR_STYLE);

     emit enableButtons();
}
