#include <TimerOne.h>

/*
CONNECT 138 (DECODER IC) TO PORTC; ANALOG PINS; 1 - A0, 2 - A1, 3 - A2
CONNECT 74LS47 (BCD TO 7 SEG) TO PORTB; ARDUINO PINS; A - 8, B - 9, C - 10, D - 11
*/
const int fullCylinder = 9;
const int emptyCylinder = 10;
const int offButton = 7;
const int onButton = 8;
const int co2Button = 13;
const int airButton = 14;
const int verButton = 5;
const int trocButton = 6;

const int icSelector = 2; //This will select decoder IC one by one
const int airLED = 3;
const int startLED = 4;
const int heaterLED = 5;

byte displayAddress = 0; //This is for holding address of 7 seg. display. Ranges from 0 to 7 for each decoder ic (Total 14 7 Seg. Displays)
byte displayCount = 0; //This represents index for display values array. Ranges from 0 to 13 (Total 14 7 Seg. Displays)
byte values[14] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00}; //This array holds actual values for each individual 7 seg. display.
int digit_0 = 0;
int digit_1 = 0;
int digit_2 = 0;

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() 
{
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  
  pinMode(icSelector, OUTPUT);
  pinMode(airLED, OUTPUT);
  pinMode(startLED, OUTPUT);
  pinMode(heaterLED, OUTPUT);
  
  digitalWrite(icSelector, LOW);
  digitalWrite(airLED, LOW);
  digitalWrite(startLED, LOW);
  digitalWrite(heaterLED, LOW);

  DDRB = B11111111; //PORTB; ARDUINO PIN NO. 8,9,10,11,12 AND 13 AS OUTPUT
  DDRC = B11111111; //PORTC; ARDUINO PINS A0, A1, A2, A3, A4 AND A5 AS OUTPUT
  
  Timer1.initialize(800); // set a timer of length 800 microseconds, each display will get updated appox. 90 times per second
  Timer1.attachInterrupt( updateDisplay ); // attach the display update routine here
}

void loop() 
{
  if (stringComplete) 
  {
    String entity = inputString.substring(0, inputString.lastIndexOf('='));
    //Serial.println(entity);
    
    if(entity == "act_flow.txt" || entity == "act_pres.txt" || entity == "vol.txt" || entity == "temp.txt" || entity == "set_flow.txt" || entity == "set_pres.txt")
      { 
        String value = inputString.substring(inputString.indexOf('"')+1, inputString.lastIndexOf('"'));
        //Serial.println(value);
        stringToNum(value);

        if(entity == "act_flow.txt")
          {
            values[6] = digit_0;  //Tens
            values[7] = digit_1;  //Units
          }

        if(entity == "act_pres.txt")
          {
            values[0] = digit_0;  //Tens
            values[1] = digit_1;  //Units
          }

        if(entity == "vol.txt")
          {
            values[8] = digit_2;  //Hundreds
            values[9] = digit_0;  //Tens
            values[10] = digit_1;  //Units
          }    

        if(entity == "temp.txt")
          {
            values[11] = digit_0;  //Tens
            values[12] = digit_1;  //Units
          }  

        if(entity == "set_flow.txt")
          {
            values[4] = digit_0;  //Tens
            values[5] = digit_1;  //Units
          }

        if(entity == "set_pres.txt")
          {
            values[2] = digit_0;  //Tens
            values[3] = digit_1;  //Units
          }   
      }
      
    if(entity == "cylinder.pic" || entity == "on_off.pic" || entity == "ver_troc.pic" || entity == "co2_air.pic")
      {
        int value = inputString.substring(inputString.indexOf('=')+1).toInt();
        //Serial.println(value.toInt());
        
        if(entity == "on_off.pic" && value == onButton)
          digitalWrite(startLED, LOW);
          
        if(entity == "on_off.pic" && value == offButton)
          digitalWrite(startLED, HIGH);

        if(entity == "co2_air.pic" && value == airButton)
          {
            digitalWrite(airLED, LOW);
            digitalWrite(heaterLED, HIGH);
          }
          
        if(entity == "co2_air.pic" && value == co2Button)
          digitalWrite(airLED, HIGH);   
      }
      
    if(entity == "temp.pco")
      {
        String value = inputString.substring(inputString.indexOf('=')+1);
        //Serial.println(value);

        if(value == "YELLOW")
            digitalWrite(heaterLED, LOW);
        else
            digitalWrite(heaterLED, HIGH);
      }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void stringToNum(String str)
{
  digit_0 = str.charAt(0) - 48;
  //Serial.println(str.charAt(0));
  digit_1 = str.charAt(1) - 48;
  //Serial.println(str.charAt(1));
  if(str.length() == 3)
    {
      digit_2 = str.charAt(2) - 48;
      //Serial.println(str.charAt(2));
    }
}

void updateDisplay()
{
  if(displayAddress == 7)
    {
      //CHANGE THE IC
      if(digitalRead(icSelector) == HIGH)
        digitalWrite(icSelector, LOW);
      else
        digitalWrite(icSelector, HIGH);
      displayAddress = 0; //RESET THE ADRESS VALUE
    }
  PORTC = displayAddress;   //SELECT THE ADRESS OF 7 SEGMENT
  PORTB = values[displayCount];   //APPLY ACTUAL VALUE TO 7 SEGMENT
  displayAddress = displayAddress + 1; //INCREMENT ADRESS OF 7 SEGMENT
  displayCount = displayCount + 1;   //INCREMENT COUNTRER OF DATA
  if(displayCount == 14)
    displayCount = 0;  //RESET VALUE ARRAY COUNTER
}

void serialEvent() 
{
  while (Serial.available()) 
  {
    byte inChar = Serial.read();
    if (inChar == 0xFF)
      stringComplete = true;
    else
      inputString += (char)inChar;
  }
}
