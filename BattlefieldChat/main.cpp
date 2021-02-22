#include "BattlefieldChat.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BattlefieldChat w;
    w.show();
    return a.exec();
}
