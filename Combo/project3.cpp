/**
 * Implementation of project 3: Escape - Drive the car out of walled room.
 *
 * Algorithm:
 *  1.  Reach the wall;
 *  2.  Decide to turn left or right;
 *  3.  Track wall to the right or left; and front;
 *  4.  Turn right or left if the wall disappears on the right or left;
 *  5.  Stop car if wall disappears both on the side and to the front.
 */

#include "project3.h"

enum {
  OT_START,
  OT_DETECT,
  OT_TURNING,
  OT_TRACK,
  OT_ESCAPE,
  OT_STOP
} p3_state;

// Interval for front / right scan in millseconds
#define SCAN_INTERVAL   600
#define QTURN_INTERVAL  800

unsigned long p3_timer;
#define p3timer_init()      timer_init(p3_timer)
#define p3timer_exceeds(x)  timer_exceeds(p3_timer, x)

void track_wall_start (void);

/**
 * Detect wall in front of the car. If detected, change p3_state to
 * OT_TRACK_WAIT to wait for the sensor to be ready.
 * If wall is not detected, and the p3_timer is set (after the wall
 * has been detected once), turn to track wall at SCAN_INTERVAL.
 */
void p3_detect_wall (void) {
  int dist = distance_avg();
  if (sensor_not_ready(dist)) {
    // Wait longer if sensor not ready:
    return;
  }
  if (object_in_range(dist)) {
    Serial.print("Wall detected, distance = ");
    Serial.println(dist);
    turn_left();
    p3_state = OT_TURNING;
  } else if (p3_timer != 0 && p3timer_exceeds(SCAN_INTERVAL)) {
    // Track wall @SCAN_INTERVAL if p3_timer is set:
    track_wall_start();
  } else {
    go_forward();
  }
}

/**
 * Found openning and started turning right.
 * After 90 degree turn is complete, drive forward and escape.
 */
void p3_escape (void) {
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
 * Stop the car after 2 seconds.
 */
void p3_stop (void) {
  if (p3timer_exceeds(2000)) {
    stop_car();
  }
}

/**
 * Check that the wall is on the right side - Sensor should point right.
 * If so, turn sensor to point ahead at SCAN_INTERVAL to detect wall ahead;
 * If not, turn right and get out.
 */
void p3_track_wall (void) {
  int dist = distance_average(5);
  if (sensor_not_ready(dist)) {
    // Wait until the sensor is ready (in position)
    return;
  }
  Serial.print("track_wall: wall tracking. distance = ");
  Serial.println(dist);
  if (object_in_range(dist)) {
    if (p3timer_exceeds(SCAN_INTERVAL)) {
      stop_car();
      set_sensor(MEASURE_FRONT);
      p3timer_init();
      p3_state = OT_DETECT;
    }
  } else if (object_outof_range(dist)) {
    Serial.println("track_wall: Escape detected.");
    turn_right();
    set_sensor(MEASURE_FRONT);
    timer_init(p3_timer);
    p3_state = OT_ESCAPE;
  } else {
    go_forward();
  }
}

/**
 * Check for the wall to disappear from front, in which case, turn the 
 * sensor to right; Keep turning left otherwise.
 */
void p3_turning (void) {
  if (object_near(distance_avg())) {
    turn_left();
  } else {
    track_wall_start();
  }
}

/**
 * Loop function for project 3.
 */
void project3_loop (void) {
  switch (p3_state) {
    case OT_START:      p3_start(); break;
    case OT_DETECT:     p3_detect_wall(); break;
    case OT_TURNING:    p3_turning(); break;
    case OT_TRACK:      p3_track_wall(); break;
    case OT_ESCAPE:     p3_escape(); break;
    case OT_STOP:       p3_stop(); break;
  }
}

void project3_setup (void) {
  avoidance_setup();
  set_car_speed(175);
  p3_timer = 0;
  p3_state = OT_START;
}

/**
 * Check for the wall to disappear from front, in which case, turn the 
 * sensor to right; Keep turning left otherwise.
 */
void track_wall_start (void) {
  Serial.println("track_wall_start");
  go_forward();
  p3timer_init();
  set_sensor(MEASURE_RIGHT);
  p3_state = OT_TRACK;
}
