#pragma once

#include <vector>

class LowPass
{
public:
    LowPass(std::vector<double> bCoeffs, std::vector<double> aCoeffs);
    ~LowPass();

    float filter(float input);
    void toggleOnOff() { active = !active; };

    bool isOn() { return active; };

private:
    std::vector<double> b;
    std::vector<double> a;

    std::vector<double> x;
    std::vector<double> y;
    float output;

    int filterOrder;
    bool active = true;
};
