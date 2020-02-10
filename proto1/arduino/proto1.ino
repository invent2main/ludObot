#include <NRF24_RX.h>
#include <nrf24_rx_cx10.h>
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

// NRF24L01 pins
static const int CE_PIN = 10;
static const int CSN_PIN = 8;

// (USE_CX10)
CX10_RX nrf24(CE_PIN, CSN_PIN);
NRF24_RX *rx = &nrf24;
static const int protocol = NRF24_RX::CX10A;
static const int rcChannelCount = CX10_RX::RC_CHANNEL_COUNT;

uint16_t nrf24RcData[19];

//motors
int LmotorPWM = 3;
int LmotorDIR = 4;
int RmotorPWM = 6; //9;
int RmotorDIR = 7;

void setup()
{
  rx->begin(protocol);
  pinMode(CE_PIN, OUTPUT);
  pinMode(CSN_PIN, OUTPUT);

  myservo6.attach(9);   //6
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
  
  nrf24RcData[NRF24_PITCH]=1500;
  nrf24RcData[NRF24_YAW]=1500;   
  nrf24RcData[NRF24_ROLL]=1500;

  Serial.begin(57600);
  Serial.println("");
  Serial.print("Receiver Starting, protocol: ");
  Serial.println(String(rx->getProtocolString()));
  Serial.println("");
  delay(100);
}

void printRcData()
{
  Serial.println("");
/*
  Serial.print("Phase=");
  Serial.print(nrf24RcData[Phase]);
  Serial.print(",");
  Serial.print("CID1=");
  Serial.print(nrf24RcData[CID1]);
  Serial.print(",");
  Serial.print("CID2=");
  Serial.print(nrf24RcData[CID2]);
  Serial.print(",");
  Serial.print("CID3=");
  Serial.print(nrf24RcData[CID3]);
  Serial.print(",");
  Serial.print("CID4=");
  Serial.print(nrf24RcData[CID4]);
  Serial.print(",");
  Serial.print("VID1=");
  Serial.print(nrf24RcData[VID1]);
  Serial.print(",");
  Serial.print("VID2=");
  Serial.print(nrf24RcData[VID2]);
  Serial.print(",");
  Serial.print("VID3=");
  Serial.print(nrf24RcData[VID3]);
  Serial.print(",");
  Serial.print("VID4=");
  Serial.print(nrf24RcData[VID4]);
  Serial.print(",");  
*/    
  Serial.print("turn="); //(1000-1500-2000)LATCHED@1500
  Serial.print(nrf24RcData[NRF24_ROLL]);
  Serial.print(",");
  Serial.print("moveFB="); //(1000-1500-2000)LATCHED@1500
  Serial.print(nrf24RcData[NRF24_PITCH]);
  Serial.print(",");
  Serial.print("up-down="); //(970-(1432)-1938)
  Serial.print(nrf24RcData[NRF24_THROTTLE]);
  Serial.print(",");
  Serial.print("clamp="); //(1000-1500-2000)LATCHED@1500
  Serial.print(nrf24RcData[NRF24_YAW]);
}

