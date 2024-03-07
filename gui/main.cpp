#include <cctype>
#include <chrono>
#include <iostream>
#include <thread>

#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>

#include <QApplication>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QLabel>

#include "../src/MainComponent.h"

#define SAMPLES_PER_FFT 1000


void read_serial(QSlider* pressureSlider, QSlider* frequencySlider, QSlider* positionSlider){
    std::string port = "/dev/ttyUSB0";
    int device = open(port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    std::cout << device << std::endl;

    int16_t* readings = (int16_t*) malloc(sizeof(int16_t) * SAMPLES_PER_FFT);

    while(true){
        for(int i = 0; i < 1000; i++){

            std::string number = "";

            while(true){
                char next_char;
                int n = read(device, &next_char, sizeof(next_char));

                if(n == 0){
                    std::cout << "Sample Error" << std::endl;
                    continue;
                }

                // if(next_char == '\r' || next_char == '\n'){
                //     break;
                // }

                if(isdigit(next_char) || next_char == '.'){
                    number += next_char;
                } else {
                    break;
                }
            }

            if(number.empty()){
                i--;
                continue;
            }

            readings[i] = atoi(number.c_str());
            std::cout << readings[i] << std::endl;
        }
        
        // Perform FFT

        QMetaObject::invokeMethod(pressureSlider, "setValue", Qt::QueuedConnection, Q_ARG(int, 50));
        QMetaObject::invokeMethod(frequencySlider, "setValue", Qt::QueuedConnection, Q_ARG(int, 450));
        QMetaObject::invokeMethod(positionSlider, "setValue", Qt::QueuedConnection, Q_ARG(int, 300));
    }

    close(device);
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
    slider1.setRange(0, 100);
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