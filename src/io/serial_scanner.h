#pragma once

#include "serial_reader.h"

#include <string>


#define DIAL_PREFIX 'd'

#define TOKEN_TYPE_MIC 0
#define TOKEN_TYPE_DIAL 1
#define TOKEN_ERROR 7


struct Token{
    double value;
    uint8_t tokenType;
};


class SerialScanner{

    public:
        SerialScanner(std::string port, int bufferSize);
        ~SerialScanner();

        Token* nextWord();

    private:
        SerialReader* serialReader;

};
