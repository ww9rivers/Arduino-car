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
#include "disttest.h"
#include <Servo.h>      // servo library

// create servo object to control servo
Servo myservo;
#define SERVERO_PIN 3   /* attach servo on pin 3 to servo object */

#define Echo  A4
#define Trig  A5
#define OIR   20  /* Object in range */
const int angle[] = { 1, 90, 180 };
int odistance[3] = { OOR, OOR, OOR };
#define leftDistance    odistance[0]
#define middleDistance  odistance[1]
#define rightDistance   odistance[2]

int measuring_pos = MEASURE_FRONT;

enum {
  MEASURING,
  TURNING
} main_mode = MEASURING;
int turning_direction = 1;      // FORWARD: 1, REVERSE: -1
unsigned long turn_timer;
bool obj_detected = false;

/**
 * Object avoidance mode setup
 */
Op_Mode avoidance_setup() {
  if (op_mode != AVOIDANCE_MODE) {
    myservo.attach(SERVERO_PIN);
    myservo.write(90);
    pinMode(Echo, INPUT);    
    pinMode(Trig, OUTPUT);
    main_mode = MEASURING;
    measuring_pos = MEASURE_FRONT;
    turning_direction = 1;
    odistance[0] = odistance[1] = odistance[2] = OOR;
  }
  return AVOIDANCE_MODE;
}

/**
 *  Set sensor angle to the next measuring position.
 */
void turn_sensor(void) {
  myservo.write(angle[measuring_pos]);
  main_mode = TURNING;
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
int distance_test() {
  digitalWrite(Trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(20);
  digitalWrite(Trig, LOW);   
  unsigned long mtime = pulseIn(Echo, HIGH, 360);
  return (mtime > 0) ? (mtime*(340/20000)) : OOR;
}

/**
 *  Function to detect object in 3 positions in front of the car.
 */
bool measuring_loop() {
  if (main_mode == MEASURING) {
    if ((odistance[measuring_pos] = distance_test()) < OIR) {
      return true;
    }
    timer_init(turn_timer);
    measuring_pos += turning_direction;
    if (measuring_pos > MEASURE_RIGHT || measuring_pos < MEASURE_LEFT) {
      turning_direction = -turning_direction;
      measuring_pos = MEASURE_FRONT;
    }
    turn_sensor();
  } else if (timer_exceeds(turn_timer, 1000)) {
    main_mode = MEASURING;
    return false;
  }
}

/**
 *  Detecting object in 3 frontal positions, and avoid hitting the object if one
 *  is detected.
 */
void avoidance_loop() {
  // Detect object(s) and mearure distances:
  obj_detected = measuring_loop();

  // Avoide object(s) if anything is detected:
  if (!obj_detected) {
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
