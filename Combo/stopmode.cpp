/**
 *  Stop mode: Stop the car; flash the LED in 1 second interval.
 */

#include "Combo.h"

unsigned long stop_timer;

void stop_setup() {
	stop_car();
  Serial.println("Stopped!");
	timer_init(stop_timer);
}
void stop_loop() {
	if (timer_exceeds(stop_timer, 1000)) {
		stateChange();
		timer_init(stop_timer);
	}
}
