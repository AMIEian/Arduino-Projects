String inputString = ""; 

bool getNewData() 
  {
    boolean stringComplete = false;
    inputString = "";
    int waitCount = 0;

    Serial2.print('#');
    //digitalWrite(Arduino_Error_LED, HIGH);
    
    while(Serial2.available() <= 0)
      {
        delay(1);
        waitCount++;
        if(waitCount > 5000)
          {
            waitCount = 0;
            Serial2.print('#');
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
            inputs[countIndex] = value;
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

void sendNewValues()
  {
    Serial2.print('0');
    for(int i = 0; i < 25; i++)
      {
        Serial2.print(',');
        Serial2.print(outputs[i]);
      }
    Serial2.print(',');
    Serial2.println();
  }

void CalibrationMode()
  {
    bool calMode = true;
    //Serial.print("Calibration Mode...!");
    while(calMode == true)
      {
        while (Serial2.available() > 0) 
          {
            Serial.write(Serial2.read());
          }
        while (Serial.available() > 0) 
          {
            Serial2.write(Serial.read());
          }
      }
  }
