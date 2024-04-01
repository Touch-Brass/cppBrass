#pragma once

#include <QGridLayout>
#include <QLabel>
#include <QSlider>

class SlideComponent : public QGridLayout{

    public:
        SlideComponent(QWidget* parent);

        void updateSlidePosition(double slidePosition);


    private:
        QGridLayout* slideLayout;
        QLabel* slideLabel;
        QSlider* slideSlider;

};