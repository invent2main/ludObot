/**
 * Exemple de code pour la télécommande nRF24 LudOcom
 */
#include <SPI.h>      // Pour la communication via le port SPI
#include <Mirf.h>     // Pour la gestion de la communication
#include <nRF24L01.h> // Pour les définitions des registres du nRF24L01
#include <MirfHardwareSpiDriver.h> // Pour la communication SPI

int OpenClosePin = A3;
int UpDownPin = A2;
int LeftRightPin = A6;
int FrontRearPin = A7;

int OCUDpushPin = A1;
int FRLRpushPin = A0;
int pushLsPin = 4;
int pushRsPin = 3;

int LedPin = 2;

int pushUpPin = 5;
int pushDownPin = 9;
int pushLeftPin = 7;
int pushRightPin = 6;
 
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
   
  Mirf.cePin = 10; // Broche CE sur D9
  Mirf.csnPin = 8; // Broche CSN sur D10
  Mirf.spi = &MirfHardwareSpi; // On veut utiliser le port SPI hardware
  Mirf.init(); // Initialise la bibliothèque

  Mirf.channel = 1; // Choix du canal de communication (128 canaux disponibles, de 0 à 127)
  Mirf.payload = sizeof(MaStructure); // Taille d'un message (maximum 32 octets)
  Mirf.config(); // Sauvegarde la configuration dans le module radio

  Mirf.setTADDR((byte *) "nrf06"); // Adresse de transmission
  Mirf.setRADDR((byte *) "nrf05"); // Adresse de réception

  pinMode(pushLsPin, INPUT_PULLUP);
  pinMode(pushRsPin, INPUT_PULLUP);

  pinMode(pushLeftPin, INPUT_PULLUP);
  pinMode(pushRightPin, INPUT_PULLUP);
  pinMode(pushUpPin, INPUT_PULLUP);
  pinMode(pushDownPin, INPUT_PULLUP);
  pinMode(OCUDpushPin, INPUT);
  pinMode(FRLRpushPin, INPUT);
    
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, HIGH);    
//  Serial.println("Go !"); 
}
 
void loop() {
  MaStructure message;
//  MaStructure oldmess;

//  delay(10);
  
  message.FrontRear = analogRead(FrontRearPin)>>1;
  message.LeftRight = analogRead(LeftRightPin)>>1;
  message.UpDown = analogRead(UpDownPin)>>1;
  message.OpenClose = analogRead(OpenClosePin)>>1;


  if ( digitalRead(pushLsPin) == false)  {  message.pushLs=255;  }
  else  {  message.pushLs=0;  }
  if ( digitalRead(pushRsPin) == false)  {  message.pushRs=255;  }
  else  {  message.pushRs=0;  }
  if ( digitalRead(pushLeftPin) == false)  {  message.pushLeft=255;  }
  else  {  message.pushLeft=0;  }  
  if ( digitalRead(pushRightPin) == false)  {  message.pushRight=255;  }
  else  {  message.pushRight=0;  }  
  if ( digitalRead(pushUpPin) == false)  {  message.pushUp=255;  }
  else  {  message.pushUp=0;  }  
  if ( digitalRead(pushDownPin) == false)  {  message.pushDown=255;  }
  else  {  message.pushDown=0;  }  

  if ( digitalRead(OCUDpushPin) == false)  {  message.OCUDpush=255;  }
  else  {  message.OCUDpush=0;  }  
  if ( digitalRead(FRLRpushPin) == false)  {  message.FRLRpush=255;  }
  else  {  message.FRLRpush=0;  }  
    
  Mirf.send((byte*) &message); // On envoie le message
  while(Mirf.isSending()); // On attend la fin de l'envoi

} 
