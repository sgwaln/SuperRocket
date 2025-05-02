#ifndef __PressureSensor_H
#define __PressureSensor_H

#include "Parameters.h"

#define SEALEVEL_PRESSURE 1013.25 /* Default sea level pressure */

#if PRESSURE_SENSOR == BMP388
    #define BMP_SCK 13
    #define BMP_MISO 12
    #define BMP_MOSI 11
    #define BMP_CS 10
#elif PRESSURE_SENSOR == MPL3115A2
    #define MPL_ADDR 0x13
    #define MPL_SCL 12
    #define MPL_SDA 11
#else
    #error "Incompatiable Pressure Sensor"
#endif

class PressureSensor {
    public:
        // PressureSensor(void);
        bool begin(void);
        float calibrate(void);
        float getPressure(void);
        float getAltitude(void);
};

#endif