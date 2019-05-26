// By Max Bian

// A program to control the Elegoo Car, with the functions listed below:
//
// o  IR remote control, selecting modes:
// o  Object avoidance;
// o  IR control;
// o  Line tracking.

#include <IRremote.h>

// create servo object to control servo
#include <Servo.h>  // servo library
Servo myservo;

int Echo = A4;
int Trig = A5; 

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
 *      Full IR keypad code table (See README:Reference):
 */
#define IR_NONE   0           // Use zero to indicate no IR code received
#define IR_UP     0xFF629D    // Up Arrow
#define IR_LEFT   0xFF22DD    // Left Arrow
#define IR_OK     0xFF02FD
#define IR_RIGHT  0xFFC23D    // Right Arrow
#define IR_DOWN   0xFFA857    // Down Arrow
#define IR_1      0xFF6897
#define IR_2      0xFF9867
#define IR_3      0xFFB04F
#define IR_4      0xFF30CF
#define IR_5      0xFF18E7
#define IR_6      0xFF7A85
#define IR_7      0xFF10EF
#define IR_8      0xFF38C7
#define IR_9      0xFF5AA5
#define IR_0      0xFF4AB5
#define IR_STAR   0xFF42BD    // *
#define IR_POUND  0xFF52AD    // #

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
 *  Timer -- Macros for timer operation
 */
#define timer_init(x)       { x = millis(); }
#define timer_exceeds(x, y) (millis()-x > y)

/**
 * BEGIN DEFINE FUNCTIONS
 */

void back() {
  left_back();
  right_back();
  start_car();
  Serial.println("go back!");
}

void forward() {
  left_fore();
  right_fore();
  start_car();
  Serial.println("go forward!");
}

void left() {
  left_back();
  right_fore();
  start_car();
  Serial.println("go left!");
}

void right() {
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
 * Object avoidance mode setup
 */
void avoidance_setup() { 
  myservo.attach(3);  // attach servo on pin 3 to servo object
  pinMode(Echo, INPUT);    
  pinMode(Trig, OUTPUT);  
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
      forward();          // go forward
      break;
    case AUTORUN_BACK:
      back();             // go back
      break;
    case AUTORUN_LEFT:
      left();             // turning left
      break;
    case AUTORUN_RIGHT:
      right();            // turning right
    }
    if (++autorun_mode > AUTORUN_RIGHT) { autorun_mode = AUTORUN_FORE; }
  }
}

/** 
 *  Object avoidance 
 */

// Ultrasonic distance measurement Sub function
int Distance_test() {
  digitalWrite(Trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);  
  delayMicroseconds(20);
  digitalWrite(Trig, LOW);   
  float Fdistance = pulseIn(Echo, HIGH);  
  Fdistance= Fdistance / 58;       
  return (int)Fdistance;
}

int rightDistance = 0, leftDistance = 0, middleDistance = 0;

void avoidance_loop() { 
  myservo.write(90);  // set servo position according to scaled value
  delay(500);
  middleDistance = Distance_test();

  if( avoidance_loop ) {
    if (middleDistance <= 50) {
      stop_car();
      }
    delay(100);
    myservo.write(10);
    delay(200);
    rightDistance = Distance_test();

    
    myservo.write(90);
    delay(200);
    leftDistance = Distance_test();

    delay(100);
    myservo.write(180);
    delay(200);
    if(rightDistance > leftDistance) {
      right();
      delay(72);
      forward();
    }
    else if(rightDistance < leftDistance) {
      left();
      delay(72);
      forward();
    }
    else if((rightDistance <= 10) || (leftDistance <= 10)) {
      back();
      delay(36);
    }
    else {
      forward();
    }
  }
  else {
    forward();
  }
}

/** IR Control
 *
 */
//  Infrared Blink
bool state = LOW;           //define default input mode

void stateChange() {
  state = !state;          
  digitalWrite(LED, state);
  Serial.print("LED state changed to ");
  Serial.println(state);
}

//  Operation modes:
enum {
  STOP_MODE,
  AUTO_MODE,
  IR_MODE,
  AVOIDANCE_MODE,
  TRACKING_MODE,
  TESTING_MODE
} op_mode = IR_MODE;

//  Infared Control reception
int ir_control_code() {
  if (!irrecv.decode(&results)) {
    return IR_NONE;
  }

  timer_init(preMillis);
  val = results.value;
  Serial.println(val);
  irrecv.resume();
  return val;
}

/** Perform IR control mode actions.
 * 
 */
void ir_control_loop(int val) {
  switch(val){
    case IR_UP:     forward(); break;
    case IR_DOWN:   back(); break;
    case IR_LEFT:   left(); break;
    case IR_RIGHT:  right();break;
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
    forward();
  }
  else if(LT_R) {
    right();
    while(LT_R);                             
  }
  else if(LT_L) {
    left();
    while(LT_L);  
  }
}

// Individual function testing
enum {
  TEST_LEFT, TEST_RIGHT
} test_mode = TEST_LEFT;

void testing_loop (int ircode) {
  switch(test_mode) {
    case TEST_LEFT:
      switch (ircode) {
        case IR_UP:     left_fore(); right_stop(); break;
        case IR_DOWN:   left_back(); right_stop(); break;
        case IR_RIGHT:  left_stop(); test_mode = TEST_RIGHT; return;
      }
      break;
    case TEST_RIGHT:
      switch (ircode) {
        case IR_UP:     left_stop(); right_fore(); break;
        case IR_DOWN:   left_stop(); right_back(); break;
        case IR_LEFT:   right_stop(); test_mode = TEST_LEFT; return;
      }
      break;
  }
   start_car();
}

/** Check for mode change
 *
 * @param   ircode  Code received from the IR control.
 * @Returns: TRUE if op_mode is changed; FALSE if unchanged.
 */
bool op_mode_change (int ircode) {
  int previous_mode = op_mode;
  switch (ircode) {
    case IR_0:    op_mode = STOP_MODE; break;
    case IR_1:    op_mode = AUTO_MODE; break;
    case IR_2:    op_mode = IR_MODE; break;
    case IR_3:    op_mode = AVOIDANCE_MODE; break;
    case IR_4:    op_mode = TRACKING_MODE; break;    
    case IR_5:    op_mode = TESTING_MODE; break;
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
  int ircode = ir_control_code();
  if (op_mode_change(ircode)) {
    stop_car();
    // ...may perform op_mode specific initialization here...
    return;
  }
  switch (op_mode) {
    case STOP_MODE:       stop_car(); break;
    case AUTO_MODE:       auto_run_loop(); break;
    case IR_MODE:         ir_control_loop(ircode); break;
    case AVOIDANCE_MODE:  avoidance_loop(); break;
    case TRACKING_MODE:   tracking_loop(); break;
    case TESTING_MODE:    testing_loop(ircode); break;
  }
}
