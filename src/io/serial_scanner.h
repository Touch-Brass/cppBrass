#pragma once

#include "input_scanner.h"

#include <string>

#include "serial_reader.h"


class SerialScanner : public InputScanner{

    public:
        SerialScanner(std::string port, int bufferSize);
        ~SerialScanner();

        Token* nextWord();

    private:
        SerialReader* serialReader;

};
