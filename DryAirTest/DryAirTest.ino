#include <SoftwareSerial.h>

SoftwareSerial swSer(5, 4, false, 256); //swSer(14, 12, false, 256);

String inputString = "";         // a String to hold incoming data

int16_t counts[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int temp[11];

void setup() 
  {
    //ESP.wdtDisable();
    Serial.begin(115200);
    swSer.begin(9600);
    // reserve 200 bytes for the inputString:
    inputString.reserve(200);
  }

void loop() 
  {
    bool newData = false;
    while(newData == false)
      newData = getNewData();
   delay(150);
  }

bool getNewData() 
  {
    boolean stringComplete = false;
    inputString = "";
    int waitCount = 0;

    cli();
    swSer.print('#');
    sei();
    
    while(swSer.available() <= 0)
      {
        delay(1);
        waitCount++;
        if(waitCount > 5000)
          {
            waitCount = 0;
            cli();
            swSer.print('#');
            sei();
          }
      }

    waitCount = 0;
    
    while(stringComplete == false)
      {    
        while (swSer.available()) 
          {
            // get the new byte:
            char inChar = (char)swSer.read();
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
        if(inputString.charAt(i) == ' ' || inputString.charAt(i) == 'R' || inputString.charAt(i) == 'F' || inputString.charAt(i) == '\n')
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

    for(int i = 0; i < 21; i++)
      {
        Serial.print(counts[i]);
        Serial.print(" ");
      }
    Serial.println();
    
    stringComplete = false;
    inputString = "";
    return true;
  }
