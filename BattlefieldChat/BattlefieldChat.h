#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BattlefieldChat.h"

class BattlefieldChat : public QMainWindow
{
    Q_OBJECT

public:
    BattlefieldChat(QWidget *parent = Q_NULLPTR);
    void pushLog(std::string message);

private:
    void closeEvent(QCloseEvent* event);
    void showEvent(QShowEvent* event);

    Ui::BattlefieldChatClass ui;

};
