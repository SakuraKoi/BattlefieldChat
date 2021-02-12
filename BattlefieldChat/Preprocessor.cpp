#include "Preprocessor.h"

std::string Preprocessor::process(QString input) {
    return input.toStdString();
}

std::string TraditionalChinesePreprocessor::process(QString input) {
    // FIXME: Not implemented
    return input.toStdString();
}

std::string PinyinPreprocessor::process(QString input) {
    // FIXME: Not implemented
    return input.toStdString();
}

std::string TranslateToEnglishPreprocessor::process(QString input) {
    // FIXME: Not implemented
    return input.toStdString();
}