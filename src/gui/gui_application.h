#pragma once

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "pressure_component.h"
#include "note_component.h"
#include "slide_component.h"

class GuiApplication{

    public:
        GuiApplication();

        void updatePressure(double pressure);
        void updateFrequency(double frequency);
        void updateSlidePosition(double slidePosition);
    

    private:
        QWidget* window;
        QGridLayout* mainLayout;

        QLabel* titleLabel;

        PressureComponent* pressureComponent;
        NoteComponent* noteComponent;
        SlideComponent* slideComponent;

};