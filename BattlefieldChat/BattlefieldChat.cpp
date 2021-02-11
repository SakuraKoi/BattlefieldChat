#include "BattlefieldChat.h"
#include <QCloseEvent>

BattlefieldChat::BattlefieldChat(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
    
}

void BattlefieldChat::closeEvent(QCloseEvent* event) {
    // TODO: cleanup
    event->accept();
}