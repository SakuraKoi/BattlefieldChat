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
    void updageGameFoundState(bool found);
    void handleSettingBypassLimit(int checked);
    void handleSettingFullscreenSupport(int checked);
    void handleSettingModeNop();
    void handleSettingModeTraditional();
    void handleSettingModePinyin();
    void handleSettingModeTranslate();

private:
    void closeEvent(QCloseEvent* event);
    void showEvent(QShowEvent* event);
    void loadConfiguration();
    Ui::BattlefieldChatClass ui;

};
