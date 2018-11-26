//
// Created by trotfunky on 26/11/18.
//

#ifndef LL_CONTROLSETTINGS_H
#define LL_CONTROLSETTINGS_H

#include "Arduino.h"

struct ControlSettings {

public:
    float maxAcceleration;
    float minAcceleration;

    float maxSpeed;
    float maxTranslationSpeed;
    float maxRotationSpeed;

    uint8_t stopDelay;

    uint8_t tolerancyTranslation;
    uint8_t tolerancyRotation;
    uint8_t tolerancySpeed;
    uint8_t tolerancyDiferential;
    uint8_t tolerancyDerivative;
};

#endif //LL_CONTROLSETTINGS_H
