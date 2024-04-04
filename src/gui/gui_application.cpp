#include "gui_application.h"

GuiApplication::GuiApplication(){
    window = new QWidget();
    window->setFixedSize(1000, 550);
    mainLayout = new QGridLayout(window);

    titleLabel = new QLabel("Digital Trombone");
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    titleLabel->setStyleSheet("font-size: 25px;");

    window->setStyleSheet(
        "QWidget#component{"
            "padding: 10px;"
            "background-color: white;"
            "border: solid black 1px;"
            "border-radius: 5px;"
        "}");

    pressureComponent = new PressureComponent();
    noteComponent = new NoteComponent();
    slideComponent = new SlideComponent();

    mainLayout->addWidget(titleLabel, 0, 0, 1, 2);
    mainLayout->addItem(new QSpacerItem(0, 20), 1, 0);
    mainLayout->addWidget(pressureComponent, 2, 0, 2, 1);
    mainLayout->addWidget(noteComponent, 2, 1);
    mainLayout->addWidget(slideComponent, 3, 1);

    mainLayout->setColumnStretch(1, 0);
    mainLayout->setColumnStretch(2, 0);

    window->show();
}


void GuiApplication::updatePressure(double pressure){
    pressureComponent->updatePressure(pressure);
}


void GuiApplication::updateFrequency(double frequency){
    noteComponent->updateFrequency(frequency);
}


void GuiApplication::updateSlidePosition(double slidePosition){
    slideComponent->updateSlidePosition(slidePosition);
}