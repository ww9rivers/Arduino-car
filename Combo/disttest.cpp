/**
 *	Distance detection test module.
 */
#include <Arduino.h>    // Must be included in submodule
#include "avoidance.h"
#include "disttest.h"

/**
 *	Distance testing loop: Set the LED on when object is detected; Set it off when no
 *	object is detected.
 *
 *	When no object is detected, the sensor is rotated to detect at 3 different angles: 0, 90, 180.
 *
 *	When an object is detected, the sensor stays in its position until the object is removed.
 */
void disttest_loop (IR_Code ircode) {
	turning_direction = 0;
	set_LED(measuring_loop() ? HIGH : LOW);
	switch(ircode) {
	case IR_LEFT:	measuring_pos = MEASURE_LEFT; break;
	case IR_UP:	measuring_pos = MEASURE_FRONT; break;
	case IR_RIGHT:	measuring_pos = MEASURE_RIGHT; break;
	}
	turn_sensor();
}