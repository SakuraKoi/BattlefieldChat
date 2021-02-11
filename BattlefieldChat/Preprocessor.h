#pragma once
#include <string>
#include <QString>

class Preprocessor { // Nop, just convert to utf8 and return
public:
    virtual std::string preprocess(QString input);
};

class TraditionalChinesePreprocessor : public Preprocessor {
public:
    std::string preprocess(QString input);
};

class PinyinPreprocessor : public Preprocessor {
public:
    std::string preprocess(QString input);
};

class TranslateToEnglishPreprocessor : public Preprocessor {
public:
    std::string preprocess(QString input);
};
