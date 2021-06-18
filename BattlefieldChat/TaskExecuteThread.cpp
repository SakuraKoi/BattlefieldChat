#include "TaskExecuteThread.h"
#include "GlobalVariables.h"

TaskExecuteThread::TaskExecuteThread(QObject* parent) : QThread(parent) {
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

TaskExecuteThread::~TaskExecuteThread() {
}

void TaskExecuteThread::run() {
    countLoaderTask->execute();
    updateCheckerTask->execute();
}