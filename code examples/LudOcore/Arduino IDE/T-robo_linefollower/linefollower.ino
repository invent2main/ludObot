/**
 * Exemple de code suiveur de ligne pour T-robo avec un seul capteur IR
 */
#include <Servo.h>

#define proxIR  17

Servo myservo5;
int val5 = 120;  // lever
uint16_t THROTTLE_OLD;

//motors
int LmotorPWM = 3;
int LmotorDIR = 4;
int RmotorPWM = 6;
int RmotorDIR = 7;

bool TurnL = 1;
int valT = 0;  // turn

void setup()
{
  myservo5.attach(5);
  myservo5.write(val5);  
  
  pinMode(LmotorPWM, OUTPUT);
  pinMode(LmotorDIR, OUTPUT);
  digitalWrite(LmotorPWM, LOW);
  digitalWrite(LmotorDIR, LOW);
  pinMode(RmotorPWM, OUTPUT);
  pinMode(RmotorDIR, OUTPUT);
  digitalWrite(RmotorPWM, LOW);
  digitalWrite(RmotorDIR, LOW);

  pinMode(proxIR,INPUT);
  
  delay(100);
}


void loop()
{

  if (digitalRead(proxIR) == false) {    //forward
    analogWrite(LmotorPWM, 150);
    analogWrite(RmotorPWM, 180);
    digitalWrite(LmotorDIR, HIGH);
    digitalWrite(RmotorDIR, HIGH);  
    valT = 0;
  }
  else {

    if (TurnL == 1) {    
      analogWrite(RmotorPWM, 180); // left
      analogWrite(LmotorPWM, 120);//      digitalWrite(LmotorPWM, LOW);
      digitalWrite(LmotorDIR, LOW);
      digitalWrite(RmotorDIR, HIGH);
      delay(valT=valT+10); 
      digitalWrite(LmotorPWM, LOW);
      digitalWrite(LmotorDIR, LOW);
      digitalWrite(RmotorPWM, LOW);
      digitalWrite(RmotorDIR, LOW);
      delay(10);      
      if (digitalRead(proxIR) == true) {TurnL = 0; } 
    }

    else {
      analogWrite(LmotorPWM, 180); // right
      digitalWrite(LmotorDIR, HIGH);
      analogWrite(RmotorPWM, 120); //     digitalWrite(RmotorPWM, LOW);
      digitalWrite(RmotorDIR, LOW);  
      delay(valT=valT+10); 
      digitalWrite(LmotorPWM, LOW);
      digitalWrite(LmotorDIR, LOW);
      digitalWrite(RmotorPWM, LOW);
      digitalWrite(RmotorDIR, LOW);
      delay(10);       
      if (digitalRead(proxIR) == true) {TurnL = 1;}
    }

    if (valT >= 160) { 
    analogWrite(LmotorPWM, 150);
    analogWrite(RmotorPWM, 180);
    digitalWrite(LmotorDIR, HIGH);
    digitalWrite(RmotorDIR, HIGH);  
    valT = 0;
    delay(200); 
    
    }      
  }

}

