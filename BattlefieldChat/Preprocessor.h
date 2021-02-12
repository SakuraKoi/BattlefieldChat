#pragma once
#include <string>
#include <QString>

class Preprocessor { // Nop, just convert to utf8 and return
public:
    virtual std::string process(QString input);
};

class TraditionalChinesePreprocessor : public Preprocessor {
public:
    std::string process(QString input);
};

class PinyinPreprocessor : public Preprocessor {
public:
    std::string process(QString input);
};

class TranslateToEnglishPreprocessor : public Preprocessor {
public:
    std::string process(QString input);
};
