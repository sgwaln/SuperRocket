/***************************************************************************
McKenzie Helms
Rev 4/18/2025 

Aero 660 Final Rocket Design Project
Code for appogee detection and parachute launch
 *****************************************************************************************/

//Include Files---------------------------------------------------------------------------
#include "Parameters.h"
#include "PressureSensor.h"
//For Accelerometer
#include <DFRobot_LIS.h>
//For Servo
#include <Servo.h> 
//---------------------------------------------------------------------------------------

#if ACCELL_EN
  DFRobot_H3LIS200DL_I2C acce(&Wire, 0x19); // Explicitly specify i2c address 0x19
  float velocity = 0;
  unsigned long prevTime = 0;
#endif

#if SERVO_EN
  int servoPin = 6; // Declare the pin the Servo is connected to 
  Servo Servo1; // Create a servo object 
  int pos = 0; //Initial Servo Position 
#endif

//Apogee Detection Variables
float minPressure = 9999;  // Start high so we can update downward
bool apogeeDetected = false;
bool servoDeployed = false;
bool launchDetected = false;
//---------------------------------------------------------------------------
PressureSensor pressureSen;

void setup() {
  //FOR TESTING ########################################################################
  #if PRINT_EN
    Serial.begin(9600);
    Serial.print(apogeeDetected);
    Serial.print("\t HELLO WORLD\t\r\n");
  #endif

  while (!pressureSen.begin());
  float sealevel = pressureSen.calibrate();
  #if PRINT_EN
    Serial.print("Ground Pressure "); 
    Serial.print(sealevel);
    Serial.print(" hPa\r\n");
  #endif

  //For Accelerometer-------------------------------------------------------------
  Wire.begin(); // Ensure I2C is initialized
  #if ACCEL_EN
    acce.begin();
    acce.setRange(DFRobot_LIS::eH3lis200dl_200g);
    acce.setAcquireRate(DFRobot_LIS::eNormal_100HZ);
    prevTime = millis();  // set initial time
  #endif

  //For Servo----------------------------------------------------------------------
  //Attach the servo object to the servo on pin declared in initilization
  #if SERVO_EN
    Servo1.attach(servoPin); 
    Servo1.write(90);
  #endif
}

void loop() {
  //For BMP388----------------------------------------------------------------------
  // Read pressure and track minimum
  int stablePressureReadings = 0;
  const int pressureWarmupCount = 10;  // number of good readings to skip
  float pressure = pressureSen.getPressure();
  #if PRINT_EN
    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.print(" hPa, MinPressure: ");
    Serial.print(minPressure);
    Serial.println(" hPa");
  #endif  
  //For Accelerometer-------------------------------------------------------------
  // Raw acceleration in g
  #if ACCEL_EN
    long az = acce.readAccZ();
    // Convert to m/s²
    float aaz = az * 9.81;
    // Time step for integration
    unsigned long now = millis();
    float dt = (now - prevTime) / 1000.0;
    prevTime = now;
    // Subtract gravity and integrate
    float accelZ_net = aaz - 9.81;  // Remove gravity
    #if PRINT_EN      
      Serial.print("Velocity: ");
      Serial.print(velocity);
      Serial.println(" m/s");
    #endif
    //For Launch Detection ----------------------------------------------------------
    if (abs(accelZ_net) < 0.5) {
      accelZ_net = 0;
    }
  #else
    float accelZ_net = 0;
    float velocity = 0;
    float dt = 0;
  #endif

  if ((!launchDetected && accelZ_net) < (-4 * 9.81)) {
    launchDetected = true;
    velocity = 0;  // reset velocity when we actually start flying
  }

  if (launchDetected) {
    velocity += (-accelZ_net * dt);
  }

  //For Apogee Detection-------------------------------------------------------------
  if (stablePressureReadings < pressureWarmupCount) {
    stablePressureReadings++;
    #if PRINT_EN
      Serial.println("Warming up... waiting for stable pressure.");
    #endif
    // Once the last warmup tick finishes, set a valid minPressure
    if (stablePressureReadings >= pressureWarmupCount) {
      minPressure = pressure;
      #if PRINT_EN
        Serial.print("Warmup complete. Initial minPressure set to: ");
        Serial.println(minPressure);
      #endif
    }

    #if PRINT_EN
      Serial.print(apogeeDetected);
      Serial.println(" apogee detected? 0 no, 1 yes  ");
    #endif

  } else {
    if (pressure < minPressure) {
      minPressure = pressure;
    }
  }

  #if PRINT_EN
    Serial.print(apogeeDetected);
    Serial.println(" apogee detected? 0 no, 1 yes  ");
  #endif

  if ((!apogeeDetected) && (pressure >= minPressure + 0.50)) { //&& (abs(velocityAtMinPressure) < 0.5)) {
    apogeeDetected = true;

    //For Servo to turn at apogee----------------------------------------------------------------------
    #if PRINT_EN
    //Serial.print("Velocity at Apogee: ");
    //Serial.println(velocityAtMinPressure);
      Serial.println("APOGEE DETECTED DEPLOYING SERVO");
    #endif

    for (int pos = 90; pos >= 0; pos -= 2) {   //this is inside the apogee detection if statement
        #if SERVO_EN
          Servo1.write(pos);
        #endif
        delay(20);
    }
    servoDeployed = true;
  }

  delay(100);  // Small delay so values dont overwrite other values

}
