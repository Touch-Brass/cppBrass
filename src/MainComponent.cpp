/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

#include "model_params.h"

#include <cmath>

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.

    // specify the number of input and output channels that we want to open

    setAudioChannels(global::useMicInput ? 2 : 0, 2);
    startTimerHz(15);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    stopTimer();
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{

    if (sampleRate != 44100)
        std::cout << "sampleRate is not 44100 Hz!!" << std::endl;

    fs = sampleRate;
    ModelParams* params = new ModelParams();

    //// Tube ////
    params->T = 26.85;
    params->LnonExtended = global::LnonExtended;
    params->Lextended = global::Lextended;
    params->L = global::LnonExtended;
    //    parameters.set ("L", 3.653);

    // Geometry
    //    parameters.set ("mp", 0.015 * 0.015 * M_PI);                   // mouthpiece cross-sectional area
    //    parameters.set ("mpL", 0.01);                   // mouthpiece length (length ratio)
    //    parameters.set ("m2tL", 0.01);                  // mouthpiece to tube (length ratio)
    //    parameters.set ("tubeS", 0.01 * 0.01 * M_PI);                 // tube cross-sectional area

    // Geometric information including formula from bell taken from T. Smyth "Trombone synthesis by model and measurement"
    geometry = {
        {0.708, 0.177, 0.711, 0.241, 0.254, 0.502},      // lengths
        {0.0069, 0.0072, 0.0069, 0.0071, 0.0075, 0.0107} // radii
    };

    params->flare = 0.7;
    params->x0 = 0.0174;
    params->b = 0.0063;
    params->bellL = 0.21;

    //// Lip ////
    double f0 = 300;
    double H0 = 2.9e-4;
    params->f0 = f0;            // fundamental freq lips
    params->Mr = 5.37e-5;            // mass lips
    params->omega0 = 2.0 * M_PI * f0; // angular freq

    params->sigmaR = 5;    // damping
    params->H0 = H0;       // equilibrium
    params->barrier = -H0; // equilibrium

    params->w = 1e-2;     // lip width
    params->Sr = 1.46e-5; // lip area

    params->Kcol = 10000;
    params->alphaCol = 3;

    //// Input ////
    params->Pm = (global::exciteFromStart ? 300 : 0) * global::pressureMultiplier;
    //    LVal = (*parameters.getVarPointer ("Lextended"));
    trombone = std::make_unique<Trombone>(params, 1.0 / fs, geometry);
    // addAndMakeVisible(trombone.get()); THIS MIGHT BE JUCE CODE

    pressureVal = 0;
    LVal = params->LnonExtended; // start by contracting
    lipFreqVal = 2.4 * trombone->getTubeC() / (trombone->getTubeRho() * LVal);

    trombone->setExtVals(pressureVal, lipFreqVal, LVal, true);

    lowPass = std::make_unique<LowPass>(std::vector<double>{0.0001343, 0.0005374, 0.0008060, 0.0005374, 0.0001343},
                                        std::vector<double>{1, -3.3964, 4.3648, -2.5119, 0.5456});
    if (~global::useMicInput)
    {
        pressureSlider.setRange(0, 6000);
        pressureSlider.setValue(300 * global::pressureMultiplier);
        // addAndMakeVisible(pressureSlider); THIS MIGHT BE JUCE CODE
        pressureSlider.addListener(this);
        pressureSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    }
    // setSize(800, 600); THIS MIGHT BE JUCE CODE
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)

    float *const channelData1 = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float *const channelData2 = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

    float output = 0.0;

    if (global::useMicInput)
    {
        const float *input = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
        double avg = 0;
        for (int i = 0; i < bufferToFill.numSamples; ++i)
            avg += input[i] * input[i];
        avg /= bufferToFill.numSamples;
        avg = sqrt(avg);

        pressureVal = 10000 * avg;
        trombone->setExtVals(10000 * avg, lipFreqVal, LVal);
    }

    for (int i = 0; i < bufferToFill.numSamples; ++i)
    {
        trombone->calculate();
        output = trombone->getOutput() * 0.001 * global::oOPressureMultiplier;
        output = lowPass->filter(output);

        //        if (!done && global::saveToFiles && t >= global::startSample)
        //        {
        //            trombone->saveToFiles();
        //        }
        ++t;

        trombone->updateStates();
        channelData1[i] = global::outputClamp(output);
        channelData2[i] = global::outputClamp(output);
    }
    if (global::saveToFiles && t >= global::stopSample && !done)
    {
        done = true;
        trombone->closeFiles();
        std::cout << "done" << std::endl;
    }
    trombone->refreshLipModelInputParams();
    //    std::cout << output << std::endl;
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}