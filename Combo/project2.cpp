/**
 * Implementation of project 2: Detour
 * 1.  Drive car on track;
 * 2.  Stop car if object is detected;
*  3.  Detour around object;
*  4.  Resume driving on track.
 */

#include "project2.h"
#include "avoidance.h"
#include "tracking.h"

enum {
  OT_DETECT,
  OT_DIRECTION
};
void trace_object(void);


/**
 * Loop function for Project2: Trace around object if one
 * is detected; Otherwise, track line.
 */
void project2_loop () {
  trace_object();
  tracking_loop();
}

void trace_object () {
  switch (ot_mode) {
    case OT_DETECT:
    default:
      return;
  }
}
