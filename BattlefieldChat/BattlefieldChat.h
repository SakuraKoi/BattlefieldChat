#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BattlefieldChat.h"

class BattlefieldChat : public QMainWindow
{
    Q_OBJECT

public:
    BattlefieldChat(QWidget *parent = Q_NULLPTR);
private:
    void closeEvent(QCloseEvent* event);

    Ui::BattlefieldChatClass ui;
};
