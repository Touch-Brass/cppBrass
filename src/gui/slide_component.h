#pragma once

#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QWidget>

class SlideComponent : public QWidget{

    public:
        SlideComponent();

        void updateSlidePosition(double slidePosition);


    private:
        QGridLayout* slideLayout;
        
        QLabel* slideLabel;
        QSlider* slideSlider;

};