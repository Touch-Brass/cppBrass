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
    static bool exciteFromStart = false;
    static bool saveToFiles = false;
    static bool onlyWriteOutput = false;
    static int startSample = 0;
    static int stopSample = 10;
    static bool plotPressure = false;
    static double nonExtendedLipFreq = 520 * 0.5;

    static bool bowing = false; // experimental stuff

    static bool useMicInput = true;

    static double lambdaFact = 0.999;
    static double Nmaxdiff = 20.0;
    static bool useDispCorr = true;
    static bool correctV = false;

    static double LnonExtended = 2.593;
    static double Lextended = 3.653;

    static std::vector<double> linspace(double start, double finish, int N);
    static double linspace(double start, double finish, int N, int idx);

    static inline double subplus(double val);

    static inline int sgn(double val);

    static double limit(double val, double min, double max);

    static double outputClamp(double val);

}