void loop()
{
  int diff;
  int diff2;
  float prop;
  
  const NRF24_RX::received_e dataReceived = rx->dataReceived();
  if (dataReceived == NRF24_RX::RECEIVED_DATA) {
    rx->setRcDataFromPayload(nrf24RcData);
 //   printRcData();
  }

//  delay(1);
  if (Timon == 0) { timer1_counter++; if (timer1_counter>=200){ Timon = 1; timer1_counter=0; }

    
  }

  if (nrf24RcData[NRF24_THROTTLE] != THROTTLE_OLD){ 
  
  diff = nrf24RcData[NRF24_THROTTLE];  
  diff = map(diff, 970, 1938, 18, 118);
  myservo5.write(diff);
//    Serial.println(diff); 
  THROTTLE_OLD = nrf24RcData[NRF24_THROTTLE];  
  }

  
  if (nrf24RcData[NRF24_YAW] < 1450){     //open
    
//    clamp_state = 128;
      if (Timon == 1) { diff_clamp = (1500-nrf24RcData[NRF24_YAW])/50 ;  val6=val6-(diff_clamp);  val6 = constrain(val6, 0, 180);  myservo6.write(val6); Timon = 0;}

//  Serial.println(""); Serial.println(val6); Serial.println(diff_clamp);  Serial.println("open");
  }

  else if (nrf24RcData[NRF24_YAW] > 1550){    //close
    
 //   clamp_state = 248;

      if (Timon == 1) {  diff_clamp = (nrf24RcData[NRF24_YAW]-1500)/50 ; val6=val6+(diff_clamp);  val6 = constrain(val6, 0, 180); myservo6.write(val6); Timon = 0;}           
// Serial.println(""); Serial.println(val6); Serial.println(diff_clamp);  Serial.println("close");
  }

  
  if ((nrf24RcData[NRF24_PITCH] < 1500) && (nrf24RcData[NRF24_ROLL] == 1500)) {     //backward
    diff = (1500-nrf24RcData[NRF24_PITCH]) ;
    if (diff >= 250) {  digitalWrite(LmotorPWM, HIGH); digitalWrite(RmotorPWM, HIGH); }
    else { analogWrite(LmotorPWM, diff); analogWrite(RmotorPWM, diff);}
    digitalWrite(LmotorDIR, LOW);
    digitalWrite(RmotorDIR, LOW);
  }

  else if ((nrf24RcData[NRF24_PITCH] > 1500) && (nrf24RcData[NRF24_ROLL] == 1500)) {    //forward
    diff = (nrf24RcData[NRF24_PITCH]-1500)/2 ;
    if (diff >= 250) {  digitalWrite(LmotorPWM, LOW); digitalWrite(RmotorPWM, LOW);}
    else { diff=250-diff; analogWrite(LmotorPWM, diff); analogWrite(RmotorPWM, diff);}
    digitalWrite(LmotorDIR, HIGH);
    digitalWrite(RmotorDIR, HIGH);  
  }
    
  else if ((nrf24RcData[NRF24_ROLL] < 1500) && (nrf24RcData[NRF24_PITCH] == 1500)) {     //left
    diff = (1500-nrf24RcData[NRF24_ROLL])/2 ;
    if (diff >= 250) { diff=250; }
//      digitalWrite(LmotorPWM, HIGH); digitalWrite(RmotorPWM, LOW); }
//    else { analogWrite(LmotorPWM, diff);
    diff=250-diff; analogWrite(RmotorPWM, diff);//}
    digitalWrite(LmotorPWM, LOW);
    digitalWrite(LmotorDIR, LOW);
    
    digitalWrite(RmotorDIR, HIGH);
  }

  else if ((nrf24RcData[NRF24_ROLL] > 1500) && (nrf24RcData[NRF24_PITCH] == 1500)) {    //right
    diff = (nrf24RcData[NRF24_ROLL]-1500)/2 ;
    if (diff >= 250) { diff=250; }
 //   if (diff >= 250) {  digitalWrite(LmotorPWM, LOW); digitalWrite(RmotorPWM, HIGH);}
 //   else { analogWrite(RmotorPWM, diff);
 diff=250-diff;
    analogWrite(LmotorPWM, diff);//}
    digitalWrite(LmotorDIR, HIGH);
    
    digitalWrite(RmotorPWM, LOW);
    digitalWrite(RmotorDIR, LOW);  
  }

  else if ((nrf24RcData[NRF24_PITCH] > 1500) && (nrf24RcData[NRF24_ROLL] < 1500)) {    //forward & left
    diff = (nrf24RcData[NRF24_PITCH]-1500) ;
    if (diff >= 250) { diff=250; }
//    digitalWrite(RmotorPWM, LOW); }
//    else {
      diff=250-diff; analogWrite(RmotorPWM, diff);// }
    diff2 = (1500-nrf24RcData[NRF24_ROLL])/2 ; 
    if (diff2 >= 125) { diff2=125; }
//    digitalWrite(LmotorPWM, HIGH); }
//    else {
//      diff = (nrf24RcData[NRF24_PITCH]-1500); diff = constrain(diff, 0, 250); prop=(10-((diff2*5)/250)); prop=(diff*prop)/10; diff=(int)prop; diff=250-diff; analogWrite(LmotorPWM, diff);//  } 
   // diff2=(250-diff2);
    diff2 = constrain(diff2, diff, 250);
    analogWrite(LmotorPWM, diff2);
    digitalWrite(LmotorDIR, HIGH);
    digitalWrite(RmotorDIR, HIGH);  
  }

  else if ((nrf24RcData[NRF24_PITCH] > 1500) && (nrf24RcData[NRF24_ROLL] > 1500)) {    //forward & right
    diff = (nrf24RcData[NRF24_PITCH]-1500) ;
    if (diff >= 250) { diff=250; } 
 //   digitalWrite(LmotorPWM, LOW); }
 //   else {
    diff=250-diff;
    analogWrite(LmotorPWM, diff);// }
    diff2 = (nrf24RcData[NRF24_ROLL]-1500)/2 ;
    if (diff2 >= 125) { diff2=125; }
 //   digitalWrite(RmotorPWM, HIGH); }
 //   else {
 //     diff = (nrf24RcData[NRF24_PITCH]-1500); diff = constrain(diff, 0, 250); prop=(10-((diff2*5)/250)); prop=(diff*prop)/10; diff=(int)prop; diff=250-diff; analogWrite(RmotorPWM, diff); // } 
//    }
   // diff2=(250-diff2);
    diff2 = constrain(diff2, diff, 250);
    analogWrite(RmotorPWM, diff2);
    digitalWrite(LmotorDIR, HIGH);
    digitalWrite(RmotorDIR, HIGH);  
  }

  else if ((nrf24RcData[NRF24_PITCH] < 1500) && (nrf24RcData[NRF24_ROLL] < 1500)) {     //backward & left
    diff = (1500-nrf24RcData[NRF24_PITCH]) ;

    if (diff >= 250) {  digitalWrite(LmotorPWM, HIGH); }
    else { analogWrite(LmotorPWM, diff); }
    diff2 = (1500-nrf24RcData[NRF24_ROLL]) ;
    if (diff2 >= 250) {  digitalWrite(RmotorPWM, LOW); }
    else {diff = (nrf24RcData[NRF24_PITCH]-1500); diff = constrain(diff, 0, 250); prop=(10-((diff2*5)/250)); prop=(diff*prop)/10; diff=(int)prop; analogWrite(RmotorPWM, diff);  } 
   
    digitalWrite(LmotorDIR, LOW);
    digitalWrite(RmotorDIR, LOW);
  }

  else if ((nrf24RcData[NRF24_PITCH] < 1500) && (nrf24RcData[NRF24_ROLL] > 1500)) {     //backward & right
    diff = (1500-nrf24RcData[NRF24_PITCH]) ;
    if (diff >= 250) {  digitalWrite(RmotorPWM, HIGH); }
    else {  analogWrite(RmotorPWM, diff);}
    diff2 = (nrf24RcData[NRF24_ROLL]-1500) ; 
    if (diff2 >= 250) {  digitalWrite(LmotorPWM, LOW); }
    else {diff = (nrf24RcData[NRF24_PITCH]-1500); diff = constrain(diff, 0, 250); prop=(10-((diff2*5)/250)); prop=(diff*prop)/10; diff=(int)prop; analogWrite(LmotorPWM, diff);  } 
    
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

