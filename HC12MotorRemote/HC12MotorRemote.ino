#include <Bounce2.h>

const int onOffButton = 2;
const int powerLed = 3;
const int motorLed = 4;
const int commLed = 5;

// Instantiate a de-bounce object
Bounce onOffDebouncer = Bounce();

int waitCount = 0;
int powerStatus = 0;
int motorStatus = 0;

void setup() 
  {
    Serial.begin(1200);
    
    pinMode(powerLed, OUTPUT);
    pinMode(motorLed, OUTPUT);
    pinMode(commLed, OUTPUT);
    pinMode(onOffButton, INPUT);

    digitalWrite(powerLed, LOW);
    digitalWrite(motorLed, LOW);
    digitalWrite(commLed, LOW);
    
    onOffDebouncer.attach(onOffButton);
    onOffDebouncer.interval(5);

    delay(2000);
    
    //Send query after every 5 seconds until current status is not received from at motor end
    Serial.print('#');
    while(Serial.available() <= 0)
      {
        delay(1);
        waitCount++;
        if(waitCount > 5000)
          {
            waitCount = 0;
            Serial.print('#');
          }
      }
    digitalWrite(commLed, HIGH);
    
    while(Serial.available()) 
      {
        // get the new byte:
        byte inChar = Serial.read();
        parseData(inChar);
      }
  }

void loop() 
  {
    onOffDebouncer.update();
    int value = onOffDebouncer.read();
    if(value == HIGH)
      {
        waitCount = 0;
        digitalWrite(commLed, LOW);
        Serial.print('@');
        while(Serial.available() <= 0)
          {
            delay(1);
            waitCount++;
            if(waitCount > 10000)
              {
                waitCount = 0;
                Serial.print('@');
              }
          }
        digitalWrite(commLed, HIGH);
        while(Serial.available()) 
          {
            // get the new byte:
            byte inChar = Serial.read();
            parseData(inChar);
          }
        while(value == HIGH)
          {
            onOffDebouncer.update();
            value = onOffDebouncer.read();
          }
      }
  }

void parseData(byte motorData)
  {
     powerStatus = motorData & 0x01;
     motorStatus = motorData & 0x02;

     if(powerStatus == 1)
      digitalWrite(powerLed, HIGH);
     else
      digitalWrite(powerLed, LOW);

     if(motorStatus == 2)
      digitalWrite(motorLed, HIGH);
     else
      digitalWrite(motorLed, LOW);
  }
