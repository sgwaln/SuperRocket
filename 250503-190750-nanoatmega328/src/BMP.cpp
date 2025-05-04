//For BMP388

#include "Parameters.h"
#if PRESSURE_SENSOR == BMP388

#include "PressureSensor.h"

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

Adafruit_BMP3XX bmp;
bmp.begin_SPI(CS, SCK, MISO, MOSI);
bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
bmp.setOutputDataRate(BMP3_ODR_50_HZ);


bool pressuresensor::begin(void) {
    return true;
}

float pressuresensor::calibrate(void) {
    return SEALEVELPRESSURE_HPA;
}

float pressuresensor::getPressure(void) {
    return -1;
}

float pressuresensor::getAltitude(void) {
    return -1;
}

#endif
