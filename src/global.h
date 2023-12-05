/*
  ==============================================================================

    Global.h
    Created: 5 Sep 2020 1:13:49pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include <fstream>
#include <iostream>
#include <vector>

namespace global
{

    static double pressureMultiplier = 10.0;
    static double oOPressureMultiplier = 1.0 / pressureMultiplier;

    static bool setTubeTo1 = false;
    static bool connectedToLip = true;
    static bool fixedNonInterpolatedL = false;
    static bool exciteFromStart = true;
    static bool saveToFiles = false;
    static bool onlyWriteOutput = false;
    static int startSample = 0;
    static int stopSample = 10;
    static bool plotPressure = false;
    static double nonExtendedLipFreq = 520 * 0.5;

    static bool bowing = false; // experimental stuff

    static bool useMicInput = false;

    static double lambdaFact = 0.999;
    static double Nmaxdiff = 20.0;
    static bool useDispCorr = true;
    static bool correctV = false;

    static double LnonExtended = 2.593;
    static double Lextended = 3.653;

    std::vector<double> linspace(double start, double finish, int N);
    double linspace(double start, double finish, int N, int idx);

    inline double subplus(double val) { return (val + abs(val)) * 0.5; };

    inline int sgn(double val) { return (0 < val) - (val < 0); };

    double limit(double val, double min, double max);

    double outputClamp(double val);

}
