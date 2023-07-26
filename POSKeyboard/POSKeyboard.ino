#include <BleKeyboard.h>

BleKeyboard bleKeyboard{"TagID BT", "TagID Solutions", 100};

bool started = false;
bool paste = false;

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

void setup()
  {
    Serial.begin(115200);
    bleKeyboard.begin();
    started = true;
    inputString.reserve(200);
  }

void loop()
  {
    if(bleKeyboard.isConnected()) 
      {
        serialEvent();
        if(stringComplete == true)
          {
            stringComplete = false;
            bleKeyboard.print(inputString);
            Serial.print('k');
            inputString = "";
          }
      }
    else
      {
        if(started == false)
          {
            ESP.restart();
          }
      }
  }

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
