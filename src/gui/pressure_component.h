#pragma once

#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

class PressureComponent : public QVBoxLayout{

    public:
        PressureComponent(QWidget* parent);

        void updatePressure(double pressure);
    
    private:
        QLabel* pressureLabel;
        QProgressBar* pressureBar;

};