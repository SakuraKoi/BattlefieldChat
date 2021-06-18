#include "CountLoaderThread.h"
#include "GlobalVariables.h"
#include "Log.h"

#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

CountLoaderThread::CountLoaderThread(QObject* parent)
    : QThread(parent) {
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

CountLoaderThread::~CountLoaderThread() {
}

void CountLoaderThread::run() {
    QUrl url("https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2FSakuraKoi%2FBattlefidChat%2Fusage-track&count_bg=%2379C83D&title_bg=%23555555&title=%E4%BD%BF%E7%94%A8%E7%BB%9F%E8%AE%A1&edge_flat=true");

    QNetworkRequest request(url);

    QNetworkReply* reply = network->get(request);

    QByteArray responseData;
    QEventLoop eventLoop;

    QObject::connect(network, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));

    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        Log() << u8" [%] 加载匿名使用数据统计失败, 网络错误: " << reply->errorString().toUtf8().data(); LogColor(Qt::yellow);
        return;
    }

    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (!(status >= 200 && status < 300)) {
        Log() << u8" [%] 加载匿名使用数据统计失败, 服务器响应: " << std::to_string(status).c_str(); LogColor(Qt::yellow);
        return;
    }

    responseData = reply->readAll();
    emit countLoaded(responseData);
}