#include "pressure_component.h"


PressureComponent::PressureComponent(QWidget* parent) : QVBoxLayout(parent){
    pressureLabel = new QLabel("Pressure");
    pressureLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    pressureBar = new QProgressBar();
    pressureBar->setOrientation(Qt::Vertical);
    // pressureBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    pressureBar->setRange(0, 500);
    QObject::connect(pressureBar, &QProgressBar::valueChanged, [this](int value) {
        pressureLabel->setText(QString("Pressure: %1").arg(value));
        pressureBar->setFormat(QString::number(value));
    });

    this->addWidget(pressureBar, 0, Qt::AlignCenter);
    this->addWidget(pressureLabel, 0, Qt::AlignCenter);
}


void PressureComponent::updatePressure(double pressure){
    QMetaObject::invokeMethod(pressureBar, "setValue", Qt::QueuedConnection, Q_ARG(int, (int)pressure));
}