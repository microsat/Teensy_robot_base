
#define MAX_PWM        255

#include "DaulHBridge.h"
#include <Encoders.h>
#include "diff_controller.h"
Encoders encoders;
DaulHBridge motor;
const int ledPin = 11;
int last = 1;
char report[150];

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  encoders.init();

}

void loop() {
  // put your main code here, to run repeatedly:
  static uint16_t lastDisplayTime;
  static uint16_t lastMotorTime;
  static uint8_t lastTime;
// just something to let us know we are up and running
  if ((uint8_t)(millis() - lastTime) >= 250)
  {
    lastTime = millis();
    if (last == 1)
    {
      last = 0;
      digitalWrite(ledPin, HIGH);   // set the LED on
    } else
    {
      last = 1;
      digitalWrite(ledPin, LOW);    // set the LED off
    }
  }
// we just hard coding setpoint for now.....  
    leftPID.TargetTicksPerFrame = 5.0;
    rightPID.TargetTicksPerFrame = 5.0;

	// testing out diff_controller pid stuff

    if ((uint8_t)(millis() - lastMotorTime) >= 50)
    {
      lastMotorTime = millis();
      leftPID.Encoder = encoders.getCountsLeft();
      rightPID.Encoder = encoders.getCountsRight();
      doPID(&leftPID);
      doPID(&rightPID);
      motor.setLeftSpeed(leftPID.output);
      motor.setRightSpeed(rightPID.output);
    }
  
/*
  motor.setLeftSpeed(222);
  motor.setRightSpeed(222);
*/
  if ((millis() - lastDisplayTime) >= 500)
  {
    lastDisplayTime = millis();
    int16_t countsLeft = encoders.getCountsLeft();
    int16_t countsRight = encoders.getCountsRight();
    bool errorLeft = encoders.checkErrorLeft();
    bool errorRight = encoders.checkErrorRight();
    // Send the information to the serial monitor also.
    snprintf_P(report, sizeof(report),
        PSTR("%6d %6d %3d  %6d %3d"),
        countsLeft, countsRight, leftPID.output, rightPID.output);
    Serial.println(report);
  }

}
