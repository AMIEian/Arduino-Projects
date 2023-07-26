
//Primary = 60 Turns / 0.1mm Gage (Diameter) / 0.74mH /6.7 ohm
//Secondary = 1000 Turns / 0.1mm Gage (Diameter) / 187mH/ 85 ohms
//Core Size = EE14
//Transistor = C2655 (NPN)

#include <Nextion.h>
#include <TimerOne.h>

#define nextion Serial  //HW Serial

Nextion myNextion(nextion, 9600); //create a Nextion object named myNextion using the nextion serial port @ 9600bps
String message = "";

const int buzzer = 2;//D1
const int pulsePin = 3;//D5
const int powerPin1 = 10;//D6
const int powerPin2 = 11;// D7

//Display Variables
const String modeValue = "Mode.modeNo";
const String onTimeValue = "Pulse.onTime";
const String frequencyValue = "Frequency.freq";
const String power1Value = "Amplitude.power1";
const String power2Value = "Amplitude.power2";

int onFl = 1;
int value = 0;
int power1 = 250;  //0-7
int power2 = 0;  //0-7
int onTime = 250; //40-260
int modOnTime = 40;
int frequency = 150;  //2-150 = Frequency Hz.
int pulseCount = 0;
int mode = 1; //0 = Bit Mode, 1 = normalmode, 2 = modMode

int offTime = 1;
int beepTimer = 0;

void tmrISR()
  { 
    offTime = offTime - 1;
    beepTimer = beepTimer - 1;
  }

void setup() 
  {
    pinMode(powerPin1, OUTPUT);
    pinMode(powerPin2, OUTPUT);
    pinMode(pulsePin, OUTPUT);
    pinMode(buzzer, OUTPUT);
    
    digitalWrite(buzzer, LOW);
    digitalWrite(powerPin1, LOW);
    digitalWrite(powerPin2, LOW);
    digitalWrite(pulsePin, LOW);

    Serial.begin(9600);
    myNextion.init(); // send the initialization commands for Page 0
    //initialize TimerOne's interrupt
    Timer1.initialize(1000);           //period in microseconds
    Timer1.attachInterrupt(tmrISR);   //attach the Timer1 interrupt

    analogWrite(powerPin1, power1);
    analogWrite(powerPin2, power2);
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

void getNewVars()
  {
    //Get values
    
    value = myNextion.getComponentValue(power1Value);
        
    value = myNextion.getComponentValue(power1Value);
    if(value > -1)
    {
      //power1 = value + 100;
      power1 = map(value,0,100,80,254);
      analogWrite(powerPin1, power1);
    }
    value = myNextion.getComponentValue(power2Value);
    if(value > -1)
    {
      //power2 = value + 100;
      power2 = map(value,0,100,80,254);
      analogWrite(powerPin2, power2);
    }
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
    //6 pulses
    PORTD = PORTD | 0b00001000; // set D3 (high)
    delayMicroseconds(onTime);
    PORTD = PORTD & 0b11110111; // clear D3 (low)
    
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
    
    PORTD = PORTD | 0b00001000; // set D3 (high)
    delayMicroseconds(modOnTime);
    PORTD = PORTD & 0b11110111; // clear D3 (low)
    //delay(25);//so that it takes 5secs to rise and 5secs to fall.
    offTime = 25;
  }
  
void normalMode()
  {
    //Pulse
    PORTD = PORTD | 0b00001000; // set D3 (high)
    delayMicroseconds(onTime);
    PORTD = PORTD & 0b11110111; // clear D3 (low)
    offTime = 1000/frequency; //Delay in ms
  }
