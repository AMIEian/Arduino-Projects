void CalibrationMode()
{
  bool terminate = false;
  bool pcCommandReceived = false;
  while(terminate == false)
    {
      while(pcCommandReceived == false)
        {
          GetDIs();
          GetAIs();
          while(Serial.available()) 
            {
              // get the new byte:
              char inChar = (char)Serial.read();
              if(inChar == '#')
                {
                  for(int i = 0; i < 5; i++)
                    {
                      Serial.print(' ');
                      Serial.print(counts[i]);
                    } 
                  Serial.println();
                  pcCommand = "";
                }
              else
                {
                  pcCommand += inChar;
                  if (inChar == '\n') 
                      pcCommandReceived = true;
                }
            }
        }
      if(pcCommandReceived == true)
        {
          pcCommandReceived = false;
          
          if(pcCommand.charAt(0) == 'x')
            calibrateZero();  //Calibrate 0 Values
          else if(pcCommand.charAt(0) == 'X')
            calibrateFull();  //Calibrate Full Values
          else if(pcCommand.charAt(0) == '>')
            saveCalibration(pcCommand.charAt(1));  //Save Calibrations to EEPROM
          else if(pcCommand.charAt(0) == '<')
            loadCalFromEEPROM();  //Discard Calibrations and Load from EEPROM
          else if(pcCommand.charAt(0) == '!')
            saveCalibration('Z');  //Factory Reset. Format user calibration and load calibrations from Factory Values 
          else if(pcCommand.charAt(0) == '.')
            allClear();  //Format everything. All calibrations and WiFi settings
          else if(pcCommand.charAt(0) == '$')
            saveWiFiConfig(); //Save WiFi Settings to EEPROM
          else if(pcCommand.charAt(0) == '@')  
            sendWiFiConfig(); //Read and send WiFi Settings from EEPROM
          else if(pcCommand.charAt(0) == 'Y')
            analogCalibrate(); //Change DOs
          else if(pcCommand.charAt(0) == '0')
            changeOutputs(); //Change DOs and AOs
            
          pcCommand = "";
        }
    }
}

void calibrateZero()
  {
    int index = 0;
    pcCommand.trim(); //Removes Extra Spaces and Newline Character from Command String
    if(validCalZeroString())
      {
        for(int i = 1; i < pcCommand.length(); i++)
          {
            //Store current readings as zeroValues
            index = pcCommand.charAt(i) - 'a';
            zeroVals[index] = zeroVals[index] + counts[index];
          }
      }
  }

boolean validCalZeroString()
  {
    boolean res = true;
    for(int i = 1; i < pcCommand.length(); i++)
      {
        if(pcCommand.charAt(i) >= 'a' && pcCommand.charAt(i) <= 'h')
          continue;
        else
          {
            res = false;
            break;
          }
      }
    return res;
  }

void calibrateFull()
  {
    int index = 0;
    pcCommand.trim(); //Removes Extra Spaces and Newline Character from Command String
    if(validCalFullString())
      {
        for(int i = 1; i < pcCommand.length(); i++)
          {
            //Store current readings as zeroValues
            index = pcCommand.charAt(i) - 'A';
            fullVals[index] = counts[index];
          }
      }
  }

boolean validCalFullString()
  {
    boolean res = true;
    for(int i = 1; i < pcCommand.length(); i++)
      {
        if(pcCommand.charAt(i) >= 'A' && pcCommand.charAt(i) <= 'D')
          continue;
        else
          {
            res = false;
            break;
          }
      }
    return res;
  }

void analogCalibrate()
{
    int index = 0;
    int i, len;
    pcCommand.trim(); //Removes Extra Spaces and Newline Character from Command String
    len = pcCommand.length();
    if(validAnalogString())
      {
        for(i = 1; i < len; i++)
          {
            //Store current readings as zeroValues
            index = pcCommand.charAt(i) - 'A';
            fullVals[index] = analogCounts[index - DAC_FULLVALS_INDEX];
          }
      }
}

bool validAnalogString()
{
    bool res = true;
    int i, len;
    len = pcCommand.length();
    for(i = 1; i < len; i++)
      {
        if(pcCommand.charAt(i) >= 'E' && pcCommand.charAt(i) <= 'H')
          continue;
        else
          {
            res = false;
            break;
          }
      }
}

void saveCalibration(char mode)
  {
    int16_t zeros[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    
    //Clear old user calibration
    prefs.putString("userCal", "Not-Done");
    prefs.putBytes("userCalZero", zeros, sizeof(zeros));
    prefs.putBytes("userCalFull", zeros, sizeof(zeros));
    //Check the mode
    if(mode == 'C') //User Mode
      {          
        //Save new user calibration
        prefs.putString("userCal", "Done");
        prefs.putBytes("userCalZero", zeroVals, sizeof(zeroVals));
        prefs.putBytes("userCalFull", fullVals, sizeof(fullVals));
      }
    if(mode == 'F') //Factory Mode
      {
        //Clear old factory calibration
        prefs.putString("factoryCal", "Not-Done");
        prefs.putBytes("factoryCalZero", zeros, sizeof(zeros));
        prefs.putBytes("factoryCalFull", zeros, sizeof(zeros));
        
        //Save new factory calibration
        prefs.putString("factoryCal", "Done");
        prefs.putBytes("factoryCalZero", zeroVals, sizeof(zeroVals));
        prefs.putBytes("factoryCalFull", fullVals, sizeof(fullVals));
      }  
    loadCalFromEEPROM();  //Load the Calibration from EEPROM to get in effect.
  }

void loadCalFromEEPROM()
  {
    String calDone = "";
    int index = 0;
    calDone = prefs.getString("userCal");
    if(calDone == "Done")
      {
        //User calibration is found.
        prefs.getBytes("userCalZero", zeroVals, sizeof(zeroVals));
        prefs.getBytes("userCalFull", fullVals, sizeof(fullVals));
      }
    else
      {
        calDone = prefs.getString("factoryCal");
        if(calDone == "Done")
          {
            //Factory calibration is found
            prefs.getBytes("factoryCalZero", zeroVals, sizeof(zeroVals));
            prefs.getBytes("factoryCalFull", fullVals, sizeof(fullVals));
          }
        else
          {
            //If User and Factory Calibration is not found then load default values
            for(int i = 0; i < 8; i++)
              {
                zeroVals[i] = 0;
              }
            for(int i = 0; i < 4; i++)
              {
                fullVals[i] = 10000;
              }
            for(int i = 4; i < 8; i++)
              {
                fullVals[i] = 3800;
              }
          }
      }
  }

void saveWiFiConfig()
  {
    prefs.putString("wifiConfig", pcCommand);
    prefs.putString("wifiConfigDone", "Done");
  }

void sendWiFiConfig()
  {
     String calDone = "";
     calDone = prefs.getString("wifiConfigDone");
     if(calDone == "Done")
        {
          Serial.println(prefs.getString("wifiConfig"));
        }
  }

void allClear()
  {
    prefs.clear();
    loadCalFromEEPROM();
  }

void changeOutputs()
  {
    String valueStr = "";
    int value = 0;
    int countIndex = 0;
    
    for(int i = 2; i < pcCommand.length()-1; i++)
      {
        if(pcCommand.charAt(i) == ',')
          {
            value = valueStr.toInt();
            inputCounts[countIndex] = value;
            Serial.print(inputCounts[countIndex]);
            Serial.print(" ");
            countIndex++;
            valueStr = "";
            value = 0;  
          }
        else
          valueStr = valueStr + pcCommand.charAt(i);
      }
    SetDOs();
    SetAOs();
  }
