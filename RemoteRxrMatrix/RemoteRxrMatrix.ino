#include <TimerOne.h>

const int Relay_1 = 2;
const int Relay_2 = 3;
const int Relay_3 = 4;
const int Relay_4 = 5;
const int Relay_5 = 6;
const int Relay_6 = 7;
const int Relay_7 = 8;
const int Relay_8 = 9;
const int Relay_9 = 10;
const int Relay_10 = 11;
const int Relay_11 = 12;
const int Relay_12 = 13;
const int Relay_RED = A0;
const int Relay_YELLOW = A1;
const int Relay_BLUE = A2;
const int Relay_STOP = A3;
const int Buzzer = A4;

byte Matrix_Relay = 0x00;
byte Latch_Relay = 0x00;
byte receivedCode = 0x00;
boolean comFlag = false;

void allPinsDown()
  {
    digitalWrite(Relay_1,HIGH);
    digitalWrite(Relay_2,HIGH);
    digitalWrite(Relay_3,HIGH);
    digitalWrite(Relay_4,HIGH);
    digitalWrite(Relay_5,HIGH);
    digitalWrite(Relay_6,HIGH);
    digitalWrite(Relay_7,HIGH);
    digitalWrite(Relay_8,HIGH);
    digitalWrite(Relay_9,HIGH);
    digitalWrite(Relay_10,HIGH);
    digitalWrite(Relay_11,HIGH);
    digitalWrite(Relay_12,HIGH);
  }

void setup()
  {
    pinMode(Relay_1,OUTPUT);
    pinMode(Relay_2,OUTPUT);
    pinMode(Relay_3,OUTPUT);
    pinMode(Relay_4,OUTPUT);
    pinMode(Relay_5,OUTPUT);
    pinMode(Relay_6,OUTPUT);
    pinMode(Relay_7,OUTPUT);
    pinMode(Relay_8,OUTPUT);
    pinMode(Relay_9,OUTPUT);
    pinMode(Relay_10,OUTPUT);
    pinMode(Relay_11,OUTPUT);
    pinMode(Relay_12,OUTPUT);
    pinMode(Relay_RED,OUTPUT);
    pinMode(Relay_YELLOW,OUTPUT);
    pinMode(Relay_BLUE,OUTPUT);
    pinMode(Relay_STOP,OUTPUT);
    pinMode(Buzzer,OUTPUT);
    
    allPinsDown();
    digitalWrite(Relay_RED,HIGH);
    digitalWrite(Relay_YELLOW,HIGH);
    digitalWrite(Relay_BLUE,HIGH);
    digitalWrite(Relay_STOP,HIGH);
    digitalWrite(Buzzer,LOW);
    
    Serial.begin(9600);
    
    Timer1.initialize(2000000); // set a timer of length 1000000 microseconds (or 1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
    Timer1.attachInterrupt( timerIsr ); // attach the service routine here
  }
  
void loop()
  {
    
  }
  
void serialEvent() {
    while (Serial.available()) {
        // get the new byte:
        receivedCode = Serial.read();
        Serial.print(receivedCode);
        comFlag = true;
        Matrix_Relay = receivedCode & 0xF0;
        Matrix_Relay = Matrix_Relay >> 4;
        Latch_Relay = receivedCode & 0x0F;
        
        if(Matrix_Relay == 0x00)
          allPinsDown();
        else
          digitalWrite(Matrix_Relay,LOW);
          
        if((Latch_Relay & 0x01) == 0x01)
          digitalWrite(Relay_RED, LOW);
        else
          digitalWrite(Relay_RED, HIGH);
        
        if((Latch_Relay & 0x02) == 0x02)
          digitalWrite(Relay_YELLOW, LOW);
        else
          digitalWrite(Relay_YELLOW, HIGH);
          
        if((Latch_Relay & 0x04) == 0x04)
          digitalWrite(Relay_BLUE, LOW);
        else
          digitalWrite(Relay_BLUE, HIGH);
          
        if((Latch_Relay & 0x08) == 0x08)
          digitalWrite(Relay_STOP, LOW);
        else
          digitalWrite(Relay_STOP, HIGH);
     }
  }
  
void timerIsr()
{
    if(comFlag == false)
      digitalWrite(Buzzer,HIGH);
    if(comFlag == true)
      digitalWrite(Buzzer, LOW);
    comFlag = false;
}
