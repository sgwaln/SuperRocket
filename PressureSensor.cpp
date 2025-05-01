//For BMP388

#include "PressureSensor.h"

#if PRESSURE_SENSOR == BMP388
  #include <Wire.h>
  #include <SPI.h>
  #include <Adafruit_Sensor.h>
  #include "Adafruit_BMP3XX.h"
  Adafruit_BMP3XX bmp;
#elif PRESSURE_SENSOR == MPL3115A2
    #include <Adafruit_MPL3115A2.h>
    Adafruit_MPL3115A2 mpl;
#endif

#if PRESSURE_SENSOR == BMP388
    bmp.begin_SPI(CS, SCK, MISO, MOSI);
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);
#elif PRESSURE_SENSOR == MPL3115A2

#endif

bool PressureSensor::begin(void) {
    #if PRESSURE_SENSOR == BMP388

    #elif PRESSURE_SENSOR == MPL3115A2
        return mpl.begin();
    #endif
}

float PressureSensor::calibrate(void) {
    #if PRESSURE_SENSOR == BMP388

    #elif PRESSURE_SENSOR == MPL3115A2
        // Going to get a running average for a few seconds, then set Sea Pressure to that
        float CummulativeAverage = 0;

        unsigned int i = 0;
        for (unsigned long time1 = millis(); (time1 + (10 * 1000)) - millis() > (10 * 1000); i += 1) {
            CummulativeAverage = (mpl.getPressure() + (i * CummulativeAverage)) / (i + 1);
        }
        mpl.setSeaPressure(CummulativeAverage);
        return CummulativeAverage;
    #endif
}

float PressureSensor::getPressure(void) {
    #if PRESSURE_SENSOR == BMP388

    #elif PRESSURE_SENSOR == MPL3115A2
        return mpl.getPressure();
    #endif
}

float PressureSensor::getAltitude(void) {
    #if PRESSURE_SENSOR == BMP388

    #elif PRESSURE_SENSOR == MPL3115A2
        return mpl.getAltitude();
    #endif
}
