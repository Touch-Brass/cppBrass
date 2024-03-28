#pragma once

#include <functional>

struct Observer{

    std::function<void(double)> updatePressure;
    std::function<void(double)> updateFrequency;
    std::function<void(double)> updateSlidePosition;

};