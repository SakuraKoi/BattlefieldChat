#pragma once

#include <QThread>

class UpdateCheckerThread : public QThread
{
    Q_OBJECT

public:
    UpdateCheckerThread(QObject *parent = NULL);
    ~UpdateCheckerThread();
    void run() override;

signals:
    void newVersionFound(QString version, QString link);
};
