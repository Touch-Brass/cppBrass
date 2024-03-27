/*
  ==============================================================================

    Trombone.cpp
    Created: 5 Sep 2020 1:12:46pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "trombone.h"

#include <memory>
#include <cmath>

//==============================================================================
Trombone::Trombone(ModelParams* params, double k, std::vector<std::vector<double>> &geometry) : k(k),
                                                                                                Pm(params->Pm)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    tube = std::make_unique<Tube>(params, k, geometry);
    // addAndMakeVisible(tube.get()); THIS MIGHT BE JUCE CODE
    lipModel = std::make_unique<LipModel>(params, k);
    lipModel->setTubeParameters(tube->getH(),
                                tube->getRho(),
                                tube->getC(),
                                tube->getSBar(0),
                                tube->getSHalf(0));
    // addAndMakeVisible(lipModel.get()); THIS MIGHT BE JUCE CODE

    massState.open("massState.csv");
    pState.open("pState.csv");
    vState.open("vState.csv");
    alfSave.open("alfSave.csv");
    MSave.open("MSave.csv");
    MwSave.open("MwSave.csv");
    energySave.open("energySave.csv");
    scaledTotEnergySave.open("scaledTotEnergySave.csv");

    maxMSave.open("maxM.csv");
    maxMwSave.open("maxMw.csv");

    Ssave.open("SSave.csv");
    output.open("output.csv");

    maxMSave << tube->getMaxM();
    maxMwSave << tube->getMaxMw();

    maxMSave.close();
    maxMwSave.close();
}

Trombone::~Trombone()
{
    closeFiles();
}

void Trombone::calculate()
{
    if (!global::fixedNonInterpolatedL && !shouldWait)
        tube->updateL();
    if (shouldLowPassConnection)
        tube->lowPassConnection();

    if (global::bowing)
        tube->calculateVRel();

    tube->calculateVelocity();

    if (global::connectedToLip)
    {
        lipModel->setTubeStates(tube->getP(1, 0), tube->getV(0, 0));
        lipModel->calculateCollision();
        lipModel->calculateDeltaP();
        lipModel->calculate();
        tube->setFlowVelocities(lipModel->getUb(), lipModel->getUr());
    }

    tube->calculatePressure();
    tube->calculateRadiation();

    if (shouldDispCorr)
        tube->dispCorr();

    if (global::fixedNonInterpolatedL)
        calculateEnergy();
}

void Trombone::calculateEnergy()
{
    bool excludeLip = !global::connectedToLip;
    //    bool excludeLip = false;

    double kinEnergy = tube->getKinEnergy();
    double potEnergy = tube->getPotEnergy();
    double radEnergy = tube->getRadEnergy();
    double radDamp = tube->getRadDampEnergy();
    double lipEnergy = lipModel->getLipEnergy();
    double lipCollisionEnergy = lipModel->getCollisionEnergy();
    double lipPower = lipModel->getPower();
    double lipDamp = lipModel->getDampEnergy();

    double totEnergy = kinEnergy + potEnergy + radEnergy + (excludeLip ? 0 : (lipEnergy + lipCollisionEnergy));
    double energy1 = tube->getKinEnergy1() + tube->getPotEnergy1() + tube->getRadEnergy1() + (excludeLip ? 0 : (lipModel->getLipEnergy1() + lipModel->getCollisionEnergy1()));

    energySave << kinEnergy << ", ";
    energySave << potEnergy << ", ";
    energySave << radEnergy << ", ";
    energySave << radDamp << ", ";
    energySave << lipEnergy << ", ";
    energySave << lipCollisionEnergy << ", ";
    energySave << lipPower << ", ";
    energySave << lipDamp << ", ";
    energySave << energy1 << ";\n";

    //     scaledTotEnergy = (totEnergy + lipModel->getPower() + lipModel->getDampEnergy() + tube->getRadDampEnergy() - energy1) / energy1;
    scaledTotEnergy = (totEnergy + lipPower + lipDamp + radDamp - energy1) / pow(2, floor(log2(energy1)));
    scaledTotEnergySave << scaledTotEnergy << ";\n";
}

void Trombone::updateStates()
{
    tube->updateStates();
    lipModel->updateStates();
}

void Trombone::saveToFiles()
{
    if (!global::onlyWriteOutput)
    {
        massState << getLipOutput() << ";\n";

        for (int l = 0; l <= tube->getMaxN() + 1; ++l)
        {
            pState << tube->getP(0, l) << ", ";
            if (l < tube->getMaxN())
                vState << tube->getV(0, l) << ", ";
        }
        pState << ";\n";
        vState << ";\n";
        alfSave << tube->getAlf() << ";\n";
        MSave << tube->getM() << ";\n";
        MwSave << tube->getMw() << ";\n";

        for (int l = 0; l <= tube->getMaxN() + 1; ++l)
        {
            Ssave << tube->getSBar(l);
            if (l == tube->getMaxN() + 1)
                Ssave << ";\n";
            else
                Ssave << ",";
        }
    }
    output << tube->getOutput() << ";\n";
}

void Trombone::closeFiles()
{
    massState.close();
    pState.close();
    vState.close();
    alfSave.close();
    MSave.close();
    MwSave.close();
    energySave.close();
    scaledTotEnergySave.close();
    Ssave.close();
    output.close();
    tube->closeFiles();
}
