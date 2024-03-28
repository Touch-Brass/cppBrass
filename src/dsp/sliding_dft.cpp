#include "sliding_dft.h"


// Explicit instantiations
template class SlidingDFT<double, 1000>;

template <class NumberFormat, size_t DFT_Length>
SlidingDFT<NumberFormat, DFT_Length>::SlidingDFT() {
    const std::complex<NumberFormat> j(0.0, 1.0);
    const NumberFormat N = DFT_Length;

    // Compute the twiddle factors, and zero the x and S arrays
    for (size_t k = 0; k < DFT_Length; k++) {
        NumberFormat factor = (NumberFormat)(2.0 * M_PI) * k / N;
        this->twiddle[k] = std::exp(j * factor);
        this->S[k] = 0;
        this->x[k] = 0;
    }
}


template <class NumberFormat, size_t DFT_Length>
bool SlidingDFT<NumberFormat, DFT_Length>::is_data_valid(){
    return this->data_valid;
}


template <class NumberFormat, size_t DFT_Length>
bool SlidingDFT<NumberFormat, DFT_Length>::update(NumberFormat new_x){
    // Update the storage of the time domain values
    const NumberFormat old_x = this->x[this->x_index];
    this->x[this->x_index] = new_x;

    // Update the DFT
    const NumberFormat r = this->damping_factor;
    const NumberFormat r_to_N = pow(r, (NumberFormat)DFT_Length);
    for (size_t k = 0; k < DFT_Length; k++) {
        this->S[k] = this->twiddle[k] * (r * this->S[k] - r_to_N * old_x + new_x);
    }

    // Apply the Hanning window
    this->dft[0] = (NumberFormat)0.5*this->S[0] - (NumberFormat)0.25*(this->S[DFT_Length - 1] + this->S[1]);
    for (size_t k = 1; k < (DFT_Length - 1); k++) {
        this->dft[k] = (NumberFormat)0.5*this->S[k] - (NumberFormat)0.25*(this->S[k - 1] + this->S[k + 1]);
    }
    this->dft[DFT_Length - 1] = (NumberFormat)0.5*this->S[DFT_Length - 1] - (NumberFormat)0.25*(this->S[DFT_Length - 2] + this->S[0]);

    // Increment the counter
    this->x_index++;
    if (this->x_index >= DFT_Length) {
        this->data_valid = true;
        this->x_index = 0;
    }

    // Done.
    return this->data_valid;
}