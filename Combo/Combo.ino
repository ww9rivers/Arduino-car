// A program to control the Elegoo Car, with the functions listed below:
//
// o  IR remote control, selecting modes:
// o  Object avoidance;
// o  IR control;
// o  Line tracking.

#include <IRremote.h>
#include <Servo.h>  //servo library
Servo myservo;      // create servo object to control servo

int Echo = A4;  
int Trig = A5; 

////////// IR REMOTE CODES //////////
#define F 16736925  // FORWARD
#define B 16754775  // BACK
#define L 16720605  // LEFT
#define R 16761405  // RIGHT
#define S 16712445  // STOP
#define UNKNOWN_F 5316027     // FORWARD
#define UNKNOWN_B 2747854299  // BACK
#define UNKNOWN_L 1386468383  // LEFT
#define UNKNOWN_R 553536955   // RIGHT
#define UNKNOWN_S 3622325019  // STOP

#define RECV_PIN  12

/*define channel enable output pins*/
#define ENA 5   // Left  wheel speed
#define ENB 6   // Right wheel speed
/*define logic control output pins*/
#define IN1 7   // Left  wheel forward
#define IN2 8   // Left  wheel reverse
#define IN3 9   // Right wheel reverse
#define IN4 11  // Right wheel forward
#define carSpeed 150  // initial speed of car >=0 to <=255


//    The direction of the car's movement
//  ENA   ENB   IN1   IN2   IN3   IN4   Description  
//  HIGH  HIGH  HIGH  LOW   LOW   HIGH  Car is runing forward
//  HIGH  HIGH  LOW   HIGH  HIGH  LOW   Car is runing back
//  HIGH  HIGH  LOW   HIGH  LOW   HIGH  Car is turning left
//  HIGH  HIGH  HIGH  LOW   HIGH  LOW   Car is turning right
//  HIGH  HIGH  LOW   LOW   LOW   LOW   Car is stoped
//  HIGH  HIGH  HIGH  HIGH  HIGH  HIGH  Car is stoped
//  LOW   LOW   N/A   N/A   N/A   N/A   Car is stoped



IRrecv irrecv(RECV_PIN);    //initialization
decode_results results;     //Define structure type

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
  // analogWrite(ENA, carSpeed);
  // analogWrite(ENB, carSpeed);
  digitalWrite(ENA,HIGH); //enable L298n A channel
  digitalWrite(ENB,HIGH); //enable L298n B channel
  digitalWrite(IN1,HIGH); //set IN1 hight level
  digitalWrite(IN2,LOW);  //set IN2 low level
  digitalWrite(IN3,LOW);  //set IN3 low level
  digitalWrite(IN4,HIGH); //set IN4 hight level
  Serial.println("go forward!");  //send message to serial monitor
}

void left(){
  // analogWrite(ENA, carSpeed);
  // analogWrite(ENB, carSpeed);
  digitalWrite(ENA,HIGH);
  digitalWrite(ENB,HIGH);
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

void setup() {
  Serial.begin(9600);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  stop();
  irrecv.enableIRIn();  
}

//  Operation modes:
#define NONE_MODE       0
#define AUTO_MODE       1
#define IR_MODE         2
#define AVOIDANCE_MODE  3
#define TRACKING_MODE   4

/** Main loop
 *
 * The program combines several modes together, using the IR remote control to switch and control the car.
 * 
 * When running in a specific mode, the "0" key is always used to 
 */
void loop() {
  // Check for mode change:
  if (irrecv.decode(&results)){ 
    preMillis = millis();
    val = results.value;
    Serial.println(val);
    irrecv.resume();
    switch(val){
      case F: 
      case UNKNOWN_F: forward(); break;
      case B: 
      case UNKNOWN_B: back(); break;
      case L: 
      case UNKNOWN_L: left(); break;
      case R: 
      case UNKNOWN_R: right();break;
      case S: 
      case UNKNOWN_S: stop(); break;
      default: break;
    }
  }
  else{
    if(millis() - preMillis > 500){
      stop();
      preMillis = millis();
    }
  }
}

// infrared_Blink
#define RECV_PIN  12        //Infrared signal receiving pin
#define LED       13        //define LED pin
#define L         16738455
#define UNKNOWN_L 1386468383

bool state = LOW;           //define default input mode


void stateChange() {
  state = !state;          
  digitalWrite(LED, state);
}

void ir_blink_setup() {
  pinMode(LED, OUTPUT); //initialize LED as an output
  Serial.begin(9600);  // debug output at 9600 baud
  irrecv.enableIRIn();  // Start receiving
}

void ir_blink_loop() {
  if (irrecv.decode(&results)) { 
    val = results.value;
    Serial.println(val);
    irrecv.resume();      // Receive the next value
    delay(150);
    if(val == L || val == UNKNOWN_L) {  
      stateChange();
    }
  }
}

// Line tracking

//Line Tracking IO define
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)




void tracking_setup(){
  Serial.begin(9600);
  pinMode(LT_R,INPUT);
  pinMode(LT_M,INPUT);
  pinMode(LT_L,INPUT);
}

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

// Object avoidance

int rightDistance = 0, leftDistance = 0, middleDistance = 0;



//Ultrasonic distance measurement Sub function
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

void avoidance_setup() { 
  myservo.attach(3);  // attach servo on pin 3 to servo object
  Serial.begin(9600);     
  pinMode(Echo, INPUT);    
  pinMode(Trig, OUTPUT);  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  stop();
} 

void avoidance_loop() { 
    myservo.write(90);  //setservo position according to scaled value
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

// Auto Run


//before execute loop() function, 
//setup() function will execute first and only execute once
void auto_run_setup() {
  Serial.begin(9600);//open serial and set the baudrate
  pinMode(IN1,OUTPUT);//before useing io pin, pin mode must be set first 
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
}

//Repeat execution
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
