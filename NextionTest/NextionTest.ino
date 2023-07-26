/*

Ricardo Mena C
ricardo@crcibernetica.com
http://crcibernetica.com

  This example code is in public domain

*/

//#include <SoftwareSerial.h>
#include <Nextion.h>

#define nextion Serial
//SoftwareSerial nextion(14, 12, false, 256); //SoftwareSerial nextion(2, 3);// Nextion TX to pin 2 and RX to pin 3 of Arduino

Nextion myNextion(nextion, 9600); //create a Nextion object named myNextion using the nextion serial port @ 9600bps

String message = "";

//Display Variables
const String modeValue = "Mode.modeNo";
const String onTimeValue = "Pulse.onTime";
const String frequencyValue = "Frequency.freq";
const String power1Value = "Amplitude.power1";
const String power2Value = "Amplitude.power2";

int onFl = 1;
int power1 = 0;  //0-7
int power2 = 0;  //0-7
int onTime = 40; //40-260
int frequency = 2;  //2-150 = Frequency Hz.
int pulseCount = 0;
int mode = 0; //1 = Bit Mode, 2 = modMode, 0 = normalmode

//boolean button1State;
//boolean button2State;

void setup() {
  //pinMode(13,OUTPUT);
  //Serial.begin(9600);
  //nextion.begin(9600);
  myNextion.init(); // send the initialization commands for Page 0
}

void loop() {
  message = myNextion.listen(); //check for message
  if(!message.equals(""))
    {
      if(message.substring(7,10) == "0 f")
        {
          getVars();
        }
    }
  message = "";
  delay(250);
  /*
  //Get values
  power1 = myNextion.getComponentValue(power1Value);
  power1 = myNextion.getComponentValue(power1Value);
  power2 = myNextion.getComponentValue(power2Value);
  onTime = myNextion.getComponentValue(onTimeValue);
  frequency = myNextion.getComponentValue(frequencyValue);
  mode = myNextion.getComponentValue(modeValue);
  delay(1000);
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
  delay(1000);
  
  String message = myNextion.listen(); //check for message
  if (message == "65 0 2 1 ffff ffff ffff") {
    //myNextion.buttonToggle(button1State, "b0", 0, 2);
    myNextion.setComponentText("LED_Status", "LED ON");
    digitalWrite(13,HIGH);
  }
  if (message == "65 0 3 1 ffff ffff ffff") {
    //myNextion.buttonToggle(button2State, "b1", 0, 2);
    myNextion.setComponentText("LED_Status", "LED OFF");
    digitalWrite(13,LOW);
  }
  */
}

void getVars()
{
  //Get values
  power1 = myNextion.getComponentValue(power1Value);
  power1 = myNextion.getComponentValue(power1Value);
  power2 = myNextion.getComponentValue(power2Value);
  onTime = myNextion.getComponentValue(onTimeValue);
  frequency = myNextion.getComponentValue(frequencyValue);
  mode = myNextion.getComponentValue(modeValue);
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

