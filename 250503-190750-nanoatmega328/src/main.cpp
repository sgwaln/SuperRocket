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
// DFRobot_H3LIS200DL_I2C acce(&Wire, ACCEL_ADDR);
Servo Servo1;



void setup() {
  #if PRINT_EN
    Serial.begin(9600);
    Serial.print("------- HELLO WORLD -------\r\n");
  #endif

  Wire.begin();

  while (!press_sen.begin());
  minPressure = press_sen.calibrate();

  #if PRINT_EN
    Serial.print("Ground Pressure "); 
    Serial.print(minPressure);
    Serial.print(" hPa\r\n");
  #endif

  acce.begin();
  acce.setRange(DFRobot_LIS::eH3lis200dl_100g);
  acce.setAcquireRate(DFRobot_LIS::eNormal_100HZ);

  Servo1.attach(SERVO_PIN); 
}


void loop() {
////////////////////////////////////////////////////////////////////////////////////////////////  Always do
  int ax = acce.readAccX();
  int ay = acce.readAccY();
  int az = acce.readAccZ();

  float pressure = press_sen.getPressure();

  #if PRINT_EN
    Serial.print("X+: ");
    Serial.print(ax);
    Serial.print(" g\tY+: ");
    Serial.print(ay);
    Serial.print(" g\tZ+: ");
    Serial.print(az);
    Serial.print("\r\nPressure: ");
    Serial.print(pressure);
    Serial.print(" hPa\r\n");
  #endif

  // Could also see if pressure is decreasing. This would be effectively the first derivative test to find a minium
  // If a turning point in pressure is detected, go to apogee state no matter what, even if launch hasn't been detected
  if ((pressure >= minPressure + PRESSURE_MARGIN) /*|| (pressure - prevPressure) < 0*/) {
    state = apogee;
    #if PRINT_EN
      Serial.print("\r\nMinium Pressure: ");
      Serial.print(minPressure);
      Serial.print("\r\nAPOGEE DETECTED, AUTOBOTS ROLL OUT\r\n");
    #endif
  }

  if (pressure < minPressure) {
    minPressure = pressure;
  }
////////////////////////////////////////////////////////////////////////////////////////////////  Detecting Launch State
  if (state == waiting)  {

    if (az < -4) {
      velocity = 0;
      prevTime = millis();
      eggStart = prevTime;
      state = launch;
      #if PRINT_EN
        Serial.print("LAUNCH DETECTED, BON VOYAGE\r\n");
      #endif
    }
////////////////////////////////////////////////////////////////////////////////////////////////  In Flight State
  } else if (state == launch) {
    unsigned long now = millis();

    if ((now - eggStart > (EGG_STOP * 1000)) /* && (abs(velocityAtMinPressure) < 0.5)) */) {
      state = apogee;
      #if PRINT_EN
        Serial.print("Egg timer remaining: ");
        Serial.print(((now - eggStart)/1000) - EGG_STOP);
      #endif
    }
////////////////////////////////////////////////////////////////////////////////////////////////  Apogee Detected State
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
