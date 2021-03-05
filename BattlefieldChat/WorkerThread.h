#pragma once

#include <QThread>

#include "Offsets.h"

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    WorkerThread(QObject *parent = NULL);
    ~WorkerThread();

    void run() override;
    void pushLog(QString message);
    void logColor(Qt::GlobalColor color);

signals:
    void updageGameFoundState(bool found);

private:
    void chatLoop();
    void doInput(Pointer messageCavePtr, ChatMessagePointer chatMessagePtr, bool isFullscreen);
    void writeChatMessage(Pointer messageCavePtr, ChatMessagePointer chatMessagePtr, std::string content, int length);
    bool suspendAndWrite(Pointer messageCavePtr, ChatMessagePointer chatMessagePtr, std::string content, int length);
    void resumePointer(ChatMessagePointer chatMessagePtr, uintptr_t oldAddress);
};