#include "BattlefieldChat.h"
#include "GlobalVariables.h"
#include <QCloseEvent>

BattlefieldChat::BattlefieldChat(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
    mainWindow = this;
    inputWindow = new InputDialog();
    workerThread = new WorkerThread();
    connect(workerThread, SIGNAL(updageGameFoundState(bool)), this, SLOT(updageGameFoundState(bool)));
}

void BattlefieldChat::showEvent(QShowEvent* ev) {
    QMainWindow::showEvent(ev);
    workerThread -> start();
}

bool shutdownPending = false;
void BattlefieldChat::closeEvent(QCloseEvent* event) {
    shutdownPending = true;
    workerThread -> requestInterruption();
    inputWindow->close();
    event->accept();
}

void BattlefieldChat::pushLog(QString message) {
    if (!shutdownPending) {
        ui.listLogs->addItem(message);
        ui.listLogs->scrollToBottom();
    }
}

void BattlefieldChat::logColor(Qt::GlobalColor color) {
    if (!shutdownPending)
        ui.listLogs->item(ui.listLogs->count() - 1)->setForeground(color);
}

bool lastGameState = false;
void BattlefieldChat::updageGameFoundState(bool found) {
    if (!shutdownPending && (lastGameState != found)) {
        lastGameState = found;
        if (found) {
            ui.mainContent->setEnabled(true);
            ui.lblCurrentStatus->setText(QString::fromLocal8Bit("中文输入工具就绪"));
            ui.lblCurrentStatus->setStyleSheet("color: rgb(85, 170, 0);\nfont: 75 12pt \"微软雅黑\";");
        } else {
            ui.mainContent->setEnabled(false);
            ui.lblCurrentStatus->setText(QString::fromLocal8Bit("正在等待游戏启动"));
            ui.lblCurrentStatus->setStyleSheet("color: rgb(255, 0, 0);\nfont: 75 12pt \"微软雅黑\";");
        }
    }
}