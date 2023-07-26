String inputString = ""; 

void getNewData() 
  {
    boolean stringComplete = false;
    inputString = "";
    int waitCount = 0;

    Serial2.print('#');
    digitalWrite(PIC_COM_LED, HIGH);
    
    while(Serial2.available() <= 0)
      {
        delay(1);
        waitCount++;
        if(waitCount > 5000)
          {
            digitalWrite(PIC_Error_LED, HIGH);
            waitCount = 0;
            Serial2.print('#');
          }
        digitalWrite(PIC_Error_LED, LOW);
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
          {
            digitalWrite(PIC_Error_LED, HIGH);
            return;
          }
      }
    
    String valueStr = "";
    int value = 0;
    int countIndex = 0;
   // SerialBT.println("---------------------------");
   // SerialBT.println("DI/AI values from PIC");
    for(int i = 1; i < inputString.length(); i++)
      {
        if(inputString.charAt(i) == ' ' || inputString.charAt(i) == '\n')
          {
            value = valueStr.toInt();
            inputs[countIndex] = value;
          //  SerialBT.println(value);
            countIndex++;
            valueStr = "";
            value = 0;  
          }
        else
          valueStr = valueStr + inputString.charAt(i);
      }
    //SerialBT.println("---------------------------");
    digitalWrite(PIC_COM_LED, LOW);
    stringComplete = false;
    inputString = "";
    setInputTemperature();
    setInputHumidity();
  }

void sendNewValues()
  {
    digitalWrite(PIC_COM_LED, HIGH);
    Serial2.print('0');
    for(int i = 0; i < 25; i++)
      {
        Serial2.print(',');
        Serial2.print(outputs[i]);
      }
    Serial2.print(',');
    Serial2.println();
    digitalWrite(PIC_COM_LED, LOW);
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

void updatePIC(int del)
  {
    sendNewValues();
    delay(del);
    getNewData();
  }

void setInputTemperature()
{
  float temp;
  if(AHUSetting[trhSens] == 1)
    inputTemperature = inputs[SA_Temp];
  else
    {
      temp = myMap(inputs[RA_Temp_AI], 0, ADC_MAX_COUNTS, RATemp[zero], RATemp[full]);
      inputTemperature = temp * 10;
    }
}

void setInputHumidity()
{
  float temp;
  if(AHUSetting[trhSens] == 1)
    inputHumidity = inputs[SA_Humidity];
  else
    {
      temp = myMap(inputs[RA_Humidity_AI], 0, ADC_MAX_COUNTS, RAHum[zero], RAHum[full]);
      inputHumidity = temp * 10;
    }
}
