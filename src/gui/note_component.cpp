#include "note_component.h"

#include <cmath>

#include "freq_to_note.h"

NoteComponent::NoteComponent(QWidget* parent) : QHBoxLayout(parent){
    for(int i = 0; i < 5; i++){
        QProgressBar* bar = new QProgressBar();
        bar->setOrientation(Qt::Vertical);
        bar->setFixedHeight(100);
        bar->setFixedWidth(10);
        bar->setValue(100);
        centBarsNegative.push_back(bar);
        this->addWidget(bar);
    }

    noteLabelLayout = new QVBoxLayout(parent);

    noteLabel = new QLabel("");
    noteLabel->setStyleSheet("font-size: 40px;");
    noteLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    noteLabel->setAlignment(Qt::AlignCenter);
    noteLabelLayout->addWidget(noteLabel);

    freqLabel = new QLabel("");
    freqLabel->setStyleSheet("font-size: 25px;");
    freqLabel->setAlignment(Qt::AlignCenter);
    freqLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    noteLabelLayout->addWidget(freqLabel);

    this->addLayout(noteLabelLayout);

    for(int i = 0; i < 5; i++){
        QProgressBar* bar = new QProgressBar();
        bar->setOrientation(Qt::Vertical);
        bar->setFixedHeight(100);
        bar->setFixedWidth(10);
        bar->setValue(100);
        centBarsPositive.push_back(bar);
        this->addWidget(bar);
    }
}


void NoteComponent::updateFrequency(double frequency){
    std::pair<std::string, int> note = freq_to_note(frequency);
    QMetaObject::invokeMethod(noteLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, QString("%1").arg(note.first.c_str())));
    QMetaObject::invokeMethod(freqLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, QString("%1 %2").arg(std::to_string((int) frequency).c_str(), "Hz")));
    
    int cents = std::round((float)note.second / 10.);
    for(int i = 0; i < 5; i++){
        QMetaObject::invokeMethod(centBarsNegative.at(i), "setVisible", Qt::QueuedConnection, Q_ARG(bool, (bool)false));
        QMetaObject::invokeMethod(centBarsPositive.at(i), "setVisible", Qt::QueuedConnection, Q_ARG(bool, (bool)false));
    }
    
    if(cents < 0){
        for(int i = 0; i > cents; i--){
            QMetaObject::invokeMethod(centBarsNegative.at(4 + i), "setVisible", Qt::QueuedConnection, Q_ARG(bool, (bool)true));
        }
    } else {
        for(int i = 0; i < cents; i++){
            QMetaObject::invokeMethod(centBarsPositive.at(i), "setVisible", Qt::QueuedConnection, Q_ARG(bool, (bool)true));
        }
    }
}