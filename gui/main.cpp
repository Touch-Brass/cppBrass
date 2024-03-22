#include <cctype>
#include <chrono>
#include <iostream>
#include <cmath>
#include <string>
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

#include <fftw3.h>

#include "../src/MainComponent.h"

#define SAMPLES_PER_FFT 1000
#define SAMPLE_RATE 1.0f / 0.0003f

#define SERIAL_BUFFER_SIZE 1024

#define DIAL_PREFIX 'd'


void calculateDFT(double* input, std::vector<float> freqs, std::vector<float> mags, int size)
{
    fftw_complex output[SAMPLES_PER_FFT];
    // Create FFTW plan
    fftw_plan plan = fftw_plan_dft_r2c_1d(size, input, output, FFTW_ESTIMATE);

    // Execute the plan
    fftw_execute(plan);

    // Clean up
    fftw_destroy_plan(plan);


    // std::vector<float> freqs;
    // std::vector<float> magns;

    for (int i = 0; i < SAMPLES_PER_FFT; ++i)
    {
        double magnitude = std::sqrt(output[i][0] * output[i][0] + output[i][1] * output[i][1]);
        double frequency = i * SAMPLE_RATE / static_cast<double>(SAMPLES_PER_FFT); // Calculate frequency

        freqs.push_back(frequency);
        mags.push_back(magnitude);
    }
}

int getFundamentalFrequency(std::vector<float> freqs, std::vector<float> mags){
    
}

std::string port = "/dev/ttyUSB0";
int device = open(port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
char* serialBuffer = (char*)malloc(sizeof(char) * SERIAL_BUFFER_SIZE);
int bytes_read = 0;
int buffer_i = 0;

char readStream(){
    while(buffer_i > bytes_read - 1){
        buffer_i = 0;
        bytes_read = read(device, serialBuffer, sizeof(char) * SERIAL_BUFFER_SIZE);
        // std:: cout << "BYTES_READ " << bytes_read << std::endl;
    }
    return serialBuffer[buffer_i++];
}

void read_serial(QSlider* pressureSlider, QSlider* frequencySlider, QSlider* positionSlider){
    double* readings = (double*) malloc(sizeof(double) * SAMPLES_PER_FFT);

    int i = 0;

    while(true){
        bool isDialValue = false;
        std::string number = "";

        while(true){
            char next_char = readStream();

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
            std::cout << DIAL_PREFIX << number << std::endl;
            QMetaObject::invokeMethod(positionSlider, "setValue", Qt::QueuedConnection, Q_ARG(int, std::stod(number.c_str()) * 100 / 1023 + 250));
        } else {
            readings[i] = std::stod(number.c_str());
            std::cout << readings[i] << std::endl;
            i++;
        }
        
        // Perform FFT
        if(i == 1000){
            std::vector<float> freqs;
            std::vector<float> mags;
            calculateDFT(readings, freqs, mags, SAMPLES_PER_FFT);

            int ff = getFundamentalFrequency(freqs, mags);

            QMetaObject::invokeMethod(frequencySlider, "setValue", Qt::QueuedConnection, Q_ARG(int, 450));
            QMetaObject::invokeMethod(pressureSlider, "setValue", Qt::QueuedConnection, Q_ARG(int, 50));


            i = 0;
        }
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