#pragma once
#include <QColor>
#include <sstream>

class Log : public std::ostringstream {
public:
    Log() {
    };

    Log::~Log();
};

void LogColor(Qt::GlobalColor color);