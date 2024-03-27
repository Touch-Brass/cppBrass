#include <cctype>
#include <chrono>
#include <iostream>
#include <cmath>
#include <string>
#include <thread>

#include <stdio.h>

#include <QApplication>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QLabel>

#include "serial_reader.h"
#include "MainComponent.h"
#include "sliding_dft.h"

#define SAMPLES_PER_FFT 1000
#define SAMPLE_RATE 1.0f / 0.0003f

#define SERIAL_PORT "/dev/ttyUSB0"
#define SERIAL_BUFFER_SIZE 1024

#define DIAL_PREFIX 'd'


SerialReader serialIn = SerialReader(SERIAL_PORT, SERIAL_BUFFER_SIZE);
SlidingDFT<double, SAMPLES_PER_FFT> dft = SlidingDFT<double, SAMPLES_PER_FFT>();


std::pair<int, int> getFundamentalFrequency(){
    for(int i = 10; i < SAMPLES_PER_FFT / 2; i++){
        if(std::abs(dft.dft[i]) > 10000){
            return std::pair<int, int>(i * SAMPLE_RATE / SAMPLES_PER_FFT, std::abs(dft.dft[i]));
        }
    }
    return std::pair<int, int>(100, 0);
}


void read_serial(QSlider* pressureSlider, QSlider* frequencySlider, QSlider* positionSlider){
    int i = 0;

    while(true){
        bool isDialValue = false;
        std::string number = "";

        while(true){
            char next_char = serialIn.nextChar();

            if(isdigit(next_char)){
                number += next_char;
            } else if(next_char == DIAL_PREFIX){
                isDialValue = true;
            } else {
                break;
            }
        }

        if(number.empty()) continue;
        
        if(isDialValue){
            QMetaObject::invokeMethod(positionSlider, "setValue", Qt::QueuedConnection, Q_ARG(int, std::stod(number.c_str()) * 100 / 1023 + 250));
        } else {
            dft.update(std::stod(number.c_str()));
        }


        if(i > 100){
            std::pair<int, int> fundamental = getFundamentalFrequency();
            std::cout << "(" << fundamental.first << ", " << fundamental.second << ")" << std::endl;
            QMetaObject::invokeMethod(frequencySlider, "setValue", Qt::QueuedConnection, Q_ARG(int, (int)fundamental.first));
            QMetaObject::invokeMethod(pressureSlider, "setValue", Qt::QueuedConnection, Q_ARG(int, (int)fundamental.second *500 / 50000));
            i = 0;
        }

        i++;
    }
}

int main(int argc, char **argv)
{
    MainComponent* mc = new MainComponent();
    mc->prepareToPlay(44100);
    mc->startPlaying();

    QApplication app(argc, argv);

    QWidget window;
    QVBoxLayout layout(&window);

    QLabel label1("Pressure");
    QSlider slider1(Qt::Horizontal);
    slider1.setRange(0, 500);
    QObject::connect(&slider1, &QSlider::valueChanged, [&label1, mc](int value) {
        label1.setText(QString("Pressure: %1").arg(value));
        mc->pressureVal = value * 50;
    });

    QLabel label2("Frequency");
    QSlider slider2(Qt::Horizontal);
    slider2.setRange(20, 1000);
    QObject::connect(&slider2, &QSlider::valueChanged, [&label2, mc](int value) {
        label2.setText(QString("Frequency: %1").arg(value));
        mc->lipFreqVal = value;
    });

    QLabel label3("Slide Position");
    QSlider slider3(Qt::Horizontal);
    slider3.setRange(250, 350);
    QObject::connect(&slider3, &QSlider::valueChanged, [&label3, mc](int value) {
        label3.setText(QString("Slide Position: %1").arg(value));
        mc->LVal = (float) value / 100;
    });

    layout.addWidget(&label1);
    layout.addWidget(&slider1);
    layout.addWidget(&label2);
    layout.addWidget(&slider2);
    layout.addWidget(&label3);
    layout.addWidget(&slider3);

    window.show();

    std::thread read_serial_thread(read_serial, &slider1, &slider2, &slider3);
    read_serial_thread.detach();


    return app.exec();
}