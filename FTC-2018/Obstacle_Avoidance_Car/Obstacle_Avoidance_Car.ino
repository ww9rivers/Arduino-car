//	By Jeffery
//www.elegoo.com

#include <Servo.h>  //servo library
Servo myservo;      // create servo object to control servo

int Echo = A0;
int Trig = A1;

#define ENB 7
#define IN1 3
#define IN2 4
#define IN3 5
#define IN4 6
#define ENA 2
#define carSpeed 250
int rightDistance = 0, leftDistance = 0, middleDistance = 0;

void forward(){
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Forward");
   digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  
}

void back() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Back");
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  
}

void left() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Left");  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
   digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
 
}

void right() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Right");
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

}

void stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  Serial.println("Stop!");
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
}

//Ultrasonic distance measurement Sub function
int getDistance() {
    digitalWrite(Trig, LOW);
    delayMicroseconds(2);
    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig, LOW);
    return (int)pulseIn(Echo, HIGH) / 58;
}

void setup() {
  myservo.attach(8);  // attach servo on pin 8 to servo object
  Serial.begin(9600);
  pinMode(Echo, INPUT);
  pinMode(Trig, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  stop();
}

void loop() {
    myservo.write(90);  //setservo position according to scaled value
    delay(500);
    middleDistance = getDistance();

    if(middleDistance <= 30) {
      stop();
      delay(10);
      myservo.write(10);
      delay(10);
      rightDistance = getDistance();

      delay(10);
      myservo.write(90);
      delay(10);
      myservo.write(180);
      delay(10);
      leftDistance = getDistance();

      delay(10);
      myservo.write(90);
      delay(10);
      if(rightDistance > leftDistance) {
        right();
        delay(10);
      }
      else if(rightDistance < leftDistance) {
        left();
        delay(10);
      }
      else if((rightDistance <= 20) || (leftDistance <= 20)) {
        back();
        delay(10);
      }
      else {
        forward();
      }
    }
    else {
        forward();
    }
}
