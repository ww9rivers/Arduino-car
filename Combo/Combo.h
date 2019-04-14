/**
 *  Header file for utilities: In this case, functions to move/stop the car
 *  that are implemented in the main module, Combo.ino, but are used in the
 *  avoidance.cpp submodule.
 */
#ifndef _COMBO_H_
#define _COMBO_H_ 1

/**
 *      Full IR keypad code table (See README:Reference):
 */
typedef enum {
  IR_NONE   = 0,          // Use zero to indicate no IR code received
  IR_UP     = 0xFF629D,   // Up Arrow
  IR_LEFT   = 0xFF22DD,   // Left Arrow
  IR_OK     = 0xFF02FD,
  IR_RIGHT  = 0xFFC23D,   // Right Arrow
  IR_DOWN   = 0xFFA857,   // Down Arrow
  IR_1      = 0xFF6897,
  IR_2      = 0xFF9867,
  IR_3      = 0xFFB04F,
  IR_4      = 0xFF30CF,
  IR_5      = 0xFF18E7,
  IR_6      = 0xFF7A85,
  IR_7      = 0xFF10EF,
  IR_8      = 0xFF38C7,
  IR_9      = 0xFF5AA5,
  IR_0      = 0xFF4AB5,
  IR_STAR   = 0xFF42BD,   // *
  IR_POUND  = 0xFF52AD    // #
} IR_Code;

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

void left_back (void);
void left_fore (void);
void left_stop (void);
void right_back (void);
void right_fore (void);
void right_stop (void);
void set_car_speed (void);
void start_car (void);
void stop_car (void);

void stop_car (void);
void go_reverse(void);
void go_forward(void);
void turn_left(void);
void turn_right(void);

#endif