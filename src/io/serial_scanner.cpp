#include "serial_scanner.h"

#include <iostream>

SerialScanner::SerialScanner(std::string port, int bufferSize){
    this->serialReader = new SerialReader(port, bufferSize);
}


SerialScanner::~SerialScanner(){
    delete serialReader;
}


Token* SerialScanner::nextWord(){
    uint8_t tokenType = TOKEN_TYPE_MIC;
    std::string number = "";

    while(number.empty()){
        while(true){
            char next_char = serialReader->nextChar();

            if(isdigit(next_char)){
                number += next_char;
            } else if(next_char == DIAL_PREFIX){
                tokenType = TOKEN_TYPE_DIAL;
            } else {
                break;
            }
        }
    }
    
    return new Token {
        std::stod(number),
        tokenType
    };
};