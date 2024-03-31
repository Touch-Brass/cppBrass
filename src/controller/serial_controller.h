#pragma once

#include "MainComponent.h"
#include "observer.hpp"
#include "input_scanner.h"
#include "sliding_dft.h"

#define SAMPLES_PER_FFT 1000
#define SAMPLE_RATE 1.0f / 0.0003f


class SerialController{

    public:
        SerialController(MainComponent* mc, InputScanner* inputScanner);
        SerialController(MainComponent* mc, InputScanner* inputScanner, Observer* observer);
        ~SerialController();

        void start();
        void stop();

    private:
        MainComponent* mc;
        Observer* observer;

        InputScanner* inputScanner;
        SlidingDFT<double, SAMPLES_PER_FFT> sdft = SlidingDFT<double, SAMPLES_PER_FFT>();

        bool spinning = true;

};