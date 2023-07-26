void initUnit()
{
  //Serial.begin(9600);
  initHardware();
  readNumbersFromROM();  
  //Start configuration of shield with baud rate.
  if (gsm.begin(9600))
    {
      Serial.println(F("READY"));
      in_call.Call("9819895913");
      for(int8_t i=1;i<=20;i++)
        sms.DeleteSMS(i);
    }                
  else
    Serial.println(F("IDLE"));
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
      Serial.print(authenticNumbers[i]);
      Serial.println();
    }    
  Serial.println(totalAdmins);
  Serial.println(totalUsers);
  */
}
