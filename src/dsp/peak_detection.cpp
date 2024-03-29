#include "peak_detection.h"

#include "sliding_dft.h"

std::pair<double, double> getFundamentalFrequency(SlidingDFT<double, SAMPLES_PER_FFT> sdft){
    for(int i = 10; i < SAMPLES_PER_FFT / 2; i++){
        if(std::abs(sdft.dft[i]) > 20000){
            return std::pair<int, int>((double)i * SAMPLE_RATE / SAMPLES_PER_FFT, std::abs(sdft.dft[i]));
        }
    }
    return std::pair<int, int>(100, 0);
}