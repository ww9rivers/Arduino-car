#include "avoidance.h"
#include "tracking.h"

enum {
  OT_DETECT,
  OT_WAIT,
  OT_NOP
} ot_mode;
bool line_tracking;

void detect_object (void);

/**
 * Setup for Challenge 1 with both line tracking and object
 * avoidance capabilities.
 */
void challenge1_setup () {
  avoidance_setup();
  tracking_setup();
  op_mode = CHALLENGE1_MODE;
  ot_mode = OT_NOP;
  line_tracking = true;
  LED_off();
}

/**
 * Loop function for Challenge 1: Trace around object if one
 * is detected; Otherwise, track line.
 */
void challenge1_loop (IR_Code ircode) {
  switch (ircode) {
    case IR_UP: ot_mode = OT_NOP; break;
    case IR_DOWN: ot_mode = OT_DETECT; break;
    default: break;
  }
  detect_object();
  if (line_tracking) {
    tracking_loop();
  }
}

void detect_object () {
  switch (ot_mode) {
    case OT_DETECT:
      if (distance_test() < ONR) {
        stop_car();
        LED_on();
        line_tracking = false;
        ot_mode = OT_WAIT;
      }
      return;
    case OT_WAIT:
      if (distance_test() >= ONR) {
        start_car();
        LED_off();
        line_tracking = true;
        ot_mode = OT_DETECT;
      }
    default:
      return;
  }
}
