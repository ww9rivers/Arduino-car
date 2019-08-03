// By Claire

//define the L298n IO pin
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11
//functions where we can move sides of the wheel.
void rfor (){
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("right wheels forward");
}
void rback(){
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("right wheels backward");
}
void lfor (){
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  Serial.println("left wheels forward");
}
void lback(){
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}
//we can turn on the LED light too.
void LEDon(){
  digitalWrite(13, HIGH);
}
void LEDoff(){
  digitalWrite(13, LOW);
}
//functions that allow us to control the movement of the car as a whole
void forward(){
  rfor();
  lfor();
  Serial.println("Going forward");
  delay(500);
}
void back(){
  rback();
  lback();
  Serial.println("Going backward");
  delay(500);
}
void left(){
  rfor();
  lback();
  Serial.println("Turning left");
  delay(500); 
}

void right(){
  rfor();
  lback();
  Serial.println("Turning Right");
  delay(500);
}
//Getting pins ready
void setup() {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(ENA, HIGH);  
  digitalWrite(ENB, HIGH); 
}
//using the functions. Could also use forward and backward instead.
void loop() {
  rfor();
  lfor();
  LEDon();
  delay(500);
  rback();
  lback();
  LEDoff();
  delay(500);
}
//notice the green LED light on the corner of the shield
