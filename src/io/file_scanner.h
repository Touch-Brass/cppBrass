#pragma once

#include "input_scanner.h"


#include <fstream>
#include <string>

class FileScanner : public InputScanner{
    
    public:
        FileScanner(std::string filename);
        ~FileScanner();

        Token* nextWord();

    private:
        std::ifstream* file_stream;

};
