#pragma once

#include <QThread>

class TaskExecuteThread : public QThread {
    Q_OBJECT

public:
    TaskExecuteThread(QObject* parent = NULL);
    ~TaskExecuteThread();
    void run() override;

signals:
    void countLoaded(QByteArray data);
};
