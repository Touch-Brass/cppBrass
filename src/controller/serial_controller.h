#pragma once

#include "MainComponent.h"
#include "observer.h"
#include "serial_scanner.h"
#include "sliding_dft.h"

#define SAMPLES_PER_FFT 1000
#define SAMPLE_RATE 1.0f / 0.0003f

#define SERIAL_PORT "/dev/ttyUSB0"
#define SERIAL_BUFFER_SIZE 1024


class SerialController{

    public:
        SerialController(MainComponent* mc);
        SerialController(MainComponent* mc, Observer* observer);
        ~SerialController();

        void start();
        void stop();

    private:
        MainComponent* mc;
        Observer* observer;

        SerialScanner serialScanner = SerialScanner(SERIAL_PORT, SERIAL_BUFFER_SIZE);;
        SlidingDFT<double, SAMPLES_PER_FFT> sdft = SlidingDFT<double, SAMPLES_PER_FFT>();

        bool spinning = true;

};