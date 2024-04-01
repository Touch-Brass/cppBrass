#include "slide_component.h"


SlideComponent::SlideComponent(QWidget* parent) : QGridLayout(parent){
    slideLabel = new QLabel("Slide Position");
    slideLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slideSlider = new QSlider(Qt::Horizontal);
    slideSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slideSlider->setRange(250, 350);
    QObject::connect(slideSlider, &QSlider::valueChanged, [this](int value) {
        slideLabel->setText(QString("Slide Position: %1").arg(value));
    });

    this->addWidget(slideLabel, 0, 0);
    this->addWidget(slideSlider, 1, 0);
}


void SlideComponent::updateSlidePosition(double slidePosition){
    QMetaObject::invokeMethod(slideSlider, "setValue", Qt::QueuedConnection, Q_ARG(int, (int)(slidePosition * 100)));
}