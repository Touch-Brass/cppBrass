#include "MainComponent.h"

#include <portaudio.h>
#include <cmath>

int main() {

    MainComponent* mc = new MainComponent();
    mc->prepareToPlay(44100);
    mc->startPlaying();

    return 0;
}

