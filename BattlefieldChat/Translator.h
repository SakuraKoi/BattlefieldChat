#pragma once
#include <QString>

class Translator {
public:
    virtual QString translate(QString text) = 0; // can throw utf8 std::string
};

class BaiduTranslator : public Translator {
public:
    QString translate(QString text);
}; 

class DeepLTranslator : public Translator {
public:
    QString translate(QString text);
};