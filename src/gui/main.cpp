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

using namespace std::placeholders;


class Application{

    public:
        Application(int argc, char **argv){
            QApplication app(argc, argv);

            QWidget window;
            QVBoxLayout layout(&window);

            pressureLabel = new QLabel("Pressure");
            pressureSlider = new QSlider(Qt::Horizontal);
            pressureSlider->setRange(0, 500);
            QObject::connect(pressureSlider, &QSlider::valueChanged, [this](int value) {
                pressureLabel->setText(QString("Pressure: %1").arg(value));
            });

            frequencyLabel = new QLabel("Frequency");
            frequencySlider = new QSlider(Qt::Horizontal);
            frequencySlider->setRange(20, 1000);
            QObject::connect(frequencySlider, &QSlider::valueChanged, [this](int value) {
                frequencyLabel->setText(QString("Frequency: %1").arg(value));
            });

            slideLabel = new QLabel("Slide Position");
            slideSlider = new QSlider(Qt::Horizontal);
            slideSlider->setRange(250, 350);
            QObject::connect(slideSlider, &QSlider::valueChanged, [this](int value) {
                slideLabel->setText(QString("Slide Position: %1").arg(value));
            });

            layout.addWidget(pressureLabel);
            layout.addWidget(pressureSlider);
            layout.addWidget(frequencyLabel);
            layout.addWidget(frequencySlider);
            layout.addWidget(slideLabel);
            layout.addWidget(slideSlider);

            window.show();

            app.exec();
        }

        void updatePressure(double pressure){
            pressureSlider->setValue(pressure);
        }

        void updateFrequency(double frequency){
            frequencySlider->setValue(frequency);
        }

        void updateSlidePosition(double slidePosition){
            slideSlider->setValue(slidePosition);
        }
    
    private:
        QLabel* pressureLabel;
        QSlider* pressureSlider;
        QLabel* frequencyLabel;
        QSlider* frequencySlider;
        QLabel* slideLabel;
        QSlider* slideSlider;

};

int main(int argc, char **argv)
{
    MainComponent* mc = new MainComponent();
    mc->prepareToPlay(44100);
    mc->startPlaying();

    Application* app = new Application(argc, argv);

    Observer* obs = new Observer {
        std::bind(&Application::updatePressure, app, _1),
        std::bind(&Application::updateFrequency, app, _1),
        std::bind(&Application::updateSlidePosition, app, _1)
    };
    SerialController serialController = SerialController(mc, obs);

    std::thread serial_controller_thread(std::bind(&SerialController::start, &serialController));
    serial_controller_thread.detach();

    return 0;
}