#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <stdio.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QLabel>

#include "MainComponent.h"
#include "serial_controller.h"
#include "freq_to_note.h"
#include "input_scanner.h"
#include "serial_scanner.h"
#include "file_scanner.h"

#define SERIAL_PORT "/dev/ttyUSB0"
#define SERIAL_BUFFER_SIZE 1024


using namespace std::placeholders;

class Application{

    public:
        Application(){
            window.setFixedSize(1000, 750);
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

            noteLayout = new QHBoxLayout(&window);

            for(int i = 0; i < 5; i++){
                QProgressBar* bar = new QProgressBar();
                bar->setOrientation(Qt::Vertical);
                bar->setFixedHeight(100);
                bar->setFixedWidth(10);
                bar->setValue(100);
                centBarsNegative.push_back(bar);
                noteLayout->addWidget(bar);
            }

            noteLabelLayout = new QVBoxLayout(&window);

            noteLabel = new QLabel("");
            noteLabel->setStyleSheet("font-size: 40px;");
            noteLabel->setAlignment(Qt::AlignCenter);
            noteLabelLayout->addWidget(noteLabel);

            freqLabel = new QLabel("");
            freqLabel->setStyleSheet("font-size: 25px;");
            freqLabel->setAlignment(Qt::AlignCenter);
            noteLabelLayout->addWidget(freqLabel);

            noteLayout->addLayout(noteLabelLayout);

            for(int i = 0; i < 5; i++){
                QProgressBar* bar = new QProgressBar();
                bar->setOrientation(Qt::Vertical);
                bar->setFixedHeight(100);
                bar->setFixedWidth(10);
                bar->setValue(100);
                centBarsPositive.push_back(bar);
                noteLayout->addWidget(bar);
            }


            layout.addWidget(titleLabel, 1, 1, 1, 2);
            layout.addWidget(pressureLabel, 2, 1);
            layout.addWidget(pressureSlider, 3, 1);
            layout.addWidget(frequencyLabel, 4, 1);
            layout.addWidget(frequencySlider, 5, 1);
            layout.addWidget(slideLabel, 6, 1);
            layout.addWidget(slideSlider, 7, 1);
            layout.addLayout(noteLayout, 2, 2, 3, 1);

            layout.setColumnStretch(1, 2);
            layout.setColumnStretch(2, 2);

            window.show();
        }

        void updatePressure(double pressure){
            QMetaObject::invokeMethod(pressureSlider, "setValue", Qt::QueuedConnection, Q_ARG(int, (int)pressure));
        }

        void updateFrequency(double frequency){
            QMetaObject::invokeMethod(frequencySlider, "setValue", Qt::QueuedConnection, Q_ARG(int, (int)frequency));
            std::pair<std::string, int> note = freq_to_note(frequency);
            QMetaObject::invokeMethod(noteLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, QString("%1").arg(note.first.c_str())));
            QMetaObject::invokeMethod(freqLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, QString("%1 %2").arg(std::to_string((int) frequency).c_str(), "Hz")));
            
            int cents = std::round((float)note.second / 10.);
            for(int i = 0; i < 5; i++){
                QMetaObject::invokeMethod(centBarsNegative.at(i), "setVisible", Qt::QueuedConnection, Q_ARG(bool, (bool)false));
                QMetaObject::invokeMethod(centBarsPositive.at(i), "setVisible", Qt::QueuedConnection, Q_ARG(bool, (bool)false));
            }
            
            if(cents < 0){
                for(int i = 0; i > cents; i--){
                    QMetaObject::invokeMethod(centBarsNegative.at(4 + i), "setVisible", Qt::QueuedConnection, Q_ARG(bool, (bool)true));
                }
            } else {
                for(int i = 0; i < cents; i++){
                    QMetaObject::invokeMethod(centBarsPositive.at(i), "setVisible", Qt::QueuedConnection, Q_ARG(bool, (bool)true));
                }
            }
        }

        void updateSlidePosition(double slidePosition){
            QMetaObject::invokeMethod(slideSlider, "setValue", Qt::QueuedConnection, Q_ARG(int, (int)(slidePosition * 100)));
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

        QHBoxLayout* noteLayout;
        QVBoxLayout* noteLabelLayout;
        QLabel* noteLabel;
        QLabel* freqLabel;
        std::vector<QProgressBar*> centBarsNegative;
        std::vector<QProgressBar*> centBarsPositive;

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

    InputScanner* inputScanner;
    if(argc == 1){
        inputScanner = new SerialScanner(SERIAL_PORT, SERIAL_BUFFER_SIZE);
    } else {
        inputScanner = new FileScanner(std::string(argv[1]));
    }
    SerialController* serialController = new SerialController(mc, inputScanner, obs);

    std::thread serial_controller_thread(std::bind(&SerialController::start, serialController));
    serial_controller_thread.detach();

    return app.exec();
}