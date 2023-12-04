/*
  ==============================================================================

    LowPass.cpp
    Created: 7 Apr 2021 2:16:52pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "lowpass.h"

//==============================================================================
LowPass::LowPass(std::vector<double> bCoeffs, std::vector<double> aCoeffs)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    filterOrder = std::max(bCoeffs.size(), aCoeffs.size());

    // zero-pad
    while (bCoeffs.size() != aCoeffs.size())
    {
        if (bCoeffs.size() < aCoeffs.size())
            bCoeffs.push_back(0);
        else
            aCoeffs.push_back(0);
    }
    b.reserve(filterOrder);
    a.reserve(filterOrder);

    x.resize(filterOrder, 0);
    y.resize(filterOrder, 0);

    for (int i = 0; i < filterOrder; ++i)
    {
        b.push_back(bCoeffs[i]);
        a.push_back(aCoeffs[i]);
    }
}

float LowPass::filter(float input)
{
    if (!active)
        return input;
    x[0] = input;

    output = b[0] * x[0];
    for (int i = 1; i < filterOrder; ++i)
    {
        output += (b[i] * x[i] - a[i] * y[i]);
    }

    y[0] = output;
    // update states
    for (int i = filterOrder - 1; i > 0; --i)
    {
        x[i] = x[i - 1];
        y[i] = y[i - 1];
    }

    return output;
}
