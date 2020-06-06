/**
 * Implementation of project 3: Escape - Drive the car out of walled room.
 */

#include "project3.h"

// Interval for front / right scan in millseconds
#define SCAN_INTERVAL   100
#define QTURN_INTERVAL  200

enum {
  OT_START,
  OT_DETECT,
  OT_TURNING,
  OT_TRACK,
  OT_ESCAPE,
  OT_STOP
} p3_state;

unsigned long p3_timer;
#define p3timer_init()      timer_init(p3_timer)
#define p3timer_exceeds(x)  timer_exceeds(p3_timer, x)

void track_wait (void);
void track_wall (void);
void track_wall_start (void);

void project3_setup (void) {
  avoidance_setup();
  p3_timer = 0;
  p3_state = OT_START;
}

/**
 * Detect wall in front of the car. If detected, change p3_state to
 * the next: OT_TRACK_WAIT to wait for the sensor to be ready.
 * If wall is not detected, and the p3_timer is set, turn to track
 * wall at SCAN_INTERVAL.
 */
void detect_wall (void) {
  int dist = distance_test();
  if (sensor_not_ready(dist)) { return; }
  if (object_near(dist)) {
    turn_left();
    p3_state = OT_TURNING;
  } else if (p3_timer != 0 && p3timer_exceeds(SCAN_INTERVAL)) {
    track_wall_start();
  }
}

/**
 * Found openning and started turning right.
 * After 90 degree turn is complete, drive forward and escape.
 */
void escape (void) {
  if (p3timer_exceeds(QTURN_INTERVAL)) {
    go_forward();
    p3timer_init();
    Serial.println("p3_stop: Escaped!");
    p3_state = OT_STOP;
  }
}

/**
 * Starting detecting the wall in front of car.
 * Also, initialize a timer to start wall tracking.
 */
void p3_start (void) {
  go_forward();
  Serial.println("p3_start: Moving forward");
  p3_state = OT_DETECT;
}

/**
 * Stop the car after 1 second.
 */
void p3_stop (void) {
  if (p3timer_exceeds(1000)) {
    stop_car();
  }
}

/**
 * Check for the wall to disappear from front, in which case, turn the 
 * sensor to right; Keep turning left otherwise.
 */
void p3_turning (void) {
  if (!object_near(distance_test())) {
    track_wall_start();
  }  
}

/**
 * Loop function for project 3.
 */
void project3_loop (void) {
  switch (p3_state) {
    case OT_START:      p3_start(); break;
    case OT_DETECT:     detect_wall(); break;
    case OT_TURNING:    p3_turning(); break;
    case OT_TRACK:      track_wall(); break;
    case OT_ESCAPE:     escape(); break;
    case OT_STOP:       p3_stop(); break;
  }
}

/**
 * Check that the wall is on the right side - Sensor should point right.
 * If so, turn sensor to point ahead at SCAN_INTERVAL to detect wall ahead;
 * If not, turn right and get out.
 */
void track_wall (void) {
  int dist = distance_test();
  if (sensor_not_ready(dist)) { return; }
  if (object_near(dist)) {
    if (p3timer_exceeds(SCAN_INTERVAL)) {
      stop_car();
      set_sensor(MEASURE_FRONT);
      p3timer_init();
      p3_state = OT_DETECT;
    }
  } else {
    Serial.println("track_wall: Escape detected.");
    turn_right();
    timer_init(p3_timer);
    p3_state = OT_ESCAPE;
  }
}

/**
 * Check for the wall to disappear from front, in which case, turn the 
 * sensor to right; Keep turning left otherwise.
 */
void track_wall_start (void) {
  Serial.println("track_wall_start");
  go_forward();
  set_sensor(MEASURE_RIGHT);
  p3_state = OT_TRACK;
}
