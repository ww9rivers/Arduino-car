// A program to control the Elegoo Car, with the functions listed below:
//
// o  IR remote control, selecting modes:
// o  Object avoidance;
// o  IR control;
// o  Line tracking.

#include <IRremote.h>

#include "Combo.h"
#include "avoidance.h"
#include "disttest.h"
#include "testing.h"

//  Logic control output pins
#define IN1 7         // Left  wheel forward
#define IN2 8         // Left  wheel reverse
#define IN3 9         // Right wheel reverse
#define IN4 11        // Right wheel forward

////////// IR REMOTE CODES //////////
#define UNKNOWN_F 0x00511dbb  // FORWARD  5316027
#define UNKNOWN_B 0xa3c8eddb  // BACK     2747854299
#define UNKNOWN_L 0x52a3d41f  // LEFT     1386468383
#define UNKNOWN_R 0x20fe4dbb  // RIGHT    553536955
#define UNKNOWN_S 0xd7e84b1b  // STOP     3622325019

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
//  Line tracking input pins
#define IN_LT_L   2
#define IN_LT_M   4
#define IN_LT_R   10

#define carSpeed 200  // initial speed of car >=0 to <=255

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
void set_car_speed ()  { analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed); }
// enable L298n A+B channels
void start_car () { digitalWrite(ENA, HIGH); digitalWrite(ENB,HIGH); }
void stop_car () {  digitalWrite(ENA, LOW);  digitalWrite(ENB, LOW);  Serial.println("Stop!"); } 

IRrecv irrecv(RECV_PIN);    // initialization
decode_results results;     // Define structure type

unsigned long val;
unsigned long preMillis;

/**
 * BEGIN DEFINE FUNCTIONS
 */

void go_reverse() {
  left_back();
  right_back();
  start_car();
  Serial.println("go back!");
}

void go_forward() {
  left_fore();
  right_fore();
  start_car();
  Serial.println("go forward!");
}

void turn_left() {
  left_back();
  right_fore();
  start_car();
  Serial.println("go left!");
}

void turn_right() {
  left_fore();
  right_back();
  start_car();
  Serial.println("go right!");
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
}

/**
 *  IR Blink mode setup.
 */
void ir_blink_setup() {
  pinMode(LED, OUTPUT); // initialize LED as an output
  irrecv.enableIRIn();  // Start receiving
}

/**
 *  Line Tracking IO define
 */
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

void tracking_setup(){
  pinMode(LT_R,INPUT);
  pinMode(LT_M,INPUT);
  pinMode(LT_L,INPUT);
}

void setup() {
  stop_car();
  Serial.begin(9600);   // debug output at 9600 baud
  auto_run_setup();
  avoidance_setup();
  ir_blink_setup();
  tracking_setup();
  set_car_speed();
  irrecv.enableIRIn();  
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
  Serial.println(val);
  irrecv.resume();
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

// Line tracking

void tracking_loop() {
  if(LT_M){
    go_forward();
  }
  else if(LT_R) {
    turn_right();
    while(LT_R);
  }
  else if(LT_L) {
    turn_left();
    while(LT_L);
  }
}

/** Check for mode change
 *
 * @param   ircode  Code received from the IR control.
 * @Returns: TRUE if op_mode is changed; FALSE if unchanged.
 */
bool op_mode_change (IR_Code ircode) {
  int previous_mode = op_mode;
  switch (ircode) {
    case IR_0:    op_mode = STOP_MODE; stop_setup(); break;
    case IR_1:    op_mode = AUTO_MODE; break;
    case IR_2:    op_mode = IR_MODE; break;
    case IR_3:    op_mode = avoidance_setup(); break;
    case IR_4:    op_mode = TRACKING_MODE; break;    
    case IR_5:    op_mode = TESTING_MODE; break;
    case IR_6:    op_mode = DISTTEST_MODE; break;
    default:
      return false;
  }
  if (op_mode == previous_mode) { return false; }
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
  }
}
