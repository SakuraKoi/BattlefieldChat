#include "UpdateCheckerThread.h"
#include "GlobalVariables.h"
#include "Log.h"

#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

UpdateCheckerThread::UpdateCheckerThread(QObject* parent)
    : QThread(parent) {
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

UpdateCheckerThread::~UpdateCheckerThread() {
}

void UpdateCheckerThread::run() {
    QUrl url("https://api.github.com/repos/SakuraKoi/BattlefieldChat/releases/latest");

    QNetworkRequest request(url);

    QNetworkReply* reply = network->get(request);

    QByteArray responseData;
    QEventLoop eventLoop;

    QObject::connect(network, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));

    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        Log() << u8" [%] 检查更新失败, 网络错误: " << reply->errorString().toUtf8().data(); LogColor(Qt::yellow);
        return;
    }

    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (!(status >= 200 && status < 300)) {
        Log() << u8" [%] 检查更新失败, 服务器响应: " << std::to_string(status).c_str(); LogColor(Qt::yellow);
        return;
    }

    responseData = reply->readAll();
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(responseData, &jsonError);

    if (!document.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (document.isObject()) {
            QJsonObject object = document.object();
            if (object.contains("tag_name")) {
                QString tag = object.value("tag_name").toString();
                QString url = object.value("html_url").toString();
                if (currentVersion.compare(tag) > 0) {
                    emit newVersionFound(tag, url);
                }
                return;
            }
        }
    }
    Log() << u8" [%] 检查更新失败, 无法解析服务器响应"; LogColor(Qt::yellow);
}