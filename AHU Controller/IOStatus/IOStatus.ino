#include <Nextion.h>

#define nextion Serial

//Indexes for various readings in counts array
#define RA_Temp_AI 0
#define RA_Humidity_AI 1
#define VFD_Speed_AI 2
#define Air_Velocity_AI 3
#define CO2_AI 4
#define SPARE_AI 5

#define AHU_On_Off 6
#define AHU_Trip 7
#define Filter_1 8
#define Filter_2 9
#define Fire_Input 10
#define Fire_Supply 11
#define Fire_Return 12
#define AHU_Door 13
#define SA_Temp 14
#define SA_Humidity 15
#define Fine_Filter 16
#define HEPA_Filter 17

#define AI_INDEX 0
#define DI_INDEX 6

#define RXD2 16
#define TXD2 17

String inputString = ""; 
int16_t counts[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

Nextion myNextion(nextion, 9600);
int screenCounter = 0;

void setup() 
{
  Serial.begin(9600);
  myNextion.init(); // send the initialization commands for Page 0
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop() 
{
  getNewData();
  switch(screenCounter)
    {
      case 1 :
        updateIOScreen_1();  //Calibrate 0 Values
        break;
      case 2 :
        updateIOScreen_2();  //Calibrate 0 Values
        break;
      case 3 :
        updateIOScreen_3();  //Calibrate 0 Values
        break;
      case 4 :
        updateIOScreen_4();  //Calibrate 0 Values
        break;
    }
  serialEvent();
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (inChar == 'a') {
      screenCounter = 1;
    }
    else if (inChar == 'b') {
      screenCounter = 2;
    }
    else if (inChar == 'c') {
      screenCounter = 3;
    }
    else if (inChar == 'd') {
      screenCounter = 4;
    }
    else {
      screenCounter = 0;
    }
  }
}

bool getNewData() 
  {
    boolean stringComplete = false;
    inputString = "";
    int waitCount = 0;

    cli();
    Serial2.print('#');
    sei();
    //digitalWrite(Arduino_Error_LED, HIGH);
    
    while(Serial2.available() <= 0)
      {
        delay(1);
        waitCount++;
        if(waitCount > 5000)
          {
            waitCount = 0;
            cli();
            Serial2.print('#');
            sei();
          }
      }

    waitCount = 0;
    
    while(stringComplete == false)
      {    
        while(Serial2.available()) 
          {
            // get the new byte:
            char inChar = (char)Serial2.read();
            inputString += inChar;
            if (inChar == '\n' || inChar == '\r') 
              stringComplete = true;
          }
        delay(1);
        waitCount++;
        if(waitCount > 1000)
          return false;
      }
    
    String valueStr = "";
    int value = 0;
    int countIndex = 0;
    
    for(int i = 1; i < inputString.length(); i++)
      {
        if(inputString.charAt(i) == ' ' || inputString.charAt(i) == '\n')
          {
            value = valueStr.toInt();
            counts[countIndex] = value;
            countIndex++;
            valueStr = "";
            value = 0;  
          }
        else
          valueStr = valueStr + inputString.charAt(i);
      }

    stringComplete = false;
    inputString = "";
    //digitalWrite(Arduino_Error_LED, LOW);
    return true;
  }

void updateIOScreen_1()
  {
    myNextion.setComponentText("l1", String(counts[AHU_On_Off]));
    myNextion.setComponentText("l2", String(counts[AHU_Trip]));
    myNextion.setComponentText("l3", String(counts[Filter_1]));
    myNextion.setComponentText("l4", String(counts[Filter_2]));
    myNextion.setComponentText("l5", String(counts[Fire_Input]));
    myNextion.setComponentText("l6", String(counts[Fire_Supply]));
    myNextion.setComponentText("l7", String(counts[Fire_Return]));
    myNextion.setComponentText("l8", String(counts[AHU_Door]));
    
    if(counts[AHU_On_Off] == 1)
      myNextion.setComponentText("v1", "On");
    else
      myNextion.setComponentText("v1", "Off");

    if(counts[AHU_Trip] == 1)
      myNextion.setComponentText("v2", "Trip");
    else
      myNextion.setComponentText("v2", "Healthy");

    if(counts[Filter_1] == 1)
      myNextion.setComponentText("v3", "Clog");
    else
      myNextion.setComponentText("v3", "Healthy");

    if(counts[Filter_2] == 1)
      myNextion.setComponentText("v4", "Clog");
    else
      myNextion.setComponentText("v4", "Healthy");

    if(counts[Fire_Input] == 1)
      myNextion.setComponentText("v5", "Safe");
    else
      myNextion.setComponentText("v5", "Danger");

    if(counts[Fire_Supply] == 1)
      myNextion.setComponentText("v6", "Open");
    else
      myNextion.setComponentText("v6", "Closed");

    if(counts[Fire_Return] == 1)
      myNextion.setComponentText("v7", "Open");
    else
      myNextion.setComponentText("v7", "Closed");

    if(counts[AHU_Door] == 1)
      myNextion.setComponentText("v8", "Closed");
    else
      myNextion.setComponentText("v8", "Open");
  }

void updateIOScreen_2()
  {
    myNextion.setComponentText("l1", String(counts[RA_Temp_AI]));
    myNextion.setComponentText("l2", String(counts[RA_Humidity_AI]));
    myNextion.setComponentText("l3", String(counts[VFD_Speed_AI]));
    myNextion.setComponentText("l4", String(counts[Air_Velocity_AI]));
    myNextion.setComponentText("l5", String(counts[CO2_AI]));
    myNextion.setComponentText("l6", String(counts[SPARE_AI]));
    myNextion.setComponentText("l7", "");
    myNextion.setComponentText("l8", "");
  }

void updateIOScreen_3()
  {
    
  }

void updateIOScreen_4()
  {
    
  }
