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

const int angle[] = { 180, 90, 0 };
const char * angletext[] = { "left", "front", "right" };
int odistance[3];
#define leftDistance    odistance[0]
#define middleDistance  odistance[1]
#define rightDistance   odistance[2]
#define last_dist       odistance[measuring_pos]

static int old_pos = -1;
int measuring_pos = MEASURE_FRONT;

enum {
  MEASURING,
  TURNING
} main_mode = MEASURING;
int turning_direction = 1;      // FORWARD: 1, REVERSE: -1
unsigned long turn_timer;
unsigned long debug_timer = 0;

/**
 * Object avoidance mode setup
 */
void avoidance_setup() {
  if (op_mode != AVOIDANCE_MODE) {
    Serial.println("Entering AVOIDANCE_MODE...");
    myservo.attach(SERVERO_PIN);
    pinMode(Echo, INPUT);    
    pinMode(Trig, OUTPUT);
    turning_direction = 1;
    odistance[0] = odistance[1] = odistance[2] = OUT_OF_RANGE;
    old_pos = -1;
    measuring_pos = MEASURE_FRONT;
    turn_sensor();
    set_car_speed(TRACKING_SPEED);
  }
}

/**
 *  Set fixed sensor direction and start turning the sensor to it.
 *  The turning_direction is set to 0 (neutral) 
 */
void set_sensor (int sdir) {
  if (measuring_pos != sdir) {
    measuring_pos = sdir;
    turn_sensor();
  }
  turning_direction = 0;
}

/**
 *  Set sensor angle to the next measuring position. Initialize turn_timer
 *  to measure / gauge if the sensor is in position, ready to detect object
 *  in the measuring direction.
 */
void turn_sensor(void) {
  if (old_pos != measuring_pos) {
    Serial.print("Turning sensor to ");
    Serial.println(angletext[measuring_pos]);
    myservo.write(angle[measuring_pos]);
    old_pos = measuring_pos;
    timer_init(turn_timer);
    Serial.print("Set turn_timer to ");
    Serial.println(turn_timer);
    main_mode = TURNING;
  }
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
  unsigned long mtime = pulseIn(Echo, HIGH);
  int dist = (mtime > 0) ? floor(mtime/(20000/340)) : OUT_OF_RANGE;
  int diff = dist-last_dist;
  if ((object_in_range(dist) || object_in_range(last_dist)) && (diff < -10 || diff > 10)) {
    Serial.print("Measured "); Serial.print(measuring_pos);
    Serial.print(", Last distance = "); Serial.print(last_dist);
    Serial.print(", Detected = "); Serial.print(dist);
    Serial.print(", mtime = "); Serial.println(mtime);
    last_dist = dist;
  }
  return dist;
}

/**
 *  Function to detect object in 3 positions in front of the car.
 *
 * @returns true, if an object is detected; false if not.
 */
int distance_scan() {
  if (main_mode == MEASURING) {
    int dist = distance_test();
    if (object_outof_range(dist)) {
      measuring_pos += turning_direction;
      if (measuring_pos > MEASURE_RIGHT || measuring_pos < MEASURE_LEFT) {
        turning_direction = -turning_direction;
        measuring_pos = MEASURE_FRONT;
      }
      turn_sensor();
    }
    return dist;
  } else if (timer_exceeds(turn_timer, 1000)) {
    Serial.println("measuring_loop: Turning => Detecting");
    main_mode = MEASURING;
    return distance_test();
  }
  return SENSOR_TURNING;
}

/**
 *  Detecting object in 3 frontal positions, and avoid hitting the object if one
 *  is detected.
 */
void avoidance_loop() {
  // Detect object(s) and mearure distances:
  int dist = distance_scan();

  // Avoide object(s) if anything is detected:
  if (!object_near(dist)) {
    go_forward();
  } else {
    stateChange();
    if ((object_in_range(leftDistance) && rightDistance > leftDistance) || rightDistance > middleDistance) {
      turn_right();
    } else if ((object_in_range(rightDistance) && rightDistance < leftDistance) || leftDistance > middleDistance) {
      turn_left();
    } else {
      go_reverse();
    }
  }
}
