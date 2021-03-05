#include "Log.h"
#include "GlobalVariables.h"

Log::~Log() {
    mainWindow->pushLog(QString::fromUtf8(str().data()));
}

void LogColor(Qt::GlobalColor color) {
    mainWindow->logColor(color);
}