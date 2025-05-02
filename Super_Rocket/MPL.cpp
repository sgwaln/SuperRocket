#include "Parameters.h"
#if PRESSURE_SENSOR == MPL3115A2

#include "PressureSensor.h"
#include <Adafruit_MPL3115A2.h>
Adafruit_MPL3115A2 mpl;

bool PressureSensor::begin(void) {
    return mpl.begin();
}

float PressureSensor::calibrate(void) {
    // Going to get a running average for a few seconds, then set Sea Pressure to that
    float CummulativeAverage = 0;

    unsigned int i = 0;
    for (unsigned long time1 = millis(); (millis() - time1) < (10 * 1000); i += 1) {
        CummulativeAverage = (mpl.getPressure() + (i * CummulativeAverage)) / (i + 1);
    }
    mpl.setSeaPressure(CummulativeAverage);
    return CummulativeAverage;
}

float PressureSensor::getPressure(void) {
    return mpl.getPressure();
}

float PressureSensor::getAltitude(void) {
    return mpl.getAltitude();
}

#endif