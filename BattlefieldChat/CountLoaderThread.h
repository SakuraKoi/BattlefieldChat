#pragma once

#include <QThread>

class CountLoaderThread : public QThread {
    Q_OBJECT

public:
    CountLoaderThread(QObject* parent = NULL);
    ~CountLoaderThread();
    void run() override;

signals:
    void countLoaded(QByteArray data);
};
