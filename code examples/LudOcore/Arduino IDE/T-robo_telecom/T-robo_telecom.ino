/**
 * Exemple de code pour piloter un T-robo à l'aide de la télécommande nRF24
 */
#include <SPI.h>      // Pour la communication via le port SPI
#include <Mirf.h>     // Pour la gestion de la communication
#include <nRF24L01.h> // Pour les définitions des registres du nRF24L01
#include <MirfHardwareSpiDriver.h> // Pour la communication SPI

#include <Servo.h>

bool initstate = 0;
bool Timon = 1;
int timer1_counter = 0;

Servo myservo6;
int val6 = 90;  // clamp
unsigned int diff_clamp;
unsigned int clamp_state = 0;

Servo myservo5;
int val5 = 98;  // lever
uint16_t THROTTLE_OLD;

//motors
int LmotorPWM = 3;
int LmotorDIR = 4;
int RmotorPWM = 6; //9;
int RmotorDIR = 7;

typedef struct {
  char OCUDpush;
  char FRLRpush;  
  char pushLs;
  char pushRs; 
  char pushUp;
  char pushDown;
  char pushLeft;
  char pushRight; 
  int FrontRear;
  int LeftRight;
  int UpDown;
  int OpenClose;
} MaStructure;

void setup() {
//  Serial.begin(57600);

  Mirf.cePin = 14; //10; // Broche CE sur D9
  Mirf.csnPin = 8; // Broche CSN sur D10
  Mirf.spi = &MirfHardwareSpi; // On veut utiliser le port SPI hardware
  Mirf.init(); // Initialise la bibliothèque

  Mirf.channel = 1; // Choix du canal de communication (128 canaux disponibles, de 0 à 127)
  Mirf.payload = sizeof(MaStructure); // Taille d'un message (maximum 32 octets)
  Mirf.config(); // Sauvegarde la configuration dans le module radio

  Mirf.setTADDR((byte *) "nrf03"); // Adresse de transmission
  Mirf.setRADDR((byte *) "nrf04"); // Adresse de réception

  myservo6.attach(9);
  myservo5.attach(5);
  
 
  pinMode(LmotorPWM, OUTPUT);
  pinMode(LmotorDIR, OUTPUT);
  digitalWrite(LmotorPWM, LOW);
  digitalWrite(LmotorDIR, LOW);
  pinMode(RmotorPWM, OUTPUT);
  pinMode(RmotorDIR, OUTPUT);
  digitalWrite(RmotorPWM, LOW);
  digitalWrite(RmotorDIR, LOW);

  myservo6.write(val6);
  myservo5.write(val5);

//  Serial.println("Go !"); 
}

