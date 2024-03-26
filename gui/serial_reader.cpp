#include "serial_reader.h"

#include <fcntl.h>
#include <unistd.h>


SerialReader::SerialReader(std::string port, int bufferSize){
    this->port = port;
    this->bufferSize = bufferSize;
    this->device = open(this->port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    this->serialBuffer = (char*)malloc(sizeof(char) * this->bufferSize);
}


SerialReader::~SerialReader(){
    free(serialBuffer);
    close(device);
}


char SerialReader::nextChar(){
    while(buffer_i > bytes_read - 1)
        loadBuffer();

    return serialBuffer[buffer_i++];
}


void SerialReader::loadBuffer(){
    buffer_i = 0;
    bytes_read = read(device, serialBuffer, sizeof(char) * bufferSize);
}