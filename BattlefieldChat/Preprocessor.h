#pragma once
#include <string>
#include <QString>
#include "Translator.h"

class Preprocessor { // Nop, just convert to utf8 and return
public:
    virtual std::string process(QString input);
    virtual QString enterPressed(QString input); // return NULL to perform prrocess and send, else replace
    virtual QString escPressed(QString input); // return NULL to cancel input, else replace
    virtual bool shouldValidateLength();
};

class TraditionalChinesePreprocessor : public Preprocessor {
public:
    std::string process(QString input);
    QString enterPressed(QString input);
    QString escPressed(QString input);
    bool shouldValidateLength();
};

class PinyinPreprocessor : public Preprocessor {
public:
    std::string process(QString input);
    QString enterPressed(QString input);
    QString escPressed(QString input);
    bool shouldValidateLength();
};

extern Translator* translatorProvider;

class TranslateToEnglishPreprocessor : public Preprocessor {
public:
    std::string process(QString input);
    QString enterPressed(QString input);
    QString escPressed(QString input);
    bool shouldValidateLength();
};
