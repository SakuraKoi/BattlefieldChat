#include "Preprocessor.h"
#include "Utils.h"

std::string Preprocessor::process(QString input) {
    return std::string(input.toLocal8Bit().data());
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

QString TraditionalChinesePreprocessor::enterPressed(QString input) {
    return Q_NULLPTR;
}

QString TraditionalChinesePreprocessor::escPressed(QString input) {
    return Q_NULLPTR;
}

bool TraditionalChinesePreprocessor::shouldValidateLength() {
    return true;
}

std::string PinyinPreprocessor::process(QString input) {
    // FIXME: Not implemented
    return std::string(input.toLocal8Bit().data());
}

QString PinyinPreprocessor::enterPressed(QString input) {
    return Q_NULLPTR;
}

QString PinyinPreprocessor::escPressed(QString input) {
    return Q_NULLPTR;
}

bool PinyinPreprocessor::shouldValidateLength() {
    return true;
}

std::string TranslateToEnglishPreprocessor::process(QString input) {
    // FIXME: Not implemented
    return std::string(input.toLocal8Bit().data());
}

QString TranslateToEnglishPreprocessor::enterPressed(QString input) {
    // FIXME: Not implemented
    // TODO enter to translate, again to send
    return Q_NULLPTR;
}

QString TranslateToEnglishPreprocessor::escPressed(QString input) {
    // FIXME: Not implemented
    // TODO esc to restore chinese, again to cancel
    return Q_NULLPTR;
}

bool TranslateToEnglishPreprocessor::shouldValidateLength() {
    // FIXME: Not implemented
    // TODO true if translated
    return false;
}