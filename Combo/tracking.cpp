#include "tracking.h"

/**
 *  Line Tracking IO define
 */
#define LT_R !digitalRead(PIN_LT_R)
#define LT_M !digitalRead(PIN_LT_M)
#define LT_L !digitalRead(PIN_LT_L)

bool line_tracking;

/** Line Tracking LED sensor testing
 *
 * The Elegoo Arduino car kit has a line tracking sensor with 3 sets of LEDs, each is able
 * to read light refection: They are only able to function as a digital switch -- if there
 * is enough light reflection, it returns 1; Otherwise it returns 0. Analog input from
 * these LED sensors do not work, as the test functions below will show.
 *
 * This means that the line tracking function is only able to track black lines.
 */
void test_tracking_pin(int pin) {
  Serial.print(pin);
  Serial.print(" = ");
  Serial.print(digitalRead(pin));
  Serial.print(" / ");
  Serial.println(analogRead(pin));
}

void test_tracking(void) {
  test_tracking_pin(PIN_LT_L);
  test_tracking_pin(PIN_LT_M);
  test_tracking_pin(PIN_LT_R);
}

void tracking_setup(){
  pinMode(PIN_LT_R,INPUT);
  pinMode(PIN_LT_M,INPUT_PULLUP);
  pinMode(PIN_LT_L,INPUT);
  set_car_speed(TRACKING_SPEED);
  line_tracking_on();
}

// Line tracking

void tracking_loop() {
  if (line_tracking) {
    if(LT_M){
      go_forward();
    }
    else if(LT_R) {
      turn_right();
    }
    else if(LT_L) {
      turn_left();
    }
  }
}
