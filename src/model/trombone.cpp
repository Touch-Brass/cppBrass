#include "trombone.h"

#include <memory>
#include <cmath>

//==============================================================================
Trombone::Trombone(ModelParams* params, double k, std::vector<std::vector<double>> &geometry) : k(k),
                                                                                                Pm(params->Pm)
{
    tube = std::make_unique<Tube>(params, k, geometry);
    lipModel = std::make_unique<LipModel>(params, k);
    lipModel->setTubeParameters(tube->getH(),
                                tube->getRho(),
                                tube->getC(),
                                tube->getSBar(0),
                                tube->getSHalf(0));
}

Trombone::~Trombone()
{
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

    //     scaledTotEnergy = (totEnergy + lipModel->getPower() + lipModel->getDampEnergy() + tube->getRadDampEnergy() - energy1) / energy1;
    scaledTotEnergy = (totEnergy + lipPower + lipDamp + radDamp - energy1) / pow(2, floor(log2(energy1)));
}

void Trombone::updateStates()
{
    tube->updateStates();
    lipModel->updateStates();
}
