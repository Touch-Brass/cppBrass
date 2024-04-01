#include "note_component.h"

#include <cmath>

#include "freq_to_note.h"

const char* BAR_COLORS[5] = {
    "#0000FF",
    "#0000FF",
    "#FFFF00",
    "#FFFF00",
    "#FF0000"
};

NoteComponent::NoteComponent(){
    this->setObjectName("component");

    noteLayout = new QHBoxLayout(this);
    noteLayout->addStretch(1);

    for(int i = 0; i < 5; i++){
        QFrame* frame = new QFrame(this);
        frame->setFrameShape(QFrame::Box);
        frame->setFixedWidth(10);
        frame->setFixedHeight(100);
        frame->setGraphicsEffect(new QGraphicsOpacityEffect(this));
        frame->setStyleSheet("background-color: " + QString(BAR_COLORS[4 - i]) + "; border: none; border-radius: 5px;");
        centBarsNegative.push_back(frame);
        noteLayout->addWidget(frame, 0);
    }

    noteLayout->addStretch(1);

    noteLabelLayout = new QVBoxLayout(this);

    noteLabel = new QLabel("");
    noteLabel->setStyleSheet("font-size: 40px;");
    noteLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    noteLabel->setAlignment(Qt::AlignHCenter);
    noteLabelLayout->addWidget(noteLabel);

    freqLabel = new QLabel("");
    freqLabel->setStyleSheet("font-size: 25px;");
    freqLabel->setAlignment(Qt::AlignHCenter);
    freqLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    noteLabelLayout->addWidget(freqLabel);

    noteLayout->addLayout(noteLabelLayout, 0);

    noteLayout->addStretch(1);

    for(int i = 0; i < 5; i++){
        QFrame* frame = new QFrame(this);
        frame->setFrameShape(QFrame::Box);
        frame->setFixedWidth(10);
        frame->setFixedHeight(100);
        frame->setGraphicsEffect(new QGraphicsOpacityEffect(this));
        frame->setStyleSheet("background-color: " + QString(BAR_COLORS[i]) + "; border: none; border-radius: 5px;");
        centBarsPositive.push_back(frame);
        noteLayout->addWidget(frame, 0);
    }

    noteLayout->addStretch(1);
}

void NoteComponent::updateFrequency(double frequency){
    std::pair<std::string, int> note = freq_to_note(frequency);
    QMetaObject::invokeMethod(noteLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, QString("%1").arg(note.first.c_str())));
    QMetaObject::invokeMethod(freqLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, QString("%1 %2").arg(std::to_string((int) frequency).c_str(), "Hz")));
    
    int cents = std::round((float)note.second / 10.);
    for(int i = 0; i < 5; i++){
        QMetaObject::invokeMethod(centBarsNegative.at(i)->graphicsEffect(), "setOpacity", Qt::QueuedConnection, Q_ARG(double, 0.));
        QMetaObject::invokeMethod(centBarsPositive.at(i)->graphicsEffect(), "setOpacity", Qt::QueuedConnection, Q_ARG(double, 0.));
        // QMetaObject::invokeMethod(centBarsNegative.at(i), "setVisible", Qt::QueuedConnection, Q_ARG(bool, (bool)false));
        // QMetaObject::invokeMethod(centBarsPositive.at(i), "setVisible", Qt::QueuedConnection, Q_ARG(bool, (bool)false));
    }
    
    if(cents < 0){
        for(int i = 0; i > cents; i--){
            QMetaObject::invokeMethod(centBarsNegative.at(4 + i)->graphicsEffect(), "setOpacity", Qt::QueuedConnection, Q_ARG(double, 1.));
            // QMetaObject::invokeMethod(centBarsNegative.at(4 + i), "setVisible", Qt::QueuedConnection, Q_ARG(bool, (bool)true));
        }
    } else {
        for(int i = 0; i < cents; i++){
            QMetaObject::invokeMethod(centBarsPositive.at(i)->graphicsEffect(), "setOpacity", Qt::QueuedConnection, Q_ARG(double, 1.));
            // QMetaObject::invokeMethod(centBarsPositive.at(i), "setVisible", Qt::QueuedConnection, Q_ARG(bool, (bool)true));
        }
    }
}