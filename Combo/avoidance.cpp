#include "Arduino.h"
#include "avoidance.h"
#include "utils.h"
#include <Servo.h>  // servo library

// create servo object to control servo
Servo myservo;

#include "avoidance.h"

const int Echo = A4;
const int Trig = A5;

/**
 * Object avoidance mode setup
 */
void avoidance_setup() { 
  myservo.attach(3);  // attach servo on pin 3 to servo object
  pinMode(Echo, INPUT);    
  pinMode(Trig, OUTPUT);  
}

/** 
 *  Object avoidance 
 */

// Ultrasonic distance measurement Sub function
int Distance_test() {
  digitalWrite(Trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);  
  delayMicroseconds(20);
  digitalWrite(Trig, LOW);   
  float Fdistance = pulseIn(Echo, HIGH);  
  Fdistance= Fdistance / 58;       
  return (int)Fdistance;
}

int rightDistance = 0, leftDistance = 0, middleDistance = 0;

void avoidance_loop() { 
  myservo.write(90);  // set servo position according to scaled value
  delay(500);
  middleDistance = Distance_test();

  if(middleDistance <= 20) {
    stop_car();
    delay(500);
    myservo.write(10);
    delay(1000);
    rightDistance = Distance_test();

    delay(500);
    myservo.write(90);
    delay(1000);
    myservo.write(180);
    delay(1000);
    leftDistance = Distance_test();

    delay(500);
    myservo.write(90);
    delay(1000);
    if(rightDistance > leftDistance) {
      turn_right();
      delay(360);
    }
    else if(rightDistance < leftDistance) {
      turn_left();
      delay(360);
    }
    else if((rightDistance <= 20) || (leftDistance <= 20)) {
      go_reverse();
      delay(180);
    }
    else {
      go_forward();
    }
  }
  else {
    go_forward();
  }
}
