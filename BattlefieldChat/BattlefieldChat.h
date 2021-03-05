#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BattlefieldChat.h"

class BattlefieldChat : public QMainWindow
{
    Q_OBJECT

public:
    BattlefieldChat(QWidget *parent = Q_NULLPTR);
    void pushLog(QString message);
    void logColor(Qt::GlobalColor color);

public slots:
    void updateGameFoundState(bool found);
    void handleNewVersionFound(QString version, QString link);
    void handleSettingBypassLimit(int checked);
    void handleSettingFullscreenSupport(int checked);
    void handleSettingTranslateKeepOriginal(int checked);

    void handleSettingModeNop();
    void handleSettingModeTraditional();
    void handleSettingModePinyin();
    void handleSettingModeTranslate();

    void handleSettingProxyEnabled(bool checked);
    void handleSettingProxyHost();
    void handleSettingProxyPort();

    void handleSettingTranslatorBaidu();
    void handleSettingTranslatorDeepL();

    void handleSettingBaiduAppid();
    void handleSettingBaiduKey();
    void handleSettingTranslateTimeout();
private:
    void closeEvent(QCloseEvent* event);
    void showEvent(QShowEvent* event);
    void loadConfiguration();
    Ui::BattlefieldChatClass ui;

};
