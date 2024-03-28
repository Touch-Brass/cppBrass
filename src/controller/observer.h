#pragma once

struct Observer{

    void (&updatePressure)(double);
    void (&updateFrequency)(double);
    void (&updateSlidePosition)(double);

};