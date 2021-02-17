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
    network = new QNetworkAccessManager(this);
    connect(workerThread, SIGNAL(updageGameFoundState(bool)), this, SLOT(updageGameFoundState(bool)));
    connect(ui.listLogs->model(), SIGNAL(rowsInserted(QModelIndex, int, int)), ui.listLogs, SLOT(scrollToBottom()));

    connect(ui.chkAllowBypassLimit, SIGNAL(stateChanged(int)), this, SLOT(handleSettingBypassLimit(int)));
    connect(ui.chkSupportFullscreen, SIGNAL(stateChanged(int)), this, SLOT(handleSettingFullscreenSupport(int)));

    connect(ui.radioModeNop, SIGNAL(clicked()), this, SLOT(handleSettingModeNop()));
    connect(ui.radioModeTrad, SIGNAL(clicked()), this, SLOT(handleSettingModeTraditional()));
    connect(ui.radioModePinyin, SIGNAL(clicked()), this, SLOT(handleSettingModePinyin()));
    connect(ui.radioModeEnglish, SIGNAL(clicked()), this, SLOT(handleSettingModeTranslate()));
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
            ui.lblCurrentStatus->setText(QString::fromUtf8(u8"中文输入工具就绪"));
            ui.lblCurrentStatus->setStyleSheet("color: rgb(85, 170, 0);\nfont: 12pt \"微软雅黑\";");
        } else {
            ui.mainContent->setEnabled(false);
            ui.lblCurrentStatus->setText(QString::fromUtf8(u8"正在等待游戏启动"));
            ui.lblCurrentStatus->setStyleSheet("color: rgb(255, 0, 0);\nfont: 12pt \"微软雅黑\";");
        }
    }
}

void BattlefieldChat::handleSettingBypassLimit(int checked) {
    allowExceedLimit = checked == Qt::Checked;
}

void BattlefieldChat::handleSettingFullscreenSupport(int checked) {
    fullscreenSupport = checked == Qt::Checked;
}

void BattlefieldChat::handleSettingModeNop() {
    preprocessor = &SINGLETON_PREPROCESSOR_NOP;
}

void BattlefieldChat::handleSettingModeTraditional() {
    preprocessor = &SINGLETON_PREPROCESSOR_TRAD;
}

void BattlefieldChat::handleSettingModePinyin() {
    preprocessor = &SINGLETON_PREPROCESSOR_PINYIN;
}

void BattlefieldChat::handleSettingModeTranslate() {
    preprocessor = &SINGLETON_PREPROCESSOR_ENGLISH;
}