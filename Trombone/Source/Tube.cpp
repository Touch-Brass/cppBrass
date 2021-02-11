/*
  ==============================================================================

    Tube.cpp
    Created: 5 Sep 2020 1:11:57pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Tube.h"

//==============================================================================
Tube::Tube (NamedValueSet& parameters, double k, std::vector<std::vector<double>>& geometry) : k (k), L (*parameters.getVarPointer("L")), T (*parameters.getVarPointer("T")), geometry (geometry)
{
    calculateThermodynamicConstants();
    
    h = c * k;
    double LnonExtended = static_cast<double>(*parameters.getVarPointer("LnonExtended"));
    NnonExtended = floor (LnonExtended / h);
    Nextended = floor (static_cast<double>(*parameters.getVarPointer("Lextended")) / h);
    N = L / h;
    if (Global::fixedNonInterpolatedL)
    {
        L = floor(N) * h;
        N = L / h;
        alf = 0;
    }
    LtoGoTo = L;

    Nint = floor (N);
    NintPrev = Nint;
//    h = L / Nint;
    
    flare = *parameters.getVarPointer ("flare");
    x0 = *parameters.getVarPointer ("x0");
    b = *parameters.getVarPointer ("b");

    M = ceil(round((geometry[0][0] + geometry[0][1] * 0.5) * NnonExtended / LnonExtended) + (Nint+1-NnonExtended) * 0.5);
    calculateGeometry();
    Mw = Nint-M;

    calculateRadii();

    lambda = c * k / h;
    lambdaOverRhoC = lambda / (rho * c);
    
    // initialise state vectors
    uvVecs.resize (2);
    upVecs.resize (2);

    wvVecs.resize (2);
    wpVecs.resize (2);

//    M = ceil (N*0.5);
//    Mw = floor (N*0.5);
    maxM = M + ceil((Nextended - Nint) * 0.5); // check whether this is correct
    maxMw = Nextended - maxM;
    for (int i = 0; i < 2; ++i)
    {
        // need to change to proper sizes
        uvVecs[i] = std::vector<double> (maxM, 0);
        upVecs[i] = std::vector<double> (maxM+1, 0);
        wvVecs[i] = std::vector<double> (maxMw, 0);
        wpVecs[i] = std::vector<double> (maxMw+1, 0);

    }
    
    if (raisedCos)// || !Global::connectedToLip)
    {
        //        int start = N * 0.25 - 5;
        //        int end = N * 0.25 + 5;
        int start = 20;
        int end = 30;
        
        double scaling = 1000.0;
        for (int n = 0; n < 2; ++n)
        {
            for (int l = start; l < end; ++l)
            {
                upVecs[n][l] = scaling * (1.0 - cos (2.0 * double_Pi * (l-start) / static_cast<float>(end - start))) * 0.5;
            }
        }
    }
    
    uv.resize (2);
    up.resize (2);
    
    wv.resize (2);
    wp.resize (2);

    for (int i = 0; i < 2; ++i)
    {
        uv[i] = &uvVecs[i][0];
        up[i] = &upVecs[i][0];
        wv[i] = &wvVecs[i][0];
        wp[i] = &wpVecs[i][0];

    }
    uvMPh = 0;
    wvmh = 0;
    upMP1 = 0;
    wpm1 = 0;
    
    // Radiation
    R1 = rho * c;
    rL = sqrt(SBar[Nint]) / (2.0 * double_Pi);
    Lr = 0.613 * rho * rL;
    R2 = 0.505 * rho * c;
    Cr = 1.111 * rL / (rho * c * c);
    
    double zDiv = 2.0 * R1 * R2 * Cr + k * (R1 + R2);
    if (zDiv == 0)
    {
        z1 = 0;
        z2 = 0;
    } else {
        z1 = 2 * R2 * k / zDiv;
        z2 = (2 * R1 * R2 * Cr - k * (R1 + R2)) / zDiv;
    }
    
    z3 = k / (2.0 *Lr) + z1 / (2.0 * R2) + Cr * z1 / k;
    z4 = (z2 + 1.0) / (2.0 * R2) + (Cr * z2 - Cr) / k;
    
    oORadTerm = 1.0 / (1.0 + rho * c * lambda * z3);
    
    p1 = 0;
    v1 = 0;
    
    quadIp.resize (3, 0);
    customIp.resize (4, 0);
    
    statesSave.open ("statesSave.csv");

}

Tube::~Tube()
{
    closeFiles();

}

void Tube::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

//    if (init)
//    {
        g.setColour (Colours::gold);
        Path stringPathTop = drawGeometry (g, -1);
        Path stringPathBottom = drawGeometry (g, 1);
        g.strokePath (stringPathTop, PathStrokeType(2.0f));
        g.strokePath (stringPathBottom, PathStrokeType(2.0f));
        init = false;
//    }
    g.setColour (Colours::cyan);
    Path state = visualiseState (g, (Global::setTubeTo1 ? 10000 : 0.01) * Global::oOPressureMultiplier);
    g.strokePath (state, PathStrokeType (2.0f));

    
//    std::cout << "repainted" << std::endl;

}

Path Tube::drawGeometry (Graphics& g, int topOrBottom)
{
    double visualScaling = 10.0 * getHeight();
    Path stringPath;
    stringPath.startNewSubPath (0, topOrBottom * radii[0] * visualScaling + getHeight() * 0.5);
    int stateWidth = getWidth();
    auto spacing = stateWidth / static_cast<double>(Nint - 1);
    auto x = spacing;
    
    for (int y = 1; y < Nint; y++)
    {
        stringPath.lineTo(x, topOrBottom * radii[y] * visualScaling + getHeight() * 0.5);
        x += spacing;
    }
    return stringPath;
}

Path Tube::visualiseState (Graphics& g, double visualScaling)
{
    auto stringBounds = getHeight() / 2.0;
    Path stringPath;
    stringPath.startNewSubPath (0, -up[1][0] * visualScaling + stringBounds);
    int stateWidth = getWidth();
    auto spacing = stateWidth / static_cast<double>(Nint - 1);
    auto x = spacing;
    bool switchToW = false;
    
    for (int y = 1; y <= Nint + 1; y++)
    {
        float newY;
        if (y <= M)
        {
            newY = -up[1][y] * visualScaling + stringBounds; // Needs to be -p, because a positive p would visually go down
//            if (y == M-1)
//            {
//                std::cout << x;
//            }
            if (isnan(x) || isinf(abs(x) || isnan(up[1][y]) || isinf(abs(up[1][y]))))
            {
                std::cout << "Wait" << std::endl;
            };
            
        } else {
            if (!switchToW)
            {
                x -= spacing;
                x += alf * spacing;
                switchToW = true;
            }
            newY = -wp[1][y-M-1] * visualScaling + stringBounds; // Needs to be -p, because a positive p would visually go down
//            if (y == M)
//            {
//                std::cout << ", " << x << std::endl;;
//            }
            if (isnan(x) || isinf(abs(x) || isnan(wp[1][y-M]) || isinf(abs(wp[1][y-M]))))
            {
                std::cout << "Wait" << std::endl;
            };

        }
       
        
        if (isnan(newY))
            newY = 0;
        stringPath.lineTo (x, newY);
        //        g.drawEllipse(x, newY, 2, 2, 5);
        x += spacing;
    }
    return stringPath;
}

void Tube::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void Tube::calculateThermodynamicConstants()
{
    double deltaT = T - 26.85;
    c = 3.4723e2 * (1 + 0.00166 * deltaT);      // Speed of sound in air [m/s]
    rho = 1.1769 * (1 - 0.00335 * deltaT);      // Density of air [kg·m^{-3}]
//    eta = 1.846 * (1 + 0.0025 * deltaT);        // Shear viscosity [kg·s^{-1}·m^{-1}]
//    nu = 0.8410 * (1 - 0.0002 * deltaT);        // Root of Prandtl number [-]
//    gamma = 1.4017 * (1 - 0.00002 * deltaT);    // Ratio of specific heats [-]
    
}

void Tube::calculateVelocity()
{
    for (int l = 0; l < M; ++l)
        uv[0][l] = uv[1][l] - lambdaOverRhoC * (up[1][l+1] - up[1][l]);
    for (int l = 0; l < Mw; ++l)
        wv[0][l] = wv[1][l] - lambdaOverRhoC * (wp[1][l+1] - wp[1][l]);
    
    quadIp[0] = -(alf - 1) / (alf + 1);
    quadIp[1] = 1;
    quadIp[2] = (alf - 1) / (alf + 1);
    
    upMP1 = up[1][M] * quadIp[2]  + wp[1][0] * quadIp[1] + wp[1][1] * quadIp[0];
    wpm1 = up[1][M-1] * quadIp[0] + up[1][M] * quadIp[1]  + wp[1][0] * quadIp[2];

    uvNextMPh = uvMPh - lambda / (rho * c) * (upMP1 - up[1][M]);
    wvNextmh = wvmh - lambda / (rho * c) * (wp[1][0] - wpm1);
    

}

void Tube::calculatePressure()
{
    for (int l = 1; l < M; ++l) // calculate full range minus the boundaries
        up[0][l] = up[1][l] - rho * c * lambda * oOSBar[l] * (SHalf[l] * uv[0][l] - SHalf[l-1] * uv[0][l-1]);
    
    // right (inner) boundary of left system
    up[0][M] = up[1][M] - rho * c * lambda * oOSBar[M] * (SHalf[M] * uvNextMPh - SHalf[M-1] * uv[0][M-1]);
    
    for (int l = 1; l < Mw; ++l) // calculate full range minus the boundaries
        wp[0][l] = wp[1][l] - rho * c * lambda * oOSBar[l+M] * (SHalf[l+M] * wv[0][l] - SHalf[l-1+M] * wv[0][l-1]);

    // left (inner) boundary of right system
    wp[0][0] = wp[1][0] - rho * c * lambda * oOSBar[M] * (SHalf[M] * wv[0][0] - SHalf[M-1] * wvNextmh);
    
    // excitation
    up[0][0] = up[1][0] - rho * c * lambda * oOSBar[0] * (-2.0 * (Ub + Ur) + 2.0 * SHalf[0] * uv[0][0]);
//    std::cout << up[0][M-1] - wp[0][0] << std::endl;
}

void Tube::calculateRadiation()
{
    wp[0][Mw] = ((1.0 - rho * c * lambda * z3) * wp[1][Mw] - 2.0 * rho * c * lambda * (v1 + z4 * p1 - (SHalf[Nint-1] * wv[0][Mw-1]) * oOSBar[Nint])) * oORadTerm;

    v1Next = v1 + k / (2.0 * Lr) * (wp[0][Mw] + wp[1][Mw]);
    p1Next = z1 * 0.5 * (wp[0][Mw] + wp[1][Mw]) + z2 * p1;
}

void Tube::updateStates()
{
    uvTmp = uv[1];
    uv[1] = uv[0];
    uv[0] = uvTmp;
    
    wvTmp = wv[1];
    wv[1] = wv[0];
    wv[0] = wvTmp;
    
    upTmp = up[1];
    up[1] = up[0];
    up[0] = upTmp;
    
    wpTmp = wp[1];
    wp[1] = wp[0];
    wp[0] = wpTmp;
    
    uvMPh = uvNextMPh;
    wvmh = wvNextmh;
    
    p1 = p1Next;
    v1 = v1Next;
}

void Tube::calculateGeometry()
{
    S.resize (Nint+1, 0);
    SHalf.resize (Nint, 0);
    SBar.resize (Nint+1, 0);
    oOSBar.resize (Nint+1, 0);
    
    std::vector<double> lengthInN (geometry[0].size(), 0);
    double totLength = 0;
    int totLengthMinSlideInN = 0;

    for (int i = 0; i < geometry[0].size(); ++i)
    {
        totLength += geometry[0][i];
    }
    
    for (int i = 0; i < geometry[0].size(); ++i)
    {
        lengthInN[i] = round(NnonExtended * geometry[0][i] / totLength);
        if (i != 1)
            totLengthMinSlideInN += lengthInN[i];
    }
    
    lengthInN[1] = Nint + 1 - totLengthMinSlideInN;
    // indicate split of two connected schemes (including offset if N differs from NnonExtended
//    double mp = *parameters.getVarPointer ("mp");
//    double tubeS = *parameters.getVarPointer ("tubeS");
//
//    int mpL = Nint * double (*parameters.getVarPointer ("mpL"));
//    int m2tL = Nint * double (*parameters.getVarPointer ("m2tL"));
//    int bellL = Nint * double (*parameters.getVarPointer ("bellL"));
    double x = 0;

    if (Global::setTubeTo1)
    {
        for (int i = 0; i <= Nint; ++i)
        {
            S[i] = 1;
        }
    }
    else
    {
        int idx = 0;
        int curN;
        int lastNofPart = lengthInN[0];
        for (int i = 0; i <= Nint; ++i)
        {
            if (i >= lastNofPart)
            {
                ++idx;
                lastNofPart += lengthInN[idx];
            }
            if (idx == 4) // tuning slide
            {
                S[i] = pow(Global::linspace(geometry[1][idx], geometry[1][idx+1],
                                        lengthInN[idx], i - curN - 1), 2) * double_Pi;
            } else if (idx == 5)
            {
                x = geometry[0][5] - geometry[0][5] * (i - (Nint - lengthInN[5]) - 1) / (lengthInN[5] - 1);
//                S[Nint-(i - (Nint - lengthInN[5]))] = pow(b * pow(((i - (Nint - lengthInN[5])) / (2.0 * lengthInN[5]) + x0), -flare), 2) * double_Pi;
                S[i] = pow(b * pow(x + x0, -flare), 2) * double_Pi;
            } else {
                S[i] = pow(geometry[1][idx], 2) * double_Pi;
                curN = i;
            }
        }
//        for (int i = 0; i < Nint; ++i)
//        {
//            if (i < mpL)
//                S[i] = mp;
//            else if (i >= mpL && i < mpL + m2tL)
//                S[i] = Global::linspace (mp, tubeS, m2tL, i-mpL);
//            else if (i >= mpL + m2tL && i < Nint - bellL)
//                S[i] = tubeS;
//            else
//                S[Nint-(i - (Nint - bellL))] = pow(b * pow(((i - (Nint - bellL)) / (2.0 * bellL) + x0), -flare), 2) * double_Pi;
//        }
    }
    
    for (int i = 0; i < Nint; ++i)
        SHalf[i] = (S[i] + S[i+1]) * 0.5;
    
    SBar[0] = S[0];
    
    for (int i = 0; i < Nint - 1; ++i)
        SBar[i+1] = (SHalf[i] + SHalf[i+1]) * 0.5;
    
    SBar[Nint] = S[Nint];
    for (int i = 0; i <= Nint; ++i)
        oOSBar[i] = 1.0 / SBar[i];
}

void Tube::calculateRadii()
{
    radii.resize (Nint+1, 0);
    for (int i = 0; i < Nint+1; ++i)
        radii[i] = sqrt (S[i]) / double_Pi;
    
}
double Tube::getKinEnergy()
{
    double kinEnergy = 0;
    for (int i = 0; i <= M; ++i)
    {
        kinEnergy += 1.0 / (2.0 * rho * c * c) * h * (SBar[i] * up[1][i] * up[1][i] * (i == 0 || i == M ? 0.5 : 1));
    }
    for (int i = 0; i <= Mw; ++i)
    {
        kinEnergy += 1.0 / (2.0 * rho * c * c) * h * (SBar[i + M] * wp[1][i] * wp[1][i] * (i == 0 || i == Mw ? 0.5 : 1));
    }
    if (kinEnergy1 <= 0)
        kinEnergy1 = kinEnergy;
    return kinEnergy;

}

double Tube::getPotEnergy()
{
    double potEnergy = 0;
    for (int i = 0; i < M; ++i)
        potEnergy += rho * 0.5 * h * (SHalf[i] * uv[0][i] * uv[1][i]);
    
    for (int i = 0; i < Mw; ++i)
        potEnergy += rho * 0.5 * h * (SHalf[i+M] * wv[0][i] * wv[1][i]);
    
    if (potEnergy1 < 0)
        potEnergy1 = potEnergy;
    
    return potEnergy;
}

double Tube::getRadEnergy()
{
    double radEnergy = SBar[Nint] / 2.0 * (Lr * v1 * v1 + Cr * p1 * p1);
    
    if (radEnergy1 < 0)
        radEnergy1 = radEnergy;
    
    return radEnergy;
}

double Tube::getRadDampEnergy()
{
    double pBar = 0.5 * (wp[0][Mw] + wp[1][Mw]);
    double muTPv2 = (pBar - 0.5 * (p1Next + p1)) / R1;
    
    double qRad = SBar[Nint] * (R1 * muTPv2 * muTPv2 + R2 * (0.5 * ((p1Next + p1) / R2) * (0.5 * ((p1Next + p1) / R2))));
    double qHRad = k * qRad + qHRadPrev;

    double qHRadPrevTmp = qHRadPrev;
    qHRadPrev = qHRad;
    return qHRadPrevTmp;
                                                            

}

void Tube::updateL()
{
    Lprev = L;
    NintPrev = Nint;
    
    L = (1-LfilterCoeff) * LtoGoTo + LfilterCoeff * Lprev;
    N = L / h;
    Nint = floor(N);
    alf = N - Nint;
    if (Nint != NintPrev)
    {
        addRemovePoint();
    }
}

void Tube::addRemovePoint()
{
    calculateGeometry();
    calculateRadii();
    if (Nint > NintPrev) // add point
    {
        createCustomIp();
        if (Nint % 2 == 1)
        {
            up[1][M + 1] = customIp[0] * up[1][M-1]
            + customIp[1] * up[1][M]
            + customIp[2] * wp[1][0]
            + customIp[3] * wp[1][1];
            uv[1][M] = uvNextMPh;
            uvMPh = customIp[0] * uv[1][M-2]
                + customIp[1] * uv[1][M-1]
                + customIp[2] * wv[1][0]
                + customIp[3] * wv[1][1];
            ++M;
        }
        else
        {
            // save w0 beforehand, otherwise things will be overwritten
            double w0 = customIp[3] * up[1][M-1]
                + customIp[2] * up[1][M]
                + customIp[1] * wp[1][0]
                + customIp[0] * wp[1][1];
            // move w vector one up (can be optimised)
            for (int l = Mw; l >= 0; --l)
            {
                wp[1][l+1] = wp[1][l];
                if (l != Mw)
                    wv[1][l+1] = wv[1][l];
                
            }
            wp[1][0] = w0;
            wv[1][0] = wvNextmh; // or wvmh, doesn't matter as they're the same at this point
            wvmh = customIp[3] * uv[1][M-2]
                + customIp[2] * uv[1][M-1]
                + customIp[1] * wv[1][0]
                + customIp[0] * wv[1][1];
            
            ++Mw;
        }
        statesSave << up[1][M-1] << "," << up[1][M] << "," << wp[1][0] << "," << wp[1][1] << "," << uv[1][M-2] << "," << uv[1][M-1] << "," << wv[1][0] << "," << wv[1][1] << "," << uvMPh << "," << wvmh << ";\n";
        
    } else {
        if (Nint % 2 == 0)
        {
            uvMPh = uv[1][M-1];
            
            up[1][M] = 0;
            uv[1][M-1] = 0;
            up[0][M] = 0;
            uv[0][M-1] = 0;
            --M;
        }
        else
        {
            wvmh = wv[1][0];
            // move w vector one down (can be optimised)
            for (int l = 0; l <= Mw; ++l)
            {
                wp[1][l] = wp[1][l+1];
                if (l != Mw)
                    wv[1][l] = wv[1][l+1];
                
            }
            wp[1][Mw] = 0;
            wv[1][Mw-1] = 0;
            wp[0][Mw] = 0;
            wv[0][Mw-1] = 0;
            --Mw;
        }
        statesSave << up[1][M-1] << "," << up[1][M] << "," << wp[1][0] << "," << wp[1][1] << "," << uv[1][M-2] << "," << uv[1][M-1] << "," << wv[1][0] << "," << wv[1][1] << "," << uvMPh << "," << wvmh << ";\n";

    }
}


void Tube::createCustomIp()
{
//    switch (dyIntType)
//    {
//        case dLinear:
//        {
//            customIp[0] = 0;
//            customIp[1] = alfTick / (alfTick + 1.0);
//            customIp[2] = 1.0 / (alfTick + 1.0);
//            customIp[3] = 0;
//            break;
//        }
//        case dQuadratic:
//        case dCubic:
//        case dAltCubic:
//        case dQuartic:
//            //        case dSinc:
//        {
            float alfTick = ((L-Mw * h) - ((M + 1) * h)) / h;
            customIp[0] = -alfTick * (alfTick + 1.0) / ((alfTick + 2.0) * (alfTick + 3.0));
            customIp[1] = 2.0 * alfTick / (alfTick + 2.0);
            customIp[2] = 2.0 / (alfTick + 2.0);
            customIp[3] = -2.0 * alfTick / ((alfTick + 3.0) * (alfTick + 2.0));
            //            if (alf-alfTick != 0)
            //                std::cout << "alf: " << alf << " alfTick " << alfTick << std::endl;
            //            customIp1[3] = alf * (alf - 1) * (alf - 2) / -6.0;
            //            customIp1[2] = (alf - 1) * (alf + 1) * (alf - 2) / 2.0;
            //            customIp1[1] = alf * (alf + 1) * (alf - 2) / -2.0;
            //            customIp1[0] = alf * (alf + 1) * (alf - 1) / 6.0;
//            break;
//        }
//
//        case dSinc:
//        {
//            int custSincWidth = 2;
//            double custBMax = M_PI;
//            std::vector <double> xPosCustIp (4, 0);
//
//            for (int i = 0; i < custSincWidth; ++i)
//                xPosCustIp[i] = i-custSincWidth;
//            for (int i = custSincWidth; i < custSincWidth * 2; ++i)
//                xPosCustIp[i] = i - custSincWidth + alf;
//
//            for (int i = 0; i < custSincWidth * 2; ++i)
//                customIp[i] = sin(custBMax * xPosCustIp[i]) / (xPosCustIp[i] * custBMax);
//
//            if (alf == 0)
//                customIp[custSincWidth] = 1;
//            break;
//        }
//    }
}

void Tube::closeFiles()
{
    statesSave.close();
}
