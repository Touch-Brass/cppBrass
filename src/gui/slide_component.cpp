#include "slide_component.h"

#include <QGraphicsOpacityEffect>

SlideComponent::SlideComponent(){
    this->setObjectName("component");
    
    slideLayout = new QGridLayout(this);

    slideLabel = new QLabel("Slide Position");
    slideLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slideLabel->setStyleSheet("font-size: 20px;");
    slideSlider = new QSlider(Qt::Horizontal);
    slideSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slideSlider->setRange(250, 350);
    QObject::connect(slideSlider, &QSlider::valueChanged, [this](int value) {
        slideLabel->setText(QString("Slide Position: %1").arg(value));
    });

    slideLayout->addWidget(slideLabel, 0, 0, 1, 7);
    slideLayout->addItem(new QSpacerItem(0, 20), 1, 0);
    slideLayout->addWidget(slideSlider, 3, 0, 1, 7);

    this->setStyleSheet(
        "QWidget#position-widget{"
            "background-color: #EEEEEE;"
            "border-radius: 5px;"
        "}");

    for(int i = 0; i < 7; i++){
        QWidget* widget = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(widget);
        widget->setLayout(layout);
        widget->setGraphicsEffect(new QGraphicsOpacityEffect());

        widget->setObjectName("position-widget");

        for(int j = POSITIONS[i].size() - 1; j >= 0; j--){
            QLabel* label = new QLabel(this);
            label->setText(POSITIONS[i].at(j));
            positionLabels[i].push_back(label);
            layout->addWidget(label);
        }

        positionLayouts.push_back(layout);
        positionWidgets.push_back(widget);
        slideLayout->addWidget(widget, 2, i);
    }
}


void SlideComponent::updateSlidePosition(double slidePosition){
    QMetaObject::invokeMethod(slideSlider, "setValue", Qt::QueuedConnection, Q_ARG(int, (int)(slidePosition * 100)));
    for(int i = 0; i < 7; i++){
        if(i == (int)((slidePosition - 2.5) * 7 - 0.01)) {
            QMetaObject::invokeMethod(positionWidgets.at(i)->graphicsEffect(), "setOpacity", Qt::QueuedConnection, Q_ARG(double, 1.));
            continue;
        }
        QMetaObject::invokeMethod(positionWidgets.at(i)->graphicsEffect(), "setOpacity", Qt::QueuedConnection, Q_ARG(double, 0.3));
    }
}