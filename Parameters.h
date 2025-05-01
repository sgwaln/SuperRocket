#ifndef __Parameters_H
    #define __Parameters_H

    #define TEST_MODE 1

    #if TEST_MODE
        #define ACCELL_EN 0
        #define PRESSURE_EN 1
        #define SERVO_EN 0
        #define PRINT_EN 1
    #else
        #define ACCELL_EN 1
        #define PRESSURE_EN 1
        #define SERVO_EN 1
        #define PRINT_EN 0
    #endif

    // Valid Pressure Sensors MPL3115A2, BMP388
    #define BMP388 1
    #define MPL3115A2 2
    #define PRESSURE_SENSOR MPL3115A2

#endif