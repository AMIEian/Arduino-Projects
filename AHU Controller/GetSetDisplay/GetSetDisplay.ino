#include <Nextion.h>
#include "variables.h"
#include "display_strings.h"

#define nextion Serial2
Nextion myNextion(nextion, 9600);

//COMMUNICATION VARIABLES
int16_t inputs[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int16_t outputs[25] = {0, 1, 0, 50, 0, 0, 0, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0, 5, 0, 0, 0};

int screenCounter = 0;

void setup() 
{
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  myNextion.init(); // send the initialization commands for Page 0
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
