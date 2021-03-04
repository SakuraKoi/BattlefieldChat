#pragma once
#include <QString>

class TranslateException {
public:
    QString reason;
};

class Translator {
public:
    virtual QString translate(QString text) = 0; // throws TranslateException
};

extern QString baiduAppid;
extern QString baiduKey;

class BaiduTranslator : public Translator {
public:
    QString translate(QString text);
}; 

class DeepLTranslator : public Translator {
public:
    QString translate(QString text);
};

extern BaiduTranslator SINGLETON_TRANSLATOR_BAIDU;
extern DeepLTranslator SINGLETON_TRANSLATOR_DEEPL;