#include "MainComponent.h"

#include <portaudio.h>
#include <cmath>
#include <fftw3.h>

std::vector<float> findLocalMaximas(std::vector<float> &arr);

void calculateDFT(double *input, fftw_complex *output, int size);

const int SAMPLE_RATE = 44100;
const int FRAMES_PER_BUFFER = 44100;

int counter = 0;

// Callback function to process audio input
int paCallback(const void *inputBuffer, void *outputBuffer,
               unsigned long framesPerBuffer,
               const PaStreamCallbackTimeInfo *timeInfo,
               PaStreamCallbackFlags statusFlags,
               void *userData)
{
    // Cast inputBuffer to const float* since PortAudio uses float samples
    double inputs_double[framesPerBuffer];
    const float* inputs = static_cast<const float*>(inputBuffer);

    // std::cout << "Captured " << framesPerBuffer << " samples." << std::endl;

    for (int i = 0; i < framesPerBuffer; i++) {
        inputs_double[i] = static_cast<double>(inputs[i]);
    }

    fftw_complex output[framesPerBuffer];

    calculateDFT(inputs_double, output, framesPerBuffer);

    // Save the frequency spectrum to a CSV file
    std::ofstream outFile(std::string("files/frequency_spectrum_") + std::to_string(counter++) + ".csv");
    outFile << "Frequency (Hz),Magnitude" << std::endl;

    std::vector<float> freqs;
    std::vector<float> magns;

    for (int i = 0; i < framesPerBuffer; ++i)
    {
        double magnitude = sqrt(output[i][0] * output[i][0] + output[i][1] * output[i][1]);
        double frequency = i * SAMPLE_RATE / static_cast<double>(framesPerBuffer); // Calculate frequency

        freqs.push_back(frequency);
        magns.push_back(magnitude);

        outFile << frequency << "," << magnitude << std::endl;
    }

    outFile.close();

    std::vector<float> peaks = findLocalMaximas(magns);

    for (size_t i = 0; i < peaks.size(); ++i)
    {
        std::cout << freqs[peaks[i]] << " ";
    }

    if(peaks.size() > 0) {
        std::cout << std::endl;
    }

    return paContinue;
}

int openPAStream()
{
    PaError err;

    // Initialize PortAudio
    err = Pa_Initialize();
    if (err != paNoError)
    {
        std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    // Set up parameters
    PaStream *stream;
    std::vector<double> audioBuffer;

    // Open stream
    err = Pa_OpenDefaultStream(&stream,
                               1,         // 1 input channel
                               0,         // No output channels
                               paFloat32, // 32-bit floating point input
                               SAMPLE_RATE,
                               FRAMES_PER_BUFFER,
                               paCallback,
                               &audioBuffer);

    if (err != paNoError)
    {
        std::cerr << "PortAudio stream opening failed: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return 1;
    }

    // Start the stream
    err = Pa_StartStream(stream);
    if (err != paNoError)
    {
        std::cerr << "PortAudio stream starting failed: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    std::cout << "Capturing audio... Press Enter to stop." << std::endl;
    std::cin.get(); // Wait for user input

    // Stop and close the stream
    err = Pa_StopStream(stream);
    if (err != paNoError)
    {
        std::cerr << "PortAudio stream stopping failed: " << Pa_GetErrorText(err) << std::endl;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError)
    {
        std::cerr << "PortAudio stream closing failed: " << Pa_GetErrorText(err) << std::endl;
    }

    // Terminate PortAudio
    Pa_Terminate();

    // Now, 'audioBuffer' contains the captured audio samples as double values
    // You can convert it to a double[] array if needed.

    return 0;
}

void calculateDFT(double *input, fftw_complex *output, int size)
{
    // Create FFTW plan
    fftw_plan plan = fftw_plan_dft_r2c_1d(size, input, output, FFTW_ESTIMATE);

    // Execute the plan
    fftw_execute(plan);

    // Clean up
    fftw_destroy_plan(plan);
}

std::vector<float> findLocalMaximas(std::vector<float> &arr)
{
    std::vector<float> maximas;

    if (arr.size() < 3)
    {
        // Array must have at least 3 elements to have a local maximum
        return maximas;
    }

    for (int i = 1; i < arr.size() - 1; ++i)
    {
        if (arr[i] > arr[i - 1] && arr[i] > arr[i + 1] && arr[i] > 2 && !std::isnan(arr[i]))
        {
            // Element at index i is a local maximum
            maximas.push_back(i);
        }
    }

    return maximas;
}

// int main() {

//     MainComponent* mc = new MainComponent();
//     mc->prepareToPlay(44100 / 2);
//     mc->startPlaying();

//     return 0;
// }

int main()
{
    openPAStream();
    return 0;
}