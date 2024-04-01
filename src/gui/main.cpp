#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <stdio.h>

#include "gui_application.h"

#include <QApplication>

#include "MainComponent.h"
#include "serial_controller.h"
#include "freq_to_note.h"
#include "input_scanner.h"
#include "serial_scanner.h"
#include "file_scanner.h"

#define SERIAL_PORT "/dev/ttyUSB0"
#define SERIAL_BUFFER_SIZE 1024


using namespace std::placeholders;


int main(int argc, char **argv)
{
    MainComponent* mc = new MainComponent();
    mc->prepareToPlay(44100);
    mc->startPlaying();

    QApplication app(argc, argv);
    GuiApplication* gui = new GuiApplication();

    Observer* obs = new Observer {
        std::bind(&GuiApplication::updatePressure, gui, _1),
        std::bind(&GuiApplication::updateFrequency, gui, _1),
        std::bind(&GuiApplication::updateSlidePosition, gui, _1)
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