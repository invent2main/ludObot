/**
 * Exemple de code pour utiliser le capteur de gestes APDS9960 de la LudOhead avec une carte LudOcore
 */
#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include <LedControl.h>

#include <Servo.h>

bool Timon = 1;
int timer1_counter = 0;

Servo myservo6;
int val6 = 90;  // clamp
unsigned int diff_clamp;
unsigned int clamp_state = 0;

Servo myservo5;
int val5 = 90;  // lever
uint16_t THROTTLE_OLD;

#define proxIR  17
#define micIn  A7
int val = 0;  // variable to store the value read


//motors
int LmotorPWM = 3;
int LmotorDIR = 4;
int RmotorPWM = 6; //9;
int RmotorDIR = 7;

const int DIN_PIN = 12;
const int CS_PIN = 16;
const int CLK_PIN = 13;

//signs for matrix 8x8
const uint64_t rightM = {0x10387cfe38383838};
const uint64_t leftM = {0x38383838fe7c3810};
const uint64_t upM = {0x0010307fff7f3010};
const uint64_t downM = {0x00080cfefffe0c08};
const uint64_t nearM = {0x0018187e7e181800};
const uint64_t farM = {0x0018181818181800};
const uint64_t noneM = {0x0060f09b8bc04000};

LedControl display = LedControl(DIN_PIN, CLK_PIN, CS_PIN);

void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      display.setLed(0, i, j, bitRead(row, j));
    }
  }
}


// Pins
#define APDS9960_INT    2 // Needs to be an interrupt pin

// Constants

// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

int lever_flag = 0;

void setup() {

  display.clearDisplay(0);
  display.shutdown(0, false);
  display.setIntensity(0, 10);
  
  // Set interrupt pin as input
  pinMode(APDS9960_INT, INPUT);

  // Initialize Serial port
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("APDS-9960 - GestureTest"));
  Serial.println(F("--------------------------------"));
  
  // Initialize interrupt service routine
  attachInterrupt(0, interruptRoutine, FALLING);

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }

  pinMode(proxIR,INPUT);
    
  myservo6.attach(9);   //6
//  myservo5.attach(5);
  
  pinMode(LmotorPWM, OUTPUT);
  pinMode(LmotorDIR, OUTPUT);
  digitalWrite(LmotorPWM, LOW);
  digitalWrite(LmotorDIR, LOW);
  pinMode(RmotorPWM, OUTPUT);
  pinMode(RmotorDIR, OUTPUT);
  digitalWrite(RmotorPWM, LOW);
  digitalWrite(RmotorDIR, LOW);

  myservo6.write(val6);

}

void loop() {
  if( isr_flag == 1 ) {
    detachInterrupt(0);
    handleGesture();
    isr_flag = 0;
    attachInterrupt(0, interruptRoutine, FALLING);
  }

  int diff;
  int diff2;
  float prop;

  
if (  digitalRead(proxIR) == false)
  {
  myservo6.write(0);
  }
else
  {
  myservo6.write(180);
  }
  delay(100);

}

void interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
    if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        Serial.println("UP");
        displayImage(upM); myservo6.write(0);
        break;
      case DIR_DOWN:
        Serial.println("DOWN");
        displayImage(downM); myservo6.write(180);
        break;
      case DIR_LEFT:
        Serial.println("LEFT");
        displayImage(leftM);
        break;
      case DIR_RIGHT:
        Serial.println("RIGHT");
        displayImage(rightM);
        break;
      case DIR_NEAR:
        Serial.println("NEAR");
        displayImage(nearM);
        break;
      case DIR_FAR:
        Serial.println("FAR");
        displayImage(farM);
        break;
      default:
        Serial.println("NONE");
        displayImage(noneM);
    }
  }
}
