#include <cctype>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

#include <stdio.h>

#include <QApplication>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QLabel>

#include "MainComponent.h"
#include "serial_controller.h"


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
    });

    QLabel label2("Frequency");
    QSlider slider2(Qt::Horizontal);
    slider2.setRange(20, 1000);
    QObject::connect(&slider2, &QSlider::valueChanged, [&label2, mc](int value) {
        label2.setText(QString("Frequency: %1").arg(value));
    });

    QLabel label3("Slide Position");
    QSlider slider3(Qt::Horizontal);
    slider3.setRange(250, 350);
    QObject::connect(&slider3, &QSlider::valueChanged, [&label3, mc](int value) {
        label3.setText(QString("Slide Position: %1").arg(value));
    });

    layout.addWidget(&label1);
    layout.addWidget(&slider1);
    layout.addWidget(&label2);
    layout.addWidget(&slider2);
    layout.addWidget(&label3);
    layout.addWidget(&slider3);

    window.show();

    SerialController serialController = SerialController(mc);

    std::thread serial_controller_thread(std::bind(&SerialController::start, &serialController));
    serial_controller_thread.detach();

    return app.exec();
}