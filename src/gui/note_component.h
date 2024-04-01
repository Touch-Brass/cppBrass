#pragma once

#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QWidget>


class NoteComponent : public QWidget{

    public:
        NoteComponent();

        void updateFrequency(double frequency);

    private:
        QHBoxLayout* noteLayout;

        QVBoxLayout* noteLabelLayout;
        QLabel* noteLabel;
        QLabel* freqLabel;
        std::vector<QFrame*> centBarsNegative;
        std::vector<QFrame*> centBarsPositive;

};