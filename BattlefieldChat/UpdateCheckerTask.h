#pragma once

#include <QObject>

class UpdateCheckerTask : public QObject
{
    Q_OBJECT

public:
    UpdateCheckerTask(QObject *parent = NULL);
    ~UpdateCheckerTask();
    void execute();

signals:
    void newVersionFound(QString version, QString link);
};
