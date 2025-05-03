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

// I think whenever possibile you should use a state machine
enum rocket_states {
  waiting,
  launch,
  apogee,
};

rocket_states state = waiting;
unsigned long prevTime = 0;
float minPressure = 9999;
float velocity = 0;
unsigned long eggStart = 0;

pressuresensor press_sen;
DFRobot_H3LIS200DL_I2C acce(&Wire, ACCEL_ADDR);
Servo Servo1;



void setup() {
  #if PRINT_EN
    Serial.begin(9600);
    Serial.print("------- HELLO WORLD -------\r\n");
  #endif

  while (!press_sen.begin());
  minPressure = press_sen.calibrate();

  #if PRINT_EN
    Serial.print("Ground Pressure "); 
    Serial.print(minPressure);
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

  long az = acce.readAccZ(); // Removing 1g of gravity
  Serial.println(az);
////////////////////////////////////////////////////////////////////////////////////////////////  Detecting Launch
  if (state == waiting)  {

    if (abs(az) < 0.5) {
      az = 0;
    }

    if (az < -4) {
      velocity = 0;
      prevTime = millis();
      eggStart = prevTime;
      state = launch;
      #if PRINT_EN
        Serial.print("LAUNCH DETECTED, BON VOYAGE\r\n");
      #endif
    }
////////////////////////////////////////////////////////////////////////////////////////////////  In Flight
  } else if (state == launch) {
    float pressure = press_sen.getPressure();
    unsigned long now = millis();
    float dt = (now - prevTime) / 1000;
    prevTime = now;
    velocity += az * -9.81 * dt;

    if ((eggStart + (EGG_STOP * 1000) > now) || (pressure >= minPressure + PRESSURE_MARGIN) /* && (abs(velocityAtMinPressure) < 0.5)) */) {
      state = apogee;
      #if PRINT_EN
        Serial.print("APOGEE DETECTED, AUTOBOTS ROLL OUT\r\n");
      #endif
    }

    if (pressure < minPressure) {
      minPressure = pressure;
    }
////////////////////////////////////////////////////////////////////////////////////////////////  Apogee Detected
  } else if (state == apogee) {
    #if PRINT_EN
      Serial.print("Velocity at Apogee: ");
      Serial.print(velocity);
      Serial.print("\r\nJOB DONE, FARWELL\r\n");
    #endif
    
    for (int pos = 90; pos >= 0; pos -= 2) {
      Servo1.write(pos);
      delay(20);
    }    

    while( 1 == 1);
  }
}
