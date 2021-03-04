#include "Preprocessor.h"
#include "Utils.h"
#include "GlobalVariables.h"

std::string Preprocessor::process(QString input) {
    return std::string(input.toLocal8Bit().data());
}

void Preprocessor::initialize() {
}

QString Preprocessor::enterPressed(QString input) {
    return Q_NULLPTR;
}

QString Preprocessor::escPressed(QString input) {
    return Q_NULLPTR;
}

bool Preprocessor::shouldValidateLength() {
    return true;
}

std::wstring replaceNonDisplayableCharacters(std::wstring str) {
    str = ReplaceWCSWithPattern(str, L"Ι¶", L"Κ²χα");
    str = ReplaceWCSWithPattern(str, L"Γ΄", L"χα");
    return str;
}

std::string TraditionalChinesePreprocessor::process(QString input) {
    std::wstring inputStr = input.toStdWString();
    std::wstring replaced = replaceNonDisplayableCharacters(inputStr);
    std::wstring trad = CHS2CHT(replaced);
    return WStrToStr(trad);
}

std::string PinyinPreprocessor::process(QString input) {
    // FIXME: Not implemented
    return std::string(input.toLocal8Bit().data());
}

std::string TranslateToEnglishPreprocessor::process(QString input) {
    return std::string(input.toLocal8Bit().data());
}

Translator* translatorProvider;
QString originalChineseText = Q_NULLPTR;

void TranslateToEnglishPreprocessor::initialize() {
    originalChineseText = Q_NULLPTR;
}

QString TranslateToEnglishPreprocessor::enterPressed(QString input) {
    if (originalChineseText != Q_NULLPTR)
        return Q_NULLPTR;
    QString result = translatorProvider->translate(input);
    originalChineseText = input;
    if (translateKeepOriginal)
        return input + " / " + result;

    return result;
}

QString TranslateToEnglishPreprocessor::escPressed(QString input) {
    if (originalChineseText != Q_NULLPTR) {
        QString temp = originalChineseText;
        originalChineseText = Q_NULLPTR;
        return temp;
    }
    return Q_NULLPTR;
}

bool TranslateToEnglishPreprocessor::shouldValidateLength() {
    return originalChineseText != Q_NULLPTR; // true if translated
}