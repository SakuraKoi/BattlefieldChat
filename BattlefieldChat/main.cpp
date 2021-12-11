#include "BattlefieldChat.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true); // FIXME: 不完美实现

    QApplication a(argc, argv);

    BattlefieldChat w;
    w.show();
    return a.exec();
}
