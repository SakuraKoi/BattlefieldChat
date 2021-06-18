#include "BattlefieldChat.h"
#include "GlobalVariables.h"
#include "TaskExecuteThread.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QDesktopServices>
#include <QPushButton>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QPainter>

TaskExecuteThread* taskExecuteThread;

BattlefieldChat::BattlefieldChat(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.mainContent->setEnabled(false);
    setWindowTitle(QString::fromUtf8(u8"Battlefield 1 中文输入工具 v") + currentVersion + addtionalVersion);
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
    mainWindow = this;
    inputWindow = new InputDialog();
    workerThread = new WorkerThread();
    updateCheckerTask = new UpdateCheckerTask();
    countLoaderTask = new CountLoaderTask();
    taskExecuteThread = new TaskExecuteThread();
    network = new QNetworkAccessManager(this);
    connect(workerThread, &WorkerThread::updateGameFoundState, this, &BattlefieldChat::updateGameFoundState);
    connect(updateCheckerTask, &UpdateCheckerTask::newVersionFound, this, &BattlefieldChat::handleNewVersionFound);
    connect(countLoaderTask, &CountLoaderTask::countLoaded, this, &BattlefieldChat::handleCountLoaded);

    connect(ui.listLogs->model(), SIGNAL(rowsInserted(QModelIndex, int, int)), ui.listLogs, SLOT(scrollToBottom()));

    connect(ui.chkAllowBypassLimit, SIGNAL(stateChanged(int)), this, SLOT(handleSettingBypassLimit(int)));
    connect(ui.chkSupportFullscreen, SIGNAL(stateChanged(int)), this, SLOT(handleSettingFullscreenSupport(int)));

    connect(ui.radioModeNop, SIGNAL(clicked()), this, SLOT(handleSettingModeNop()));
    connect(ui.radioModeTrad, SIGNAL(clicked()), this, SLOT(handleSettingModeTraditional()));
    connect(ui.radioModePinyin, SIGNAL(clicked()), this, SLOT(handleSettingModePinyin()));
    connect(ui.radioModeEnglish, SIGNAL(clicked()), this, SLOT(handleSettingModeTranslate()));

    connect(ui.radioProviderBaidu, SIGNAL(clicked()), this, SLOT(handleSettingTranslatorBaidu()));
    connect(ui.radioProviderDeepL, SIGNAL(clicked()), this, SLOT(handleSettingTranslatorDeepL()));

    connect(ui.chkTranslateKeepOriginal, SIGNAL(stateChanged(int)), this, SLOT(handleSettingTranslateKeepOriginal(int)));

    connect(ui.editTranslateBaiduAppid, SIGNAL(editingFinished()), this, SLOT(handleSettingBaiduAppid()));
    connect(ui.editTranslateBaiduKey, SIGNAL(editingFinished()), this, SLOT(handleSettingBaiduKey()));

    connect(ui.editTranslateTimeout, SIGNAL(editingFinished()), this, SLOT(handleSettingTranslateTimeout()));

    loadConfiguration();
}

void BattlefieldChat::loadConfiguration() {
    ui.chkAllowBypassLimit->setChecked(settings->value(SETTING_KEY_bypassLimit, false).toBool());
    ui.chkSupportFullscreen->setChecked(settings->value(SETTING_KEY_fullscreenSupport, false).toBool());

    switch(settings->value(SETTING_KEY_preprocessorMode, 1).toInt()) {
    case 0:
        preprocessor = &SINGLETON_PREPROCESSOR_NOP;
        ui.radioModeNop->setChecked(true);
        break;
    default:
    case 1:
        preprocessor = &SINGLETON_PREPROCESSOR_TRAD;
        ui.radioModeTrad->setChecked(true);
        break;
    case 2:
        preprocessor = &SINGLETON_PREPROCESSOR_PINYIN;
        ui.radioModePinyin->setChecked(true);
        break;
    case 3:
        preprocessor = &SINGLETON_PREPROCESSOR_ENGLISH;
        ui.radioModeEnglish->setChecked(true);
        break;
    }

    ui.chkTranslateKeepOriginal->setChecked(translateKeepOriginal = settings->value(SETTING_KEY_translateKeepOriginal, false).toBool());
    ui.editTranslateTimeout->setValue(translatorTimeout = settings->value(SETTING_KEY_translatorTimeout, 5000).toInt());
    
    switch (settings->value(SETTING_KEY_translatorProvider, 1).toInt()) {
    case 1:
    default:
        translatorProvider = &SINGLETON_TRANSLATOR_DEEPL;
        ui.radioProviderDeepL->setChecked(true);
        break;
    case 2:
        translatorProvider = &SINGLETON_TRANSLATOR_BAIDU;
        ui.radioProviderBaidu->setChecked(true);
        break;
    }

    ui.editTranslateBaiduAppid->setText(baiduAppid = settings->value(SETTING_KEY_translatorBaiduAppid, "").toString());
    ui.editTranslateBaiduKey->setText(baiduKey = settings->value(SETTING_KEY_translatorBaiduKey, "").toString());

}

