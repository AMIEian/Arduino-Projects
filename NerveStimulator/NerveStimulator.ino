//Primary = 50 Turns / 0.2mm Gage (Diameter) / 0.16mH
//Secondary = 1000 Turns / 0.1mm Gage (Diameter) / 133mH
//Core Size = EE14
//Transistor = C2655 (NPN)

#include <ESP8266WiFi.h>
#include <Nextion.h>
#include <Ticker.h>

extern "C" {
#include "user_interface.h"
}

#define NODEMCU_FREQUENCY 160 //Valid 80, 160 MHz
#define nextion Serial  //HW Serial

Nextion myNextion(nextion, 9600); //create a Nextion object named myNextion using the nextion serial port @ 9600bps
String message = "";
Ticker ticker;

const int buzzer = D1;
const int pulsePin = D5;
const int powerPin1 = D6;
const int powerPin2 = D7;

//Display Variables
const String modeValue = "Mode.modeNo";
const String onTimeValue = "Pulse.onTime";
const String frequencyValue = "Frequency.freq";
const String power1Value = "Amplitude.power1";
const String power2Value = "Amplitude.power2";

int onFl = 1;
int value = 0;
int power1 = 0;  //0-7
int power2 = 0;  //0-7
int onTime = 40; //40-260
int modOnTime = 40;
int frequency = 2;  //2-150 = Frequency Hz.
int pulseCount = 0;
int mode = 0; //0 = Bit Mode, 1 = normalmode, 2 = modMode

int offTime = 1;
int beepTimer = 0;

void setup() 
  {
    WiFi.forceSleepBegin();                                   // turn off ESP8266 RF
    delay(1);                                                 // give RF section time to shutdown
    system_update_cpu_freq(NODEMCU_FREQUENCY);
    
    pinMode(powerPin1, OUTPUT);
    pinMode(powerPin2, OUTPUT);
    pinMode(pulsePin, OUTPUT);
    pinMode(buzzer, OUTPUT);
    
    digitalWrite(buzzer, LOW);
    digitalWrite(powerPin1, LOW);
    digitalWrite(powerPin2, LOW);
    digitalWrite(pulsePin, LOW);

    //Serial.begin(9600);
    myNextion.init(); // send the initialization commands for Page 0
    ticker.attach_ms(1, tmrISR);
  }

void loop() 
  {
    if(offTime <= 0)
      {
        if(mode == 0)
          bitMode();
        else if(mode == 1)
          normalMode();
        else if(mode == 2)
          modMode();
      }
      
    checkTouchScreen();
    
    if(beepTimer <= 0)
      {
        beepTimer = 0;
        digitalWrite(buzzer, LOW);
      }
  }

void tmrISR()
  { 
    offTime = offTime - 1;
    beepTimer = beepTimer - 1;
  }

void getNewVars()
  {
    //Get values
    
    value = myNextion.getComponentValue(power1Value);
        
    value = myNextion.getComponentValue(power1Value);
    if(value > -1)
      power1 = value;
    value = myNextion.getComponentValue(power2Value);
    if(value > -1)
      power2 = value;
    value = myNextion.getComponentValue(onTimeValue);
    if(value > -1)
      onTime = value;
    value = myNextion.getComponentValue(frequencyValue);
    if(value > -1)
      frequency = value;
    value = myNextion.getComponentValue(modeValue);
    if(value > -1)
      mode = value;
    
    Serial.println();
    Serial.print(power1);
    Serial.print(' ');
    Serial.print(power2);
    Serial.print(' ');
    Serial.print(onTime);
    Serial.print(' ');
    Serial.print(frequency);
    Serial.print(' ');
    Serial.print(mode);
    
  }

void checkTouchScreen()
  {
    message = myNextion.listen(); //check for message
    if(!message.equals(""))
      {
        if(message.substring(7,10) == "1 f")
          {
            digitalWrite(buzzer, HIGH);
            beepTimer = 100;
          }
        if(message.substring(7,10) == "0 f")
          {
            getNewVars();
          }
        message = "";
      }
  }

void bitMode()
  {
    analogWrite(powerPin1, power1);
    analogWrite(powerPin2, power2);
       
    //6 pulses
    digitalWrite(pulsePin, HIGH);
    delayMicroseconds(onTime);
    digitalWrite(pulsePin, LOW);

    pulseCount = pulseCount + 1;
    if(pulseCount == 8)
      {
        pulseCount = 0;
        offTime = 500;
      }
    else
      {
        offTime = 1000/frequency; //Delay in ms
      }
  }
 
void modMode()
  {
    if(onFl == 1)
      {
        modOnTime = modOnTime + 1;
        if(modOnTime >= onTime)
          onFl = 0;
      }
     else
      {
        modOnTime = modOnTime - 1;
        if(modOnTime <= 40)
          onFl = 1; 
      } 
    
    analogWrite(powerPin1, power1);
    analogWrite(powerPin2, power2);
    
    digitalWrite(pulsePin, HIGH);
    delayMicroseconds(modOnTime);
    digitalWrite(pulsePin,LOW);
    //delay(25);//so that it takes 5secs to rise and 5secs to fall.
    offTime = 25;
  }
  
void normalMode()
  {
    analogWrite(powerPin1, power1); 
    analogWrite(powerPin2, power2);
    
    //Pulse
    digitalWrite(pulsePin, HIGH);
    delayMicroseconds(onTime);
    digitalWrite(pulsePin, LOW);
    offTime = 1000/frequency; //Delay in ms
  }

