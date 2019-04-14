/**
 *  Object avoidance module
 *  
 *  The logic (algorithm) of object avoidance:
 *  
 *  1. The car moves forward (as the ultrasound sensor is mouted at the front);
 *  2. While moving, the ultrasound sensor (USS) is triggerd to emit a sound;
 *  3. If an echo is received by the USS, then it is assumed that an object is
 *     detected at the direction of the sensor;
 *  4. The car is moved in the opposite direction of the sensor.
 */
#include <Arduino.h>    // Must be included in submodule
#include "Combo.h"
#include "avoidance.h"
#include <Servo.h>      // servo library

// create servo object to control servo
Servo myservo;

#include "avoidance.h"

const int Echo = A4;
const int Trig = A5;
const int angle[] = { 1, 90, 180 };
int odistance[3] = { 999, 999, 999 };
#define leftDistance    odistance[0]
#define middleDistance  odistance[1]
#define rightDistance   odistance[2]
#define MEASURE_LEFT    0
#define MEASURE_FRONT   1
#define MEASURE_RIGHT   2
int measuring_mode = MEASURE_FRONT;
enum {
  MEASURING,
  TURNING
} main_mode = MEASURING;
int turning_direction = 1;      // FORWARD: 1, REVERSE: -1
unsigned long turn_timer;

/**
 * Object avoidance mode setup
 */
Op_Mode avoidance_setup() {
  if (op_mode != AVOIDANCE_MODE) {
    myservo.attach(3);  // attach servo on pin 3 to servo object
    myservo.write(90);
    pinMode(Echo, INPUT);    
    pinMode(Trig, OUTPUT);
    main_mode = MEASURING;
    measuring_mode = MEASURE_FRONT;
    turning_direction = 1;
    odistance[0] = odistance[1] = odistance[2] = 999;
  }
  return AVOIDANCE_MODE;  
}

/** 
 *  Ultrasonic distance measurement Sub function 
 *
 *  Algorithm for ultrasound distance measurement:
 *    Distance = (Time x Speed of sound in air (340 m/s))/2 meters
 *             = (((ut / 1000000)*(340*100))/2) cm
 *             = (ut*340/20000) cm
 * @return Centimeters rounded to the nearest integer.
 */
int Distance_test() {
  digitalWrite(Trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);   
  return (int)(pulseIn(Echo, HIGH)*(340/20000));
}

void avoidance_loop() {
  // Detect object(s) and mearure distances:
  if (main_mode == MEASURING) {
    odistance[measuring_mode] = Distance_test();
    main_mode = TURNING;
    timer_init(turn_timer);
    measuring_mode += turning_direction;
    if (measuring_mode > MEASURE_RIGHT || measuring_mode < MEASURE_LEFT) {
      turning_direction = -turning_direction;
      measuring_mode = MEASURE_FRONT;
    }
    myservo.write(angle[measuring_mode]);
  } else if (timer_exceeds(turn_timer, 600)) {
    main_mode = MEASURING;
  }

  // Avoide object(s) if anything is detected:
  if (middleDistance > 20 && rightDistance > 20 && leftDistance > 20) {
    go_forward();
  } else {
    stateChange();
    if (rightDistance > leftDistance) {
      turn_right();
    } else if (rightDistance < leftDistance) {
      turn_left();
    } else {
      go_reverse();
    }
  }
}
