/**
 * Implementation of project 1 (Challenge 1): Drive car on track;
 * Stop car if object is detected; Restart when object is removed.
 */

#include "challenge1.h"
#include "avoidance.h"
#include "tracking.h"

enum {
  OT_DETECT,
  OT_WAIT,
  OT_NOP
};
int ot_mode;

void detect_object (void);

/**
 * Setup for Challenge 1 with both line tracking and object
 * avoidance capabilities.
 */
void challenge1_setup () {
  avoidance_setup();
  tracking_setup();
  ot_mode = OT_NOP;
  LED_off();
}

/**
 * Loop function for Challenge 1: Stop and wait if object
 * is detected; Restart when the object is removed.
 */
void challenge1_loop (IR_Code ircode) {
  switch (ircode) {
    case IR_UP: ot_mode = OT_NOP; break;
    case IR_DOWN: ot_mode = OT_DETECT; break;
    default: break;
  }
  detect_object();
  tracking_loop();
}

void detect_object () {
  switch (ot_mode) {
    case OT_DETECT:
      if (object_near(distance_test())) {
        stop_car();
        LED_on();
        line_tracking_off();
        ot_mode = OT_WAIT;
      }
      return;
    case OT_WAIT:
      if (object_near(distance_test())) {
        start_car();
        LED_off();
        line_tracking_on();
        ot_mode = OT_DETECT;
      }
    default:
      return;
  }
}
