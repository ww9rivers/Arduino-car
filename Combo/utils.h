/**
 *  Header file for utilities: In this case, functions to move/stop the car
 *  that are implemented in the main module, Combo.ino, but are used in the
 *  avoidance.cpp submodule.
 */

/**
 *  Timer -- Macros for timer operation
 */
#define timer_init(x)       { x = millis(); }
#define timer_exceeds(x, y) (millis()-x > y)

void stop_car (void);
void go_reverse(void);
void go_forward(void);
void turn_left(void);
void turn_right(void);
