#pragma once

#include <QObject>

class CountLoaderTask : public QObject {
    Q_OBJECT

public:
    CountLoaderTask(QObject* parent = NULL);
    ~CountLoaderTask();
    void execute();

signals:
    void countLoaded(QByteArray data);
};
