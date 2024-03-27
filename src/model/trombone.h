#pragma once

#include "global.h"
#include "tube.h"
#include "lipmodel.h"
#include "model_params.h"

#include <memory>

class Trombone
{
public:
    Trombone(ModelParams* params, double k, std::vector<std::vector<double>> &geometry);
    ~Trombone();

    void calculate();
    void calculateEnergy();

    float getOutput() { return tube->getOutput(); };
    float getLipOutput() { return lipModel->getY(); };

    void updateStates();

    void refreshLipModelInputParams() { lipModel->refreshInputParams(); };

    void setExtVals(double pVal, double lFVal, double LVal, bool wait = false)
    {
        //        lipModel->setExtVals (pVal, 2.0 * tube->getC() / (LVal));
        shouldWait = wait;
        lipModel->setExtVals(pVal, lFVal);
        tube->setExtVals(LVal);
    };

    void changeSetting() { tube->changeSetting(); };
    void changeSetting(bool b) { tube->changeSetting(b); };

    double getTubeC() { return tube->getC(); };
    double getTubeRho() { return tube->getRho(); };

    void setWait(bool w) { shouldWait = w; };

    double getBellRad() { return tube->getBellRad(); };

    std::unique_ptr<LipModel> lipModel;
private:
    std::unique_ptr<Tube> tube;

    double k, Pm;

    double scaledTotEnergy = 0;

    bool shouldLowPassConnection = false;
    bool shouldDispCorr = global::useDispCorr;

    bool shouldWait = false; // wait with changing L?
};
