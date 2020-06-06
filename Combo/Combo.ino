// A program to control the Elegoo Car, with the functions listed below:
//
// o  IR remote control, selecting modes:
// o  Object avoidance;
// o  IR control;
// o  Line tracking.

#include <IRremote.h>

#include "Combo.h"
#include "avoidance.h"
#include "challenge1.h"
#include "disttest.h"
#include "project2.h"
#include "project3.h"
#include "project4.h"
#include "testing.h"
#include "tracking.h"

////////// IR REMOTE CODES //////////
#define UNKNOWN_F 0x00511dbb  // FORWARD  5316027
#define UNKNOWN_B 0xa3c8eddb  // BACK     2747854299
#define UNKNOWN_L 0x52a3d41f  // LEFT     1386468383
#define UNKNOWN_R 0x20fe4dbb  // RIGHT    553536955
#define UNKNOWN_S 0xd7e84b1b  // STOP     3622325019

int carSpeed;         // initial speed of car >=0 to <=255
void set_car_speed (int spd) { carSpeed = spd; }

//    The direction of the car's movement
//  ENA   ENB   IN1   IN2   IN3   IN4   Description  
//  HIGH  HIGH  HIGH  LOW   LOW   HIGH  Car is runing forward
//  HIGH  HIGH  LOW   HIGH  HIGH  LOW   Car is runing back
//  HIGH  HIGH  LOW   HIGH  LOW   HIGH  Car is turning left
//  HIGH  HIGH  HIGH  LOW   HIGH  LOW   Car is turning right
//  HIGH  HIGH  LOW   LOW   LOW   LOW   Car is stoped
//  HIGH  HIGH  HIGH  HIGH  HIGH  HIGH  Car is stoped
//  LOW   LOW   N/A   N/A   N/A   N/A   Car is stoped
void left_back ()  { digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); }
void left_fore ()  { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); }
void left_stop ()  { digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW); }
void right_back () { digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); }
void right_fore () { digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); }
void right_stop () { digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW); }
// enable L298n A+B channels
void start_car () { analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed); }
void stop_car () {  digitalWrite(ENA, 0);  digitalWrite(ENB, 0); } 

IRrecv irrecv(RECV_PIN);    // initialization
decode_results results;     // Define structure type

unsigned long val;          // IR code
unsigned long preMillis;

/**
 * BEGIN DEFINE FUNCTIONS
 */
enum {
  GO_NONE,
  GO_FORWARD,
  GO_REVERSE,
  GO_LEFT,
  GO_RIGHT
} drive_direction = GO_NONE;

void go_reverse() {
  if (drive_direction != GO_REVERSE) {
    left_back();
    right_back();
    start_car();
    drive_direction = GO_REVERSE;
    Serial.println("go back!");
  }
}

void go_forward() {
  if (drive_direction != GO_FORWARD) {
    left_fore();
    right_fore();
    start_car();
    drive_direction = GO_FORWARD;
    Serial.println("go forward!");
  }
}

void turn_left() {
  if (drive_direction != GO_LEFT) {
    left_back();
    right_fore();
    start_car();
    drive_direction = GO_LEFT;
    Serial.println("go left!");
  }
}

void turn_right() {
  if (drive_direction != GO_RIGHT) {
    left_fore();
    right_back();
    start_car();
    drive_direction = GO_RIGHT;
    Serial.println("go right!");
  }
}

/**
 *  Before execute loop() function,
 *  setup() function will execute first and only execute once. 
 */
void auto_run_setup() {
  pinMode(IN1,OUTPUT);// before useing io pin, pin mode must be set first 
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  set_car_speed(DRIVE_SPEED);
}

/**
 *  IR Blink mode setup.
 */
void ir_blink_setup() {
  pinMode(LED, OUTPUT); // initialize LED as an output
  irrecv.enableIRIn();  // Start receiving
}

void setup() {
  Serial.begin(9600);   // debug output at 9600 baud
  auto_run_setup();
  avoidance_setup();
  ir_blink_setup();
  tracking_setup();
  irrecv.enableIRIn();  
  stop_setup();
}

// Auto Run
#define AUTORUN_FORE  0
#define AUTORUN_BACK  1
#define AUTORUN_LEFT  2
#define AUTORUN_RIGHT 3
int autorun_mode = AUTORUN_FORE;  // Current auto run mode
int autorun_time = 0;             // Time in the current mode

