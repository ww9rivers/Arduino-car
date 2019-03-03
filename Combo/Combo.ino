// A program to control the Elegoo Car, with the functions listed below:
//
// o  IR remote control, selecting modes:
// o  Object avoidance;
// o  IR control;
// o  Line tracking.

#include <IRremote.h>
#include <Servo.h>  // servo library
Servo myservo;      // create servo object to control servo

int Echo = A4;
int Trig = A5; 

////////// IR REMOTE CODES //////////
#define UNKNOWN_F 0x00511dbb  // FORWARD  5316027
#define UNKNOWN_B 0xa3c8eddb  // BACK     2747854299
#define UNKNOWN_L 0x52a3d41f  // LEFT     1386468383
#define UNKNOWN_R 0x20fe4dbb  // RIGHT    553536955
#define UNKNOWN_S 0xd7e84b1b  // STOP     3622325019

/**     
 *      Full IR keypad code table (See README:Reference):
 */
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
//  Channel enable output pins
#define ENA 5         // Left  wheel speed
#define ENB 6         // Right wheel speed
//  Logic control output pins
#define IN1 7         // Left  wheel forward
#define IN2 8         // Left  wheel reverse
#define IN3 9         // Right wheel reverse
#define IN4 11        // Right wheel forward
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



IRrecv irrecv(RECV_PIN);    // initialization
decode_results results;     // Define structure type

unsigned long val;
unsigned long preMillis;

/**
 * BEGIN DEFINE FUNCTIONS
 */

void back(){
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("go back!");
}

void forward(){
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  //digitalWrite(ENA,HIGH);       // enable L298n A channel
  //digitalWrite(ENB,HIGH);       // enable L298n B channel
  digitalWrite(IN1,HIGH);         // set IN1 hight level
  digitalWrite(IN2,LOW);          // set IN2 low level
  digitalWrite(IN3,LOW);          // set IN3 low level
  digitalWrite(IN4,HIGH);         // set IN4 hight level
  Serial.println("go forward!");  // send message to serial monitor
}

void left(){
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  //digitalWrite(ENA,HIGH);
  //digitalWrite(ENB,HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("go left!");
}

void right(){
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW); 
  Serial.println("go right!");
}

void stop(){
   digitalWrite(ENA, LOW);
   digitalWrite(ENB, LOW);
   Serial.println("Stop!");
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
  Serial.begin(9600);   // debug output at 9600 baud
  auto_run_setup();
  avoidance_setup();
  ir_blink_setup();
  tracking_setup();
  stop();
  irrecv.enableIRIn();  
}

// Auto Run

// Repeat execution
void auto_run_loop() {
  forward();  //go forward
  delay(1000);//delay 1000 ms
  back();     //go back
  delay(1000);
  left();     //turning left
  delay(1000);
  right();    //turning right
  delay(1000);
}

// Object avoidance

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

  if(middleDistance <= 20) {
    stop();
    delay(500);
    myservo.write(10);
    delay(1000);
    rightDistance = Distance_test();

    delay(500);
    myservo.write(90);
    delay(1000);
    myservo.write(180);
    delay(1000);
    leftDistance = Distance_test();

    delay(500);
    myservo.write(90);
    delay(1000);
    if(rightDistance > leftDistance) {
      right();
      delay(360);
    }
    else if(rightDistance < leftDistance) {
      left();
      delay(360);
    }
    else if((rightDistance <= 20) || (leftDistance <= 20)) {
      back();
      delay(180);
    }
    else {
      forward();
    }
  }
  else {
    forward();
  }
}

//  Infrared Blink
bool state = LOW;           //define default input mode

void stateChange() {
  state = !state;          
  digitalWrite(LED, state);
  Serial.print("LED state changed to ");
  Serial.println(state);
}

//  Operation modes:
#define STOP_MODE       0
#define AUTO_MODE       1
#define IR_MODE         2
#define AVOIDANCE_MODE  3
#define TRACKING_MODE   4
int op_mode = IR_MODE;

//  Infared Control
int ir_control_loop() {
  // Check for mode change:
  if (irrecv.decode(&results)){ 
    preMillis = millis();
    val = results.value;
    Serial.println(val);
    irrecv.resume();
    switch(val){
      case IR_UP:
      case UNKNOWN_F: forward(); break;
      case IR_DOWN:
      case UNKNOWN_B: back(); break;
      case IR_LEFT:
      case UNKNOWN_L: left(); break;
      case IR_RIGHT:
      case UNKNOWN_R: right();break;
      case IR_OK:
      case UNKNOWN_S: stop(); break;
      case IR_STAR:
        stateChange();
        break;
      default:
        return val;
    }
  }
  else if (op_mode == IR_MODE) {
    if(millis() - preMillis > 500){
      stop();
      preMillis = millis();
    }
  }
  return 0;
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

/** Main loop
 *
 * The program combines several modes together, using the IR remote control to switch and control the car.
 *
 * When running in a specific mode, the "0" key is always used to stop the car.
 */
void loop() {
  switch (ir_control_loop()) {
    case IR_0:  op_mode = STOP_MODE; break;
    case IR_1:  op_mode = AUTO_MODE; break;
    case IR_2:  op_mode = IR_MODE; return;
    case IR_3:  op_mode = AVOIDANCE_MODE; break;
    case IR_4:  op_mode = TRACKING_MODE; break;    
  }
  switch (op_mode) {
    case STOP_MODE:
      stop();
      break;
    case AUTO_MODE:
      auto_run_loop();
      break;
    case IR_MODE:
      break;
    case AVOIDANCE_MODE:
      avoidance_loop();
      break;
    case TRACKING_MODE:
      tracking_loop();
      break;
  }
}
