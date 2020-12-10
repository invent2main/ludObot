/**
 * Exemple de code pour utiliser le micro de la LudOhead avec une carte LudOcore: compte le nombre le clap, max 9
 */
#include <Wire.h>
#include <LedControl.h>

const int DIN_PIN = 12;
const int CS_PIN = 16;
const int CLK_PIN = 13;

const uint64_t blank = {0x0000000000000000};
const uint64_t zero = {0x00007e8181817e00};
const uint64_t one = {0x00000001ff410000};
const uint64_t two = {0x0000718985834100};
const uint64_t three = {0x00006e9191814200};
const uint64_t four = {0x0000ff4424140c00};
const uint64_t five = {0x00008e919191f200};
const uint64_t six = {0x00004e9191917e00};
const uint64_t seven = {0x0000e09088878000};
const uint64_t eight = {0x00006e9191916e00};
const uint64_t nine = {0x00007e8989897200};

LedControl display = LedControl(DIN_PIN, CLK_PIN, CS_PIN);

void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      display.setLed(0, i, j, bitRead(row, j));
    }
  }
}

int micPin = 7;                             // analog pin to sample level from amp

int threshold;
int level;
unsigned long prevmillis, currentmillis;    // used to call AveBGSoundLevel() once per minute

int clapcount;                              // a var to keep count of claps in ...
int dctimeout = 2;                          // ... a given timeframe. 1 second

/****************************************/

void setup() 
{

  display.clearDisplay(0);
  display.shutdown(0, false);
  display.setIntensity(0, 10);
  
  pinMode(micPin, INPUT); 

  Serial.begin(115200);                     // open serial port at a fast rate so to make minimal interfere with timing
  
  Serial.println("Running... ");
  delay(500);                               // let things settle down

  FindAveBGLevel();
  
  prevmillis = millis();                    // get a time ref 
        
}  // End of setup()

/****************************************/
 
void loop() 
{
  int i;
  
  level = analogRead(micPin);
  
  if (level > threshold)                    // we have a sample above the threshold
    {
    clapcount = 1;  
    displayImage(one);
      Serial.println("one");
    /*    
    digitalWrite(LED1, LOW);
    digitalWrite(RelayPin, HIGH);
    */
    
    delay(40);                              // delay to let echo's go away and set min time between double/triple claps

    for (i = 0; i < 500 * dctimeout; i++)  // make a number of attempts to detect another clap
      {
      level = analogRead(micPin);

      if (level > threshold)                // we have a sample above the threshold
        {
        clapcount += 1;
        delay(40);
          
        if (clapcount == 2)
          {
          displayImage(two); //digitalWrite(LED2, LOW);
          Serial.println("two");
          delay(40);
          }
          
        if (clapcount == 3)
          {
          displayImage(three); //digitalWrite(LED3, LOW);
          Serial.println("three");
          delay(40);
          }

        if (clapcount == 4)
          {
          displayImage(four);
          Serial.println("four");
          delay(40);
          }

         if (clapcount == 5)
          {
          displayImage(five);
          Serial.println("five");
          delay(40);
          }

        if (clapcount == 6)
          {
          displayImage(six);
          Serial.println("six");
          delay(40);
          }

         if (clapcount == 7)
          {
          displayImage(seven);
          Serial.println("seven");
          delay(40);
          }

        if (clapcount == 8)
          {
          displayImage(eight);
          Serial.println("eight");
          delay(40);
          }

         if (clapcount == 9)
          {
          displayImage(nine);
          Serial.println("nine");
          delay(40);
          }

          
      
      }    
      delay(2);                             // set to X millisecs between samples
    }

    clapcount = 0; 
    displayImage(blank);
    Serial.println("zero");
    }  
  else  
    {
    delay(1);                               // short delay between samples, unless a loud sample is detected above
    }

  // determine if has been 1 minute since last recalibration of background noise
  currentmillis = millis();
  if (currentmillis > prevmillis + 60000)
    {
    prevmillis = currentmillis;            // keep prev time
    FindAveBGLevel();                      // recalibrate sound level  
    }
  
} // End of loop()

/****************************************/

// Func to determine average background sound level. This is done at prog start then once per minute

void FindAveBGLevel()
{
  int i;
  unsigned long sum = 0;

  displayImage(zero);
  Serial.print("...calculating background sound level...  ");

  for (i = 0; i < 1000; i++)               // take 1000 samples in 1 sec
    {
    sum += analogRead(micPin);             // an analogRead() takes 100us but timing is not critical at this point
    delay(1);                                 
    }

  threshold = int((sum/1000) * 1.35);      // determine average of the samples then add a fudge factor

  mySPL("threshold set at ", threshold);
  
  displayImage(blank);
  
} // End of FindAveBGLevel()

/****************************************/

// So sick of multiple statements just to print a var and its value. This does it in 1

void mySPL(String p1, unsigned long p2)
{
  
  Serial.print(p1);
  Serial.println(p2);
  
} // End of mySerialprintln() 

/****************************************/
