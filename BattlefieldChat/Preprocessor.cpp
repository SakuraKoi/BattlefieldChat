#include "Preprocessor.h"

std::string Preprocessor::preprocess(QString input) {
    return input.toStdString();
}

std::string TraditionalChinesePreprocessor::preprocess(QString input) {
    // FIXME: Not implemented
    return input.toStdString();
}

std::string PinyinPreprocessor::preprocess(QString input) {
    // FIXME: Not implemented
    return input.toStdString();
}

std::string TranslateToEnglishPreprocessor::preprocess(QString input) {
    // FIXME: Not implemented
    return input.toStdString();
}