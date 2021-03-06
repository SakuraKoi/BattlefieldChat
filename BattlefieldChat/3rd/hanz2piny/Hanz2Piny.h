﻿// last modified 

#ifndef HANZ2PINY_H
#define HANZ2PINY_H

#include <string>
#include <vector>


class Hanz2Piny {
public:
    typedef unsigned short Unicode;
    enum Polyphone { all, name, noname };

public:
    Hanz2Piny();

    bool isHanziUnicode(const Unicode unicode);
    std::vector<std::string> toPinyinFromUnicode(const Unicode hanzi_unicode, const bool with_tone = true);

    bool isUtf8(const std::string& s);
    std::vector<std::pair<bool, std::vector<std::string>>> toPinyinFromUtf8(const std::string& s,
        const bool with_tone = true,
        const bool replace_unknown = false,
        const std::string& replace_unknown_with = "");

    bool isStartWithBom(const std::string& s);


private:
    static const Unicode begin_hanzi_unicode_, end_hanzi_unicode_;
    static const char* pinyin_list_with_tone_[];
};

#endif

