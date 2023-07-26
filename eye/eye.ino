#include <Nextion.h>
#include <TimerOne.h>

#define nextion Serial

//Pin Assignment
const int LED = 13;
const int controlVolve = 12;
const int motor = 10;
const int buzzer = 5;
const int footSwitch = 8;

Nextion myNextion(nextion, 9600); //create a Nextion object named myNextion using the nextion serial port @ 9600bps

//Display Variables
const String pulsesDisplay = "pulses";
const String progressBarDisplay = "progress";

//Controling Variables
int setPulses = 1;
int progressValue = 20;
int touchTimer = 0;
int beepTimer = 0;
unsigned int pulseOnTime = 20;  //Pluse on/off time in milliSecs
unsigned int pulseOffTime = 1180;  //Pluse on/off time in milliSecs
unsigned int pulseTimer = 0;

void setup()
  {
    pinMode(LED, OUTPUT);
    pinMode(controlVolve, OUTPUT);
    pinMode(motor, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(footSwitch, INPUT);

    digitalWrite(LED, LOW);
    digitalWrite(controlVolve, LOW);
    digitalWrite(motor, LOW);
    digitalWrite(buzzer, LOW);

    Serial.begin(9600);
    myNextion.init(); // send the initialization commands for Page 0
    Timer1.initialize( 1000 );
    Timer1.attachInterrupt( tmrISR );

    myNextion.setComponentText(pulsesDisplay, numToString(setPulses));
    myNextion.setProgressBar(progressBarDisplay, progressValue);
  }

void loop()
  {
    if(beepTimer <= 0)
      {
        beepTimer = 0;
        digitalWrite(buzzer, LOW);
      }
    if(touchTimer <= 0)
      {
        if(digitalRead(footSwitch) == LOW)
          checkTouchScreen();
        touchTimer = 250;
      }
    if(digitalRead(footSwitch) == LOW)
      {
        digitalWrite(controlVolve, LOW);
        digitalWrite(motor, LOW);
        pulseTimer = 0;
      }
    if(pulseTimer <= 0)
      {
        if(digitalRead(footSwitch) == HIGH)
          {
            digitalWrite(motor, HIGH);
            if(digitalRead(controlVolve) == HIGH)
              {
                digitalWrite(controlVolve, LOW);
                pulseTimer = pulseOffTime;
              }      
            else
              {
                digitalWrite(controlVolve, HIGH);
                pulseTimer = pulseOnTime;
              }
          }
      }
  }

void tmrISR()
  { 
    beepTimer = beepTimer - 1;
    touchTimer = touchTimer - 1;
    pulseTimer = pulseTimer - 1;
  }

void checkTouchScreen()
  {
    String message = myNextion.listen(); //check for message
  
    if (message == "65 0 2 1 ffff ffff ffff") 
      {
        //Pluse button pressed
        if(setPulses == 1)
          {
            setPulses = 100;
            pulseOffTime = 60000 - (setPulses * pulseOnTime);
            pulseOffTime = int(pulseOffTime/setPulses);
          }
        else
          {
            setPulses = setPulses + 100;
            if(setPulses > 800)
              setPulses = 800;
              
            pulseOffTime = 60000 - (setPulses * pulseOnTime);
            pulseOffTime = int(pulseOffTime/setPulses);
          }

        if(progressValue < 100)
          progressValue = progressValue + 10;
        else
          progressValue = 100;

        myNextion.setComponentText(pulsesDisplay, numToString(setPulses));
        myNextion.setProgressBar(progressBarDisplay, progressValue);

        //Play Beep
        digitalWrite(buzzer,HIGH);
        beepTimer = 250;
      }
  
    if (message == "65 0 3 1 ffff ffff ffff") 
      {
        //Minus button pressed
        setPulses = setPulses - 100;
        
        if(setPulses < 100)
          {
            setPulses = 1;
            pulseOffTime = 1180;
          }
        else
          {
            pulseOffTime = 60000 - (setPulses * pulseOnTime);
            pulseOffTime = int(pulseOffTime/setPulses);
          }
        
        if(progressValue > 20)
          progressValue = progressValue - 10;
        else
          progressValue = 20;

        myNextion.setComponentText(pulsesDisplay, numToString(setPulses));
        myNextion.setProgressBar(progressBarDisplay, progressValue);

        //Play Beep
        digitalWrite(buzzer,HIGH);
        beepTimer = 250;
      }
  }

String numToString(int num)
  {
    if(num < 100)
      return String(0) + String(0) + String(num);
    else
      return String(num);
  }
