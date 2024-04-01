#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>


class NoteComponent : public QHBoxLayout{

    public:
        NoteComponent(QWidget* parent);

        void updateFrequency(double frequency);

    private:
        QVBoxLayout* noteLabelLayout;
        QLabel* noteLabel;
        QLabel* freqLabel;
        std::vector<QProgressBar*> centBarsNegative;
        std::vector<QProgressBar*> centBarsPositive;

};