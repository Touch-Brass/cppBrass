#include "serial_controller.h"

#include "peak_detection.h"


void noop(double){}


SerialController::SerialController(MainComponent* mc, InputScanner* inputScanner){
    this->mc = mc;
    this->observer = new Observer { noop, noop, noop };
    this->inputScanner = inputScanner;
    this->sdft = new SlidingDFT<double, SAMPLES_PER_FFT>();
}


SerialController::SerialController(MainComponent* mc, InputScanner* inputScanner, Observer* observer){
    this->mc = mc;
    this->observer = observer;
    this->inputScanner = inputScanner;
    this->sdft = new SlidingDFT<double, SAMPLES_PER_FFT>();
}


SerialController::~SerialController(){
    this->stop();
}


void SerialController::start(){
    spinning = true;

    int i = 0;
    while(spinning){
        Token* next_word = inputScanner->nextWord();

        
        switch(next_word->tokenType){
            
            case TOKEN_TYPE_MIC: {
                sdft->update(next_word->value);
                break;
            }

            case TOKEN_TYPE_DIAL: {
                double slidePos = next_word->value / 1023. + 2.5;
                this->mc->LVal = slidePos;
                this->observer->updateSlidePosition(slidePos);
                break;
            }
            
            default: {
                std::cout << "Word Error" << std::endl;
                continue;
            }

        }


        if(i > 10){
            std::pair<double, double> fundamental = getFundamentalFrequency(this->sdft);

            if(fundamental.first == 10 && fundamental.second == 0){
                mc->pressureVal = 0;
                mc->lipFreqVal = 100;

                observer->updateFrequency(0);
                observer->updatePressure(0);
            } else {
                double pressure = (fundamental.second + mc->pressureVal) / 2;

                mc->lipFreqVal = fundamental.first;
                mc->pressureVal = pressure;
                
                observer->updateFrequency(fundamental.first);
                observer->updatePressure(pressure / 100);
            }

            i = 0;
        }

        i++;
    }
}


void SerialController::stop(){
    spinning = false;
}