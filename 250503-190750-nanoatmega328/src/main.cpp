/***************************************************************************
McKenzie Helms
Rev 4/18/2025 

Aero 660 Final Rocket Design Project
Code for appogee detection and parachute launch
 *****************************************************************************************/

#include "Parameters.h"
#include "PressureSensor.h"
#include <Adafruit_MPU6050.h>
#include <Servo.h> 

// I think whenever possibile you should use a state machine
enum rocket_states {
  waiting,
  launch,
  apogee,
};

#if TEST_MODE 
  rocket_states state = apogee;
#else
  rocket_states state = waiting;
#endif

unsigned long prevTime = 0;
float minPressure = 9999;
float velocity = 0;
unsigned long eggStart = 0;

pressuresensor barrometer;
Servo Servo1;
Adafruit_MPU6050 mpu;

void setup() {
  #if PRINT_EN
    Serial.begin(9600);
    Serial.print("------- HELLO WORLD -------\r\n");
  #endif

  Wire.begin();

  if (!barrometer.begin()) {
    #if PRINT_EN
      Serial.print("Error Setting up Barrometer\r\n");
    #endif
  }

  #if TEST_MODE == 0
    minPressure = barrometer.calibrate();
  #endif

  #if PRINT_EN
    Serial.print("Ground Pressure "); 
    Serial.print(minPressure);
    Serial.print(" hPa\r\n");
  #endif

  if (!mpu.begin()) {
    #if PRINT_EN
      Serial.print("MPU Setup Error\r\n");
    #endif
  }

  Servo1.attach(SERVO_PIN); 
  Servo1.write(0);
  
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);

  #if TEST_MODE
    delay(30*1000);
  #endif
}


void loop() {
////////////////////////////////////////////////////////////////////////////////////////////////  Always do
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float pressure = barrometer.getPressure();

  #if PRINT_EN
    Serial.print("Acceleration X: ");
    Serial.print(a.acceleration.x);
    Serial.print(" \tY: ");
    Serial.print(a.acceleration.y);
    Serial.print(" \tZ: ");
    Serial.print(a.acceleration.z);
    Serial.print(" m/s\r\nPressure: ");
    Serial.print(pressure);
    Serial.print(" hPa\r\n");
    Serial.print("Rotation X: ");
    Serial.print(g.gyro.x);
    Serial.print(", Y: ");
    Serial.print(g.gyro.y);
    Serial.print(", Z: ");
    Serial.print(g.gyro.z);
    Serial.print(" rad/s\r\n");
  #endif

  // Could also see if pressure is decreasing. This would be effectively the first derivative test to find a minium
  // If a turning point in pressure is detected, go to apogee state no matter what, even if launch hasn't been detected
  if ((pressure >= minPressure + PRESSURE_MARGIN) && !TEST_MODE /*|| (pressure - prevPressure) < 0*/) {
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

    if ((a.acceleration.x * a.acceleration.x) + (a.acceleration.y * a.acceleration.y) + (a.acceleration.z * a.acceleration.z) > 4 * 4 * 9.81 * 9.81) {
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
        Serial.print("Egg Timer Expired\r\n");
      #endif
    }
////////////////////////////////////////////////////////////////////////////////////////////////  Apogee Detected State
  } else if (state == apogee) {
    #if PRINT_EN
      Serial.print("Velocity at Apogee: ");
      Serial.print(velocity);
      Serial.print("\r\nJOB DONE, FARWELL\r\n");
    #endif

    for (int pos = 0; pos <= 180; pos += 2) {
      Servo1.write(pos);
      #if PRINT_EN
        Serial.println(pos);
      #endif
      delay(25);
    }    

    for (int pos = 180; pos >= 0; pos -= 2) {
      Servo1.write(pos);
      #if PRINT_EN
        Serial.println(pos);
      #endif
      delay(25);
    }    

    while(1 == 1);
  }
}
