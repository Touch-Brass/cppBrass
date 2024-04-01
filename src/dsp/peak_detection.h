#include <vector>

#include "serial_controller.h"

std::pair<double, double> getFundamentalFrequency(SlidingDFT<double, SAMPLES_PER_FFT>* sdft);