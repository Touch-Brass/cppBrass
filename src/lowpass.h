/*
  ==============================================================================

    LowPass.h
    Created: 7 Apr 2021 2:16:52pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once


#include <vector>

class LowPass
{
public:
    LowPass (std::vector<double> bCoeffs, std::vector<double> aCoeffs);
    ~LowPass();

    float filter (float input);
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
