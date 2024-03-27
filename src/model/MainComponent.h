#pragma once

#include "global.h"
#include "trombone.h"
#include "lowpass.h"

#include <memory>

#include <portaudio.h>

class MainComponent
{
public:
    MainComponent();
    ~MainComponent();

    void prepareToPlay(double sampleRate);
    void startPlaying();
    static int computeAndOutput(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
    double pressureVal, lipFreqVal, LVal;

private:
    PaStream* stream;
    std::unique_ptr<Trombone> trombone;
    double fs;
    long t = 0;
    bool done = false;
    std::vector<std::vector<double>> geometry;
    int controlHeight, controlY;

    std::unique_ptr<LowPass> lowPass;

    bool init = true;
    double pressureValSave = 0;
};