// Repeat execution
void auto_run_loop() {
  int now = millis();
  if (now > autorun_time) {
    autorun_time = now+999;
    switch (autorun_mode) {
    case AUTORUN_FORE:
      go_forward();          // go forward
      break;
    case AUTORUN_BACK:
      go_reverse();       // go back
      break;
    case AUTORUN_LEFT:
      turn_left();             // turning left
      break;
    case AUTORUN_RIGHT:
      turn_right();            // turning right
    }
    if (++autorun_mode > AUTORUN_RIGHT) { autorun_mode = AUTORUN_FORE; }
  }
}


/** IR Control
 *
 */
Op_Mode op_mode = IR_MODE;
//  Infrared Blink
bool state = LOW;           //define default input mode

/**
 *  Set LED to specified state.
 */
void set_LED(bool xst) {
  digitalWrite(LED, state = xst);
}

void stateChange() {
  set_LED(!state);          
  Serial.print("LED state changed to ");
  Serial.println(state);
}

//  Infared Control reception
IR_Code ir_control_code() {
  if (!irrecv.decode(&results)) {
    return IR_NONE;
  }

  timer_init(preMillis);
  IR_Code val = (IR_Code)results.value;
  irrecv.resume();
  if (val <= IR_NONE) {
    return IR_NONE;
  }
  Serial.print("IR code = ");
  Serial.println(val);
  return val;
}

/** Perform IR control mode actions.
 * 
 */
void ir_control_loop(IR_Code val) {
  switch(val){
    case IR_UP:     go_forward(); break;
    case IR_DOWN:   go_reverse(); break;
    case IR_LEFT:   turn_left(); break;
    case IR_RIGHT:  turn_right();break;
    case IR_OK:     stop_car(); break;
    default:
      if (timer_exceeds(preMillis, 500)) {
        stop_car();
        timer_init(preMillis);
      }
  }
}

/** Check for mode change
 *
 * @param   ircode  Code received from the IR control.
 * @Returns: TRUE if op_mode is changed; FALSE if unchanged.
 */
bool op_mode_change (IR_Code ircode) {
  const char * mode_text[] = {
    "STOP_MODE",
    "AUTO_MODE",
    "IR_MODE",
    "AVOIDANCE_MODE",
    "TRACKING_MODE",
    "TESTING_MODE",
    "DISTTEST_MODE",
    "PROJECT1_MODE",
    "PROJECT2_MODE",
    "PROJECT3_MODE",
    "PROJECT4_MODE"
  };
  int previous_mode = op_mode;
  switch (ircode) {
    case IR_POUND:stop_setup(); break;
    case IR_1:    auto_run_setup(); op_mode = AUTO_MODE; break;
    case IR_2:    ir_blink_setup(); op_mode = IR_MODE; break;
    case IR_3:    avoidance_setup(); op_mode = AVOIDANCE_MODE; break;
    case IR_4:    tracking_setup(); op_mode = TRACKING_MODE; break;
    case IR_5:    op_mode = TESTING_MODE; break;
    case IR_6:    op_mode = DISTTEST_MODE; break;
    case IR_7:    challenge1_setup(); op_mode = PROJECT1_MODE; break;
    case IR_8:    project2_setup(); op_mode = PROJECT2_MODE; break;
    case IR_9:    project3_setup(); op_mode = PROJECT3_MODE; break;
    case IR_0:    project4_setup(); op_mode = PROJECT4_MODE; break;
    default:
      return false;
  }

  if (op_mode == previous_mode) { return false; }
  Serial.println(mode_text[op_mode]);
  stateChange();
  return true;
}

/** Main loop
 *
 * The program combines several modes together, using the IR remote control to switch and control the car.
 *
 * When running in a specific mode, the "0" key is always used to stop the car.
 */
void loop() {
  IR_Code ircode = ir_control_code();
  if (op_mode_change(ircode)) {
    stop_car();
    // ...may perform op_mode specific initialization here...
    return;
  }
  switch (op_mode) {
    case STOP_MODE:       stop_loop(); break;
    case AUTO_MODE:       auto_run_loop(); break;
    case IR_MODE:         ir_control_loop(ircode); break;
    case AVOIDANCE_MODE:  avoidance_loop(); break;
    case TRACKING_MODE:   tracking_loop(); break;
    case TESTING_MODE:    testing_loop(ircode); break;
    case DISTTEST_MODE:   disttest_loop(ircode); break;
    case PROJECT1_MODE:   challenge1_loop(ircode); break;
    case PROJECT2_MODE:   project2_loop(); break;
    case PROJECT3_MODE:   project3_loop(); break;
    case PROJECT4_MODE:   project4_loop(); break;
  }
}
