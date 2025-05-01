#ifndef __Parameters_H
    #define __Parameters_H

    #define PRINT_EN 1

    #define SERVO_PIN 6

    #define ACCEL_ADDR 0x19

    // Not really sure if I like this, I thought it would be nice to support both, but I don't think this is good way to do it, I think maybe polymopryhism would be better
    // Valid Pressure Sensors MPL3115A2, BMP388 (untested)
    #define BMP388 1
    #define MPL3115A2 2
    #define PRESSURE_SENSOR MPL3115A2
#endif