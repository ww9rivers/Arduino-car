#ifndef _TRACKING_H_
#define _TRACKING_H_ 1

#include "Combo.h"

extern bool line_tracking;
#define line_tracking_off() line_tracking=false
#define line_tracking_on()  line_tracking=true

void test_tracking(void);
void tracking_setup(void);
void tracking_loop(void);

#endif
