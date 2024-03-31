#pragma once

#include <string>


#define DIAL_PREFIX 'd'

#define TOKEN_TYPE_MIC 0
#define TOKEN_TYPE_DIAL 1
#define TOKEN_ERROR 7


struct Token{
    double value;
    uint8_t tokenType;
};


class InputScanner{

    public:
        virtual Token* nextWord() = 0;

};
