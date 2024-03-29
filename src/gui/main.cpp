#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <stdio.h>

#include <QApplication>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QLabel>

#include "MainComponent.h"
#include "serial_controller.h"
#include "freq_to_note.h"

using namespace std::placeholders;


class Application{

    public:
        Application(){
            // window.setBaseSize(500, 500);
            QGridLayout layout(&window);

            titleLabel = new QLabel("Digital Trombone");
            titleLabel->setStyleSheet("font-size: 25px;");

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

            noteLabel = new QLabel("Note: ");

            layout.addWidget(titleLabel, 1, 1, 1, 2);
            layout.addWidget(pressureLabel, 2, 1);
            layout.addWidget(pressureSlider, 3, 1);
            layout.addWidget(frequencyLabel, 4, 1);
            layout.addWidget(frequencySlider, 5, 1);
            layout.addWidget(slideLabel, 6, 1);
            layout.addWidget(slideSlider, 7, 1);
            layout.addWidget(noteLabel, 2, 2);

            window.show();
        }

        void updatePressure(double pressure){
            pressureSlider->setValue(pressure);
        }

        void updateFrequency(double frequency){
            frequencySlider->setValue(frequency);
            std::pair<std::string, int> note = freq_to_note(frequency);
            noteLabel->setText(QString("%1 %2").arg(note.first.c_str(), std::to_string(note.second).c_str()));
        }

        void updateSlidePosition(double slidePosition){
            slideSlider->setValue(slidePosition * 100);
        }
    
    private:
        QWidget window;

        QLabel* titleLabel;

        QLabel* pressureLabel;
        QSlider* pressureSlider;
        QLabel* frequencyLabel;
        QSlider* frequencySlider;
        QLabel* slideLabel;
        QSlider* slideSlider;

        QLabel* noteLabel;

};

int main(int argc, char **argv)
{
    MainComponent* mc = new MainComponent();
    mc->prepareToPlay(44100);
    mc->startPlaying();

    QApplication app(argc, argv);
    Application* gui = new Application();

    Observer* obs = new Observer {
        std::bind(&Application::updatePressure, gui, _1),
        std::bind(&Application::updateFrequency, gui, _1),
        std::bind(&Application::updateSlidePosition, gui, _1)
    };
    SerialController* serialController = new SerialController(mc, obs);

    std::thread serial_controller_thread(std::bind(&SerialController::start, serialController));
    serial_controller_thread.detach();

    return app.exec();
}