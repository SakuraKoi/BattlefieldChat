#include "BattlefieldChat.h"
#include <QCloseEvent>

BattlefieldChat::BattlefieldChat(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    workerThread.start();
}

void BattlefieldChat::closeEvent(QCloseEvent* event) {
    workerThread.requestInterruption();
    // TODO: cleanup
    event->accept();
}