void BattlefieldChat::showEvent(QShowEvent* ev) {
    QMainWindow::showEvent(ev);
    workerThread -> start();
    taskExecuteThread->start();
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
void BattlefieldChat::updateGameFoundState(bool found) {
    if (!shutdownPending && (lastGameState != found)) {
        lastGameState = found;
        if (found) {
            ui.mainContent->setEnabled(true);
            ui.lblCurrentStatus->setText(QString::fromUtf8(u8"中文输入工具就绪"));
            ui.lblCurrentStatus->setStyleSheet(u8"color: rgb(85, 170, 0);\nfont: 12pt \"微软雅黑\";");
        } else {
            ui.mainContent->setEnabled(false);
            ui.lblCurrentStatus->setText(QString::fromUtf8(u8"正在等待游戏启动"));
            ui.lblCurrentStatus->setStyleSheet(u8"color: rgb(255, 0, 0);\nfont: 12pt \"微软雅黑\";");
        }
    }
}

void BattlefieldChat::handleNewVersionFound(QString version, QString link) {
    pushLog(QString::fromUtf8(u8" [%] BattlefieldChat 新版本 v") + version + QString::fromUtf8(u8" 已发布!")); logColor(Qt::cyan);

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(u8"你从未用过的船新版本");
    msgBox.setText(u8"新版本 BattlefieldChat v" + version + u8" 已发布!                                                        ");
    msgBox.setInformativeText(link);
    QPushButton* accept = msgBox.addButton(u8"立即更新", QMessageBox::AcceptRole);
    QPushButton* cancel = msgBox.addButton(u8"咕咕咕", QMessageBox::RejectRole);
    msgBox.exec();

    if (msgBox.clickedButton() == accept) {
        QDesktopServices::openUrl(QUrl(link));
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

void BattlefieldChat::handleSettingTranslateKeepOriginal(int checked) {
    translateKeepOriginal = checked == Qt::Checked;
    settings->setValue(SETTING_KEY_translateKeepOriginal, translateKeepOriginal);
}

void BattlefieldChat::handleSettingBaiduAppid() {
    baiduAppid = ui.editTranslateBaiduAppid->text();
    settings->setValue(SETTING_KEY_translatorBaiduAppid, baiduAppid);
}

void BattlefieldChat::handleSettingBaiduKey() {
    baiduKey = ui.editTranslateBaiduKey->text();
    settings->setValue(SETTING_KEY_translatorBaiduKey, baiduKey);
}

void BattlefieldChat::handleSettingTranslateTimeout() {
    translatorTimeout = ui.editTranslateTimeout->value();
    settings->setValue(SETTING_KEY_translatorTimeout, translatorTimeout);
}

void BattlefieldChat::handleSettingTranslatorDeepL() {
    translatorProvider = &SINGLETON_TRANSLATOR_DEEPL;
    settings->setValue(SETTING_KEY_translatorProvider, 1);
}

void BattlefieldChat::handleSettingTranslatorBaidu() {
    translatorProvider = &SINGLETON_TRANSLATOR_BAIDU;
    settings->setValue(SETTING_KEY_translatorProvider, 2);
}

void BattlefieldChat::handleCountLoaded(QByteArray data) {
    QSvgRenderer renderer(data);
    QSize size = renderer.defaultSize();
    
    int height = size.height();
    int width = size.width();

    double rate = (double)width / height;
    height = ui.counterView->height();
    width = height * rate;

    QImage* image = new QImage(width, height, QImage::Format_ARGB32);
    image->fill(0xffffffff);

    QPainter painter(image);
    QFont font("Microsoft YaHei");
    font.setPointSize(painter.font().pointSize());
    painter.setFont(font);
    renderer.render(&painter);

    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
    QGraphicsScene* scene = new QGraphicsScene;
    scene->addItem(item);

    ui.counterView->setScene(scene);
}