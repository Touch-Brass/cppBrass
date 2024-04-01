#pragma once

#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QWidget>

class PressureComponent : public QWidget{

    public:
        PressureComponent();

        void updatePressure(double pressure);
    
    private:
        QVBoxLayout* pressureLayout;
        QLabel* pressureLabel;
        QProgressBar* pressureBar;

};