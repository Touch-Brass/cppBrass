#pragma once

#include <vector>

#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>


const std::vector<const char*> POSITIONS[7] = {
    { "A#2", "F3", "A#3", "D4", "F4", "A#4", "C5", "D5", "F5" },
    { "A2", "E3", "A3", "C#4", "E4", "G4", "A4", "B4", "C#5", "D#5", "E5" },
    { "G#2", "D#3", "G#3", "C4", "D#4", "F#4", "G#4" },
    { "G2", "D3", "G3", "B3", "D4", "F4" },
    { "F#2", "C#3", "F#3" },
    { "F2", "C3", "F3" },
    { "E2", "B2" }
};


class SlideComponent : public QWidget{

    public:
        SlideComponent();

        void updateSlidePosition(double slidePosition);


    private:
        QGridLayout* slideLayout;
        
        QLabel* slideLabel;
        QSlider* slideSlider;

        std::vector<QWidget*> positionWidgets;
        std::vector<QVBoxLayout*> positionLayouts;
        std::vector<QLabel*> positionLabels[7];
};