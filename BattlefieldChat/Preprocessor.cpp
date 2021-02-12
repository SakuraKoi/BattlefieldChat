#include "Preprocessor.h"

std::string Preprocessor::process(QString input) {
    return input.toStdString();
}

QString Preprocessor::enterPressed(QString input) {
    return Q_NULLPTR;
}

QString Preprocessor::escPressed(QString input) {
    return Q_NULLPTR;
}

std::string TraditionalChinesePreprocessor::process(QString input) {
    // FIXME: Not implemented
    return input.toStdString();
}

QString TraditionalChinesePreprocessor::enterPressed(QString input) {
    return Q_NULLPTR;
}

QString TraditionalChinesePreprocessor::escPressed(QString input) {
    return Q_NULLPTR;
}

std::string PinyinPreprocessor::process(QString input) {
    // FIXME: Not implemented
    return input.toStdString();
}

QString PinyinPreprocessor::enterPressed(QString input) {
    return Q_NULLPTR;
}

QString PinyinPreprocessor::escPressed(QString input) {
    return Q_NULLPTR;
}

std::string TranslateToEnglishPreprocessor::process(QString input) {
    // FIXME: Not implemented
    return input.toStdString();
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