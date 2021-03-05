#include "Translator.h"
#include "GlobalVariables.h"
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCryptographicHash>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

QString baiduAppid;
QString baiduKey;

BaiduTranslator SINGLETON_TRANSLATOR_BAIDU;

QString doTranslate(QString text) {
    QUrl url("https://fanyi-api.baidu.com/api/trans/vip/translate");
    QUrlQuery query;

    query.addQueryItem("q", text);
    query.addQueryItem("from", "auto");
    query.addQueryItem("to", "en");
    query.addQueryItem("appid", baiduAppid);
    query.addQueryItem("salt", "1145141919810");

    QCryptographicHash md5(QCryptographicHash::Md5);
    QByteArray hashData, hashResult;
    hashData.append((baiduAppid + text + "1145141919810" + baiduKey).toUtf8());
    md5.addData(hashData);
    hashResult = md5.result();
    QString hashResultString;
    hashResultString.append(hashResult.toHex());

    query.addQueryItem("sign", hashResultString.toLower());

    url.setQuery(query.query());

    QNetworkRequest request(url);

    QNetworkReply* reply = network->get(request);

    QByteArray responseData;
    QEventLoop eventLoop;
    QTimer timer;
    timer.setSingleShot(true);

    QObject::connect(network, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    timer.start(translatorTimeout);
    eventLoop.exec();

    // TODO ensure this object is deleted correctly and wont cause any problem :)
    reply->deleteLater();

    if (!timer.isActive()) {
        TranslateException exception;
        exception.reason = QString::fromUtf8(u8"Timeout");
        throw exception;
    }
    timer.stop();

    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        TranslateException exception;
        exception.reason = QString::fromUtf8(u8"Network error: ") + reply->errorString();
        throw exception;
    }

    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (!(status >= 200 && status < 300)) {
        TranslateException exception;
        exception.reason = QString::fromUtf8(u8"Server responsed ") + std::to_string(status).c_str();
        throw exception;
    }

    responseData = reply->readAll();
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(responseData, &jsonError);

    if (document.isNull() || jsonError.error != QJsonParseError::NoError) {
        TranslateException exception;
        exception.reason = QString::fromUtf8(u8"Cannot parse response");
        throw exception;
    }

    if (document.isObject()) {
        QJsonObject object = document.object();
        if (object.contains("trans_result")) {
            QJsonArray result = object.value("trans_result").toArray();
            if (result.isEmpty()) {
                TranslateException exception;
                exception.reason = QString::fromUtf8(u8"Empty result");
                throw exception;
            }
            return result.first().toObject().value("dst").toString();
        } else if (object.contains("error_code")) {
            TranslateException exception;
            exception.reason = QString::fromUtf8(u8"Server error: ") + object.value("error_code").toString() + " " + object.value("error_msg").toString();
            throw exception;
        }
    }
    TranslateException exception;
    exception.reason = QString::fromUtf8(u8"Invalid json response received");
    throw exception;
}

QString BaiduTranslator::translate(QString text) {
    return doTranslate(text);
}