void loop() {
  int diff;
  int diff2;
  float prop;
  
  MaStructure message;  

 /*   
  Serial.println("");
  Serial.print("FrontRear="); // Affiche le message
  Serial.print(message.FrontRear);
  Serial.print(" LeftRight=");
  Serial.println(message.LeftRight);
  Serial.print(" UpDown=");
  Serial.println(message.UpDown);  
  Serial.print(" OpenClose=");
  Serial.println(message.OpenClose);
*/
    
  if(initstate==0) {
  message.OpenClose=255;
  message.UpDown=255;
  message.LeftRight=255;
  message.FrontRear=255; 
  digitalWrite(LmotorPWM, LOW);
  digitalWrite(LmotorDIR, LOW);
  digitalWrite(RmotorPWM, LOW);
  digitalWrite(RmotorDIR, LOW);   
  initstate=1;
  Serial.println("");
  Serial.print("FrontRear="); // Affiche le message
  Serial.print(message.FrontRear);
  Serial.print(" LeftRight=");
  Serial.println(message.LeftRight);
  Serial.print(" UpDown=");
  Serial.println(message.UpDown);  
  Serial.print(" OpenClose=");
  Serial.println(message.OpenClose);
  }
  else{

  if(Mirf.dataReady()){
    Mirf.getData((byte*) &message); // Réception du paquet
  }

  if (Timon == 0) { timer1_counter++; if (timer1_counter>=200){ Timon = 1; timer1_counter=0; }}

  if (message.UpDown != THROTTLE_OLD){ 
  
    diff = message.UpDown;  
    diff = map(diff, 0, 512, 18, 128);
    myservo5.write(diff);
//    Serial.println(diff); 
    THROTTLE_OLD = message.UpDown;  
  }

  
  if (message.OpenClose < 240){     //open
    
//    clamp_state = 128;
      if (Timon == 1) { diff_clamp = (260-message.OpenClose)/20 ;  val6=val6-(diff_clamp);  val6 = constrain(val6, 0, 180);  myservo6.write(val6); Timon = 0;}

//  Serial.println(""); Serial.println(val6); Serial.println(diff_clamp);  Serial.println("open");
  }

  else if (message.OpenClose > 270){    //close
    
 //   clamp_state = 248;

      if (Timon == 1) {  diff_clamp = (message.OpenClose-250)/20 ; val6=val6+(diff_clamp);  val6 = constrain(val6, 0, 180); myservo6.write(val6); Timon = 0;}           
// Serial.println(""); Serial.println(val6); Serial.println(diff_clamp);  Serial.println("close");
  }

  
  if ((message.FrontRear < 240) && (message.LeftRight > 240) && (message.LeftRight < 270)) {     //backward
    diff = (240-message.FrontRear) ;
    if (diff >= 125) {  digitalWrite(LmotorPWM, HIGH); digitalWrite(RmotorPWM, HIGH); }
    else { analogWrite(LmotorPWM, diff); analogWrite(RmotorPWM, diff);}
    digitalWrite(LmotorDIR, LOW);
    digitalWrite(RmotorDIR, LOW);
  }

  else if ((message.FrontRear > 270) && (message.LeftRight > 240) && (message.LeftRight < 270)) {    //forward
    diff = (message.FrontRear-270);
    if (diff >= 125) {  digitalWrite(LmotorPWM, LOW); digitalWrite(RmotorPWM, LOW);}
    else { diff=270-diff; analogWrite(LmotorPWM, diff); analogWrite(RmotorPWM, diff);}
    digitalWrite(LmotorDIR, HIGH);
    digitalWrite(RmotorDIR, HIGH);  
  }
    
  else if ((message.LeftRight < 240) && (message.FrontRear > 240) && (message.FrontRear < 270)) {     //left
    diff = (240-message.LeftRight);
    diff=240-diff; analogWrite(RmotorPWM, diff);//}
    digitalWrite(LmotorPWM, LOW);
    digitalWrite(LmotorDIR, LOW);
    
    digitalWrite(RmotorDIR, HIGH);
  }

  else if ((message.LeftRight > 270) && (message.FrontRear > 240) && (message.FrontRear < 270)) {    //right
    diff = (message.LeftRight-270);
    diff=270-diff;
    analogWrite(LmotorPWM, diff);//}
    digitalWrite(LmotorDIR, HIGH);
    
    digitalWrite(RmotorPWM, LOW);
    digitalWrite(RmotorDIR, LOW);  
  }

  else if ((message.FrontRear > 270) && (message.LeftRight < 240)) {    //forward & left
    diff = (message.FrontRear-270) ;

      diff=270-diff; analogWrite(RmotorPWM, diff);// }
    diff2 = (240-message.LeftRight); 
    if (diff2 >= 125) { diff2=125; }
//    digitalWrite(LmotorPWM, HIGH); }
//    else {
//      diff = (message.FrontRear-1500); diff = constrain(diff, 0, 250); prop=(10-((diff2*5)/250)); prop=(diff*prop)/10; diff=(int)prop; diff=250-diff; analogWrite(LmotorPWM, diff);//  } 
//      diff2=(250-diff2);
    diff2 = constrain(diff2, diff, 250);
    analogWrite(LmotorPWM, diff2);
    digitalWrite(LmotorDIR, HIGH);
    digitalWrite(RmotorDIR, HIGH);  
  }

  else if ((message.FrontRear > 270) && (message.LeftRight > 270)) {    //forward & right
    diff = (message.FrontRear-270) ;

    diff=270-diff;
    analogWrite(LmotorPWM, diff);// }
    diff2 = (message.LeftRight-270) ;
    if (diff2 >= 125) { diff2=125; }
//   digitalWrite(RmotorPWM, HIGH); }
//   else {
//    diff = (message.FrontRear-1500); diff = constrain(diff, 0, 250); prop=(10-((diff2*5)/250)); prop=(diff*prop)/10; diff=(int)prop; diff=250-diff; analogWrite(RmotorPWM, diff); // } 
//    }
   // diff2=(250-diff2);
    diff2 = constrain(diff2, diff, 250);
    analogWrite(RmotorPWM, diff2);
    digitalWrite(LmotorDIR, HIGH);
    digitalWrite(RmotorDIR, HIGH);  
  }

  else if ((message.FrontRear < 240) && (message.LeftRight < 240)) {     //backward & left
    diff = (240-message.FrontRear) ;

    if (diff >= 125) {  digitalWrite(LmotorPWM, HIGH); }
    else { analogWrite(LmotorPWM, diff); }
    diff2 = (240-message.LeftRight) ;
    if (diff2 >= 125) {  digitalWrite(RmotorPWM, LOW); }
    else {diff = (message.FrontRear-240); diff = constrain(diff, 0, 250); prop=(10-((diff2*5)/250)); prop=(diff*prop)/10; diff=(int)prop; analogWrite(RmotorPWM, diff);  } 
   
    digitalWrite(LmotorDIR, LOW);
    digitalWrite(RmotorDIR, LOW);
  }

  else if ((message.FrontRear < 240) && (message.LeftRight > 270)) {     //backward & right
    diff = (240-message.FrontRear) ;
    if (diff >= 125) {  digitalWrite(RmotorPWM, HIGH); }
    else {  analogWrite(RmotorPWM, diff);}
    diff2 = (message.LeftRight-270) ; 
    if (diff2 >= 125) {  digitalWrite(LmotorPWM, LOW); }
    else {diff = (message.FrontRear-240); diff = constrain(diff, 0, 250); prop=(10-((diff2*5)/250)); prop=(diff*prop)/10; diff=(int)prop; analogWrite(LmotorPWM, diff);  } 
    
    digitalWrite(LmotorDIR, LOW);
    digitalWrite(RmotorDIR, LOW);
  }
      
  else{
  digitalWrite(LmotorPWM, LOW);
  digitalWrite(LmotorDIR, LOW);
  digitalWrite(RmotorPWM, LOW);
  digitalWrite(RmotorDIR, LOW);  
  }
  }  
}
