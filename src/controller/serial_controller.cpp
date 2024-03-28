#include "serial_controller.h"

#include "peak_detection.h"

void noop(double){}

SerialController::SerialController(MainComponent* mc){
    this->mc = mc;
    this->observer = new Observer { noop, noop, noop };
}


SerialController::SerialController(MainComponent* mc, Observer* observer){
    this->mc = mc;
    this->observer = observer;
}


SerialController::~SerialController(){
    this->stop();
}


void SerialController::start(){
    spinning = true;

    int i = 0;
    while(spinning){
        Token* next_word = serialScanner.nextWord();
        
        switch(next_word->tokenType){
            
            case TOKEN_TYPE_MIC: {
                sdft.update(next_word->value);
                break;
            }

            case TOKEN_TYPE_DIAL: {
                double slidePos = (float) next_word->value * 100 / 1024 + 250;
                this->mc->LVal = (float) slidePos;
                this->observer->updateSlidePosition(slidePos);
                break;
            }
            
            default: {
                std::cout << "Word Error" << std::endl;
                continue;
            }

        }


        if(i > 100){
            std::pair<double, double> fundamental = getFundamentalFrequency(this->sdft);
            double pressure = fundamental.second * 500 / 50000;

            mc->lipFreqVal = fundamental.first;
            mc->pressureVal = pressure;
            
            observer->updateFrequency(fundamental.first);
            observer->updatePressure(pressure);

            i = 0;
        }

        i++;
    }
}


void SerialController::stop(){
    spinning = false;
}