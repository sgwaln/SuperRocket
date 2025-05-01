/***************************************************************************
McKenzie Helms
Rev 4/18/2025 

Aero 660 Final Rocket Design Project
Code for appogee detection and parachute launch
 *****************************************************************************************/

#include "Parameters.h"
#include "PressureSensor.h"
#include <DFRobot_LIS.h>
#include <Servo.h> 

// I really like state diagrams, so I made it state bassed, I think that makes it more readable
enum rocket_states {
  waiting,
  launch,
  apogee,
};

rocket_states state = waiting;
unsigned long prevTime = 0;
float minPressure = 9999;
float velocity = 0;

PressureSensor pressureSen;
DFRobot_H3LIS200DL_I2C acce(&Wire, ACCEL_ADDR);
Servo Servo1;



void setup() {
  #if PRINT_EN
    Serial.begin(9600);
    Serial.print("------- HELLO WORLD -------\r\n");
  #endif

  while (!pressureSen.begin());
  float sealevel = pressureSen.calibrate();

  #if PRINT_EN
    Serial.print("Ground Pressure "); 
    Serial.print(sealevel);
    Serial.print(" hPa\r\n");
  #endif

  Wire.begin();
  acce.begin();
  acce.setRange(DFRobot_LIS::eH3lis200dl_200g);
  acce.setAcquireRate(DFRobot_LIS::eNormal_100HZ);

  Servo1.attach(SERVO_PIN); 
  Servo1.write(90);
}

void loop() {

  float pressure = pressureSen.getPressure();
  long az = acce.readAccZ() - 1;

////////////////////////////////////////////////////////////////////////////////////////////////  Detecting Launch
  if (state == waiting)  {
    #if PRINT_EN
      Serial.print("Pressure: ");
      Serial.print(pressure);
      Serial.print(" hPa \r\n MinPressure: ");
      Serial.print(minPressure);
      Serial.print(" hPa \r\n Velocity: ");
      Serial.print(velocity);
      Serial.print(" m/s\r\n");
    #endif

    if (abs(az) < (0.5 / 9.81)) {
      az = 0;
    }

    if (az < -4) {
      velocity = 0;
      prevTime = millis();
      state = launch;
      #if PRINT_EN
        Serial.print("LAUNCH DETECTED, BON VOYAGE\r\n");
      #endif
    }
////////////////////////////////////////////////////////////////////////////////////////////////  In Flight
  } else if (state == launch) {
    unsigned long now = millis();
    float dt = (now - prevTime) / 1000.0;
    prevTime = now;
    velocity += (az * -9.81 * dt);
    if (pressure < minPressure) {
      minPressure = pressure;
    }

    if ((pressure >= minPressure + 0.50) /* && (abs(velocityAtMinPressure) < 0.5)) */) {
      state = apogee;
      Serial.print("APOGEE DETECTED, AUTOBOTS ROLL OUT\r\n");
    }
////////////////////////////////////////////////////////////////////////////////////////////////  Apogee Detected
  } else if (state == apogee) {
    #if PRINT_EN
      Serial.print("Velocity at Apogee: ");
      Serial.println(velocity);
      Serial.print("JOB DONE, FARWELL\r\n");
    #endif
    
    for (int pos = 90; pos >= 0; pos -= 2) {
      Servo1.write(pos);
      delay(20);
    }    

    return;
  }
}
