/**
 *  Header file for utilities: In this case, functions to move/stop the car
 *  that are implemented in the main module, Combo.ino, but are used in the
 *  avoidance.cpp submodule.
 */
#ifndef _COMBO_H_
#define _COMBO_H_ 1

//  Operation modes:
typedef enum {
  STOP_MODE,
  AUTO_MODE,
  IR_MODE,
  AVOIDANCE_MODE,
  TRACKING_MODE,
  TESTING_MODE
} Op_Mode;
extern Op_Mode op_mode;

/**
 *  Timer -- Macros for timer operation
 */
#define timer_init(x)       { x = millis(); }
#define timer_exceeds(x, y) (millis()-x > y)

void stateChange(void);

void stop_car (void);
void go_reverse(void);
void go_forward(void);
void turn_left(void);
void turn_right(void);

#endif