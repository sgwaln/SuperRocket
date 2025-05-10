#ifndef __Parameters_H
    #define __Parameters_H

    #define PRINT_EN 1
    #define TEST_MODE 1

    #define SERVO_PIN 6
    #define ACCEL_ADDR 0x18

    // This determins how long since launch the egg timer triggers, which will put the rocket
    // into apoggee mode, depoying the recovery system
    #define EGG_STOP 7         /* seconds */

    // This is the margin of how much pressure has to increase from the minium pressure to tigger 
    // the recovery system
    #define PRESSURE_MARGIN 0.5 /* hPa */
    
    // Un-used, same idea as pressure margin, is the minium velocity that the recovery system
    // is deployed
    #define VELOCITY_MARGIN 0.5 /* m/s */

    // Valid Pressure Sensors MPL3115A2, BMP388 (untested)
    #define BMP388 1
    #define MPL3115A2 2
    #define PRESSURE_SENSOR MPL3115A2
#endif