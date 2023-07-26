void initUnit()
{
  //Serial.begin(9600);
  //ClearROM();
  initHardware();
  readNumbersFromROM();  
  setSpkVolume();
  //Start configuration of shield with baud rate.
  if (gsm.begin(9600))
    {
      //Serial.println(F("READY"));
      for(int8_t i=1;i<=20;i++)
        sms.DeleteSMS(i);
    }                
  //else
    //Serial.println(F("IDLE"));
  gsm.setBusy(0);
}

void readNumbersFromROM()
{  
  int8_t i;
  String retrievedString = readStringFromEEPROM(0);
  totalAdmins = readStringFromEEPROM(0).toInt();
  totalUsers = readStringFromEEPROM(5).toInt();

  //Reading Admin numbers from EEPROM
  for(i = 0; i < totalAdmins; i++)
    {
      authenticNumbers[i] = readStringFromEEPROM(10 + (i*20));
    }

  //Reading Users numbers from EEPROM
  for(i = 2; i < totalUsers + 2; i++)
    {
      authenticNumbers[i] = readStringFromEEPROM(10 + (i*20));
    }
  /*
  for(i = 0; i < 7; i++)
    {
      //Serial.print(authenticNumbers[i]);
      //Serial.println();
    }    
  //Serial.println(totalAdmins);
  //Serial.println(totalUsers);
  */
}

void setSpkVolume()
  {
    String vol = readStringFromEEPROM(250);
    if(vol != 0)
      {
        String cmd = "AT+CLVL=" + String(vol);
        cmd.toCharArray(smsbuffer,15,0);
        gsm.setVolume(smsbuffer);
      }
  }

void ClearROM()
  {
    for(int i = 0; i < 500; i++)
      EEPROM.write(i,0);
  }
