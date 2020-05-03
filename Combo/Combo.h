/**
 *  Header file for utilities: In this case, functions to move/stop the car
 *  that are implemented in the main module, Combo.ino, but are used in the
 *  avoidance.cpp submodule.
 */
#ifndef _COMBO_H_
#define _COMBO_H_ 1

#include <Arduino.h>    // Must be included in submodule

/**
 *    Name the GPIO pins used in the car
 */

//  Logic control output pins
#define IN1 7         // Left  wheel forward
#define IN2 8         // Left  wheel reverse
#define IN3 9         // Right wheel reverse
#define IN4 11        // Right wheel forward

//  Channel enable output pins
#define ENA 5         // Left  wheel speed
#define ENB 6         // Right wheel speed
//  IR input and LED pins
#define RECV_PIN  12  // Infrared signal receiving pin
#define LED       13  // LED pin

//  Line tracking sensor pins
#define PIN_LT_L  2
#define PIN_LT_M  4
#define PIN_LT_R  10

//  I/O pins for servo & ultrasonic sensor for object detection
#define SERVERO_PIN 3   /* attach servo on pin 3 to servo object */
#define Echo  A4
#define Trig  A5

/**
 *  Car Speed settings: Between 0 and 255
 */
#define DRIVE_SPEED     200
#define TRACKING_SPEED  200
void set_car_speed (int speed);

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
  TESTING_MODE,
  DISTTEST_MODE,
  CHALLENGE1_MODE
} Op_Mode;
extern Op_Mode op_mode;

/**
 *  Timer -- Macros for timer operation
 */
#define timer_init(x)       { x = millis(); }
#define timer_exceeds(x, y) (millis()-x > y)

void set_LED(bool xst);
void stateChange(void);
#define LED_off() set_LED(LOW)
#define LED_on()  set_LED(HIGH)

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

void stop_setup(void);
void stop_loop(void);

#endif
