#include <vector>
#include <string>
#include "Preprocessor.h"
#include "Utils.h"
#include "GlobalVariables.h"
#include "./3rd/hanz2piny/Hanz2Piny.h"

using namespace std;

string Preprocessor::process(QString input) {
    return string(input.toLocal8Bit().data());
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

wstring replaceNonDisplayableCharacters(wstring str) {
    str = ReplaceWCSWithPattern(str, L"啥", L"什麽");
    str = ReplaceWCSWithPattern(str, L"么", L"麽");
    return str;
}

string TraditionalChinesePreprocessor::process(QString input) {
    wstring inputStr = input.toStdWString();
    wstring replaced = replaceNonDisplayableCharacters(inputStr);
    wstring trad = CHS2CHT(replaced);
    string result = WStrToStr(trad);

    if (translateKeepOriginal)
        result = result + " / " + SINGLETON_PREPROCESSOR_PINYIN.process(input);

    return result;
}

Hanz2Piny hanz2piny;

string PinyinPreprocessor::process(QString input) {
    vector<pair<bool, vector<string>>> pinyin_list_list = hanz2piny.toPinyinFromUtf8(string(input.toUtf8().constData()), false, false, "?");

    string out = "";
    for (const auto& e : pinyin_list_list) {
        auto pinyin_list = e.second;

        string pinyin;
        if (pinyin_list.size() >= 1) {
            pinyin = pinyin_list[0];
        } else {
            pinyin = "?";
        }

        out = out + " " + pinyin;
    }

    return out.substr(1, out.length()-1);
}

string TranslateToEnglishPreprocessor::process(QString input) {
    if (translateKeepOriginal)
        return SINGLETON_PREPROCESSOR_TRAD.process(input);
    return string(input.toLocal8Bit().data());
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