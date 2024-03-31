#include "file_scanner.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <locale>
#include <thread>


using namespace std::chrono_literals;


FileScanner::FileScanner(std::string filename){
    this->file_stream = new std::ifstream(filename);

    if(!this->file_stream->is_open()){
        std::cout << "Could not open file" << std::endl;
        exit(1);
    }
}


FileScanner::~FileScanner(){
    delete file_stream;
}

inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

Token* FileScanner::nextWord(){
    uint8_t tokenType = TOKEN_TYPE_MIC;
    std::string number = "";

    while(number.empty()){
        if(file_stream->eof()){
            file_stream->clear();
            file_stream->seekg(0);
        }
        getline(*file_stream, number);
        rtrim(number);
    }
    
    if(number.at(0) == DIAL_PREFIX){
        tokenType = TOKEN_TYPE_DIAL;
        number = number.substr(1);
    } else {
        std::this_thread::sleep_for(300us);
    }
    
    return new Token {
        std::stod(number),
        tokenType
    };
};