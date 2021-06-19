#pragma once

#include <string>
#include <QString>
#include "Translator.h"

class Preprocessor { // Nop, just convert to utf8 and return
public:
    virtual std::string process(QString input);
    virtual void initialize(); // called when input dialog shown
    virtual QString enterPressed(QString input); // return NULL to perform prrocess and send, else replace
    virtual QString escPressed(QString input); // return NULL to cancel input, else replace
    virtual bool shouldValidateLength();
};

class TraditionalChinesePreprocessor : public Preprocessor {
public:
    std::string process(QString input);
};

class PinyinPreprocessor : public Preprocessor {
public:
    std::string process(QString input);
};

extern Translator* translatorProvider;

class TranslateToEnglishPreprocessor : public Preprocessor {
public:
    std::string process(QString input);
    void initialize();
    QString enterPressed(QString input);
    QString escPressed(QString input);
    bool shouldValidateLength();
};
