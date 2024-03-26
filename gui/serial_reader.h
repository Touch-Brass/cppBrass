#pragma once

#include <string>


class SerialReader{

    public:
        SerialReader(std::string port, int bufferSize);
        ~SerialReader();

        std::string port;
        int bufferSize;

        int device;

        char* serialBuffer;
        int bytes_read = 0;
        int buffer_i = 0;

        char nextChar();


    private:
        void loadBuffer();

};