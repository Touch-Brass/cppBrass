#include "pressure_component.h"


PressureComponent::PressureComponent(){
    this->setObjectName("component");

    pressureLayout = new QVBoxLayout(this);

    pressureLabel = new QLabel("Pressure");
    // pressureLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    pressureLabel->setStyleSheet("font-size: 20px;");
    pressureBar = new QProgressBar();
    pressureBar->setOrientation(Qt::Vertical);
    // pressureBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    pressureBar->setRange(0, 500);
    // QObject::connect(pressureBar, &QProgressBar::valueChanged, [this](int value) {
    //     pressureLabel->setText(QString("Pressure: %1").arg(value));
    // });

    pressureLayout->addWidget(pressureLabel, 0, Qt::AlignHCenter);
    pressureLayout->addStretch(1);
    pressureLayout->addWidget(pressureBar, 10, Qt::AlignHCenter);
}


void PressureComponent::updatePressure(double pressure){
    QMetaObject::invokeMethod(pressureBar, "setValue", Qt::QueuedConnection, Q_ARG(int, (int)pressure));
}