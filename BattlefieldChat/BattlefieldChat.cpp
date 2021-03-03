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

    connect(ui.chkUseProxy, SIGNAL(toggled(bool)), this, SLOT(handleSettingProxyEnabled(bool)));
    connect(ui.editProxyHost, SIGNAL(editingFinished()), this, SLOT(handleSettingProxyHost()));
    connect(ui.editProxyPort, SIGNAL(editingFinished()), this, SLOT(handleSettingProxyPort()));

    loadConfiguration();
}

void BattlefieldChat::loadConfiguration() {
    ui.chkAllowBypassLimit->setChecked(settings->value(SETTING_KEY_bypassLimit, false).toBool());
    ui.chkSupportFullscreen->setChecked(settings->value(SETTING_KEY_fullscreenSupport, false).toBool());

    ui.chkUseProxy->setChecked(settings->value(SETTING_KEY_proxyEnabled, false).toBool());
    ui.editProxyHost->setText(settings->value(SETTING_KEY_proxyHost, "127.0.0.1").toString());
    ui.editProxyPort->setValue(settings->value(SETTING_KEY_proxyPort, 1080).toInt());


    switch(settings->value(SETTING_KEY_preprocessorMode, 1).toInt()) {
    case 0:
        preprocessor = &SINGLETON_PREPROCESSOR_NOP;
        break;
    default:
    case 1:
        preprocessor = &SINGLETON_PREPROCESSOR_TRAD;
        break;
    case 2:
        preprocessor = &SINGLETON_PREPROCESSOR_PINYIN;
        break;
    case 3:
        preprocessor = &SINGLETON_PREPROCESSOR_ENGLISH;
        break;
    }
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
    settings->sync();
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
    settings->setValue(SETTING_KEY_bypassLimit, allowExceedLimit);
}

void BattlefieldChat::handleSettingFullscreenSupport(int checked) {
    fullscreenSupport = checked == Qt::Checked;
    settings->setValue(SETTING_KEY_fullscreenSupport, fullscreenSupport);
}

void BattlefieldChat::handleSettingModeNop() {
    preprocessor = &SINGLETON_PREPROCESSOR_NOP;
    settings->setValue(SETTING_KEY_preprocessorMode, 0);
}

void BattlefieldChat::handleSettingModeTraditional() {
    preprocessor = &SINGLETON_PREPROCESSOR_TRAD;
    settings->setValue(SETTING_KEY_preprocessorMode, 1);
}

void BattlefieldChat::handleSettingModePinyin() {
    preprocessor = &SINGLETON_PREPROCESSOR_PINYIN;
    settings->setValue(SETTING_KEY_preprocessorMode, 2);
}

void BattlefieldChat::handleSettingModeTranslate() {
    preprocessor = &SINGLETON_PREPROCESSOR_ENGLISH;
    settings->setValue(SETTING_KEY_preprocessorMode, 3);
}

void BattlefieldChat::handleSettingProxyEnabled(bool checked) {
    settings->setValue(SETTING_KEY_proxyEnabled, checked);
    if (checked) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(ui.editProxyHost->text());
        proxy.setPort(ui.editProxyPort->value());
        network->setProxy(proxy);
    } else {
        network->setProxy(QNetworkProxy::NoProxy);
    }
}

void BattlefieldChat::handleSettingProxyHost() {
    settings->setValue(SETTING_KEY_proxyHost, ui.editProxyHost->text());
    if (ui.chkUseProxy->isChecked()) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(ui.editProxyHost->text());
        proxy.setPort(ui.editProxyPort->value());
        network->setProxy(proxy);
    }
}

void BattlefieldChat::handleSettingProxyPort() {
    settings->setValue(SETTING_KEY_proxyPort, ui.editProxyPort->value());
    if (ui.chkUseProxy->isChecked()) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(ui.editProxyHost->text());
        proxy.setPort(ui.editProxyPort->value());
        network->setProxy(proxy);
    }
}