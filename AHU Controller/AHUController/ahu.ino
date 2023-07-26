void initAHUSettings()
{
  changeAHUStatus(AHU_INIT);
  
  setAISettings();
  setPIDSettings();
  setPasswordsValues();
  setUserSettingsValues();
  //setAHUDateTimeValues();
  setSchedularValues();
  setHolidaysValues();
  setAHUSettingsValues();
  
  delay(1000);

  if(AHUSetting[ahuMode] == 3)
    ahuScadaModeInit();
  else if(AHUSetting[ahuMode] == 2)
    ahuManualModeInit();
    
  if(AHUSetting[ahuStatus] == AHU_ON)
    {
      if(schedulerEnabled == 0)
        {
          if(AHUSetting[ahuMode] == 1 || AHUSetting[ahuMode] == 3)
            {
              SerialBT.println("Turning it on again...");
              startAHU();
            }
        }
      else
        {
          onBySchedular = prefs.getBool("SCHON");
          if(onBySchedular == true)
            {
              SerialBT.println("AHU was on by scheduler, so leave it to scheduler...");
              AHUSetting[ahuStatus] = AHU_OFF;
              changeAHUStatus(AHU_OFF);
            }
          else
            {
              SerialBT.println("Scheduler is on, AHU was manually on, so scheduler can't turn it off...");
              if(AHUSetting[ahuMode] == 1 || AHUSetting[ahuMode] == 3)
                {
                  SerialBT.println("Turning it on again...");
                  startAHU();
                }
            }
        }
    }
  else
    changeAHUStatus(AHU_OFF);
}

void amRrelayTurnOff()
{
  outputs[RO_INDEX] = outputs[RO_INDEX] & AM_RELAY_OFF;
  updatePIC(1000);
}

void ahuManualModeInit()
{
  changeAHUStatus(AHU_INIT);
  //Turn on A/M Relay, Fire supply and Fire Damper
  if(AHUSetting[fireDampSupCtrl] == 1 || AHUSetting[fireDampRetCtrl] == 1)
    {
      if(AHUSetting[fireDampSupCtrl] == 1)
        outputs[RO_INDEX] = outputs[RO_INDEX] | FIRE_DAMPER_SUPPLY_ON;
      if(AHUSetting[fireDampRetCtrl] == 1)
        outputs[RO_INDEX] = outputs[RO_INDEX] | FIRE_DAMPER_RETURN_ON;
      //Switch Fire Damper Relays On and Check Their DIs after 30s
      for(int i = 0; i <= 30; i++)
        updatePIC(1000);
    }
    
  outputs[RO_INDEX] = outputs[RO_INDEX] | AM_RELAY_ON;
  if(VFD[enabled] == 1)
    {
      outputs[VFD_AO] = DAC_MAX_COUNTS;
    }
  updatePIC(1000);
  changeAHUStatus(AHU_OFF);
}

void startAHU_Manual_Mode()
{
  updatePIC(1000);
  if(inputs[AHU_Door] == 1)
    {
      changeAHUStatus(AHU_COUNTDOWN);
      setCountDown();
      //Turn on AHU
      outputs[RO_INDEX] = outputs[RO_INDEX] | AHU_RELAY_ON;
      updatePIC(1000);
      changeAHUStatus(AHU_ON);
      updatePIC(20000);
      tempPIDTurnOn();
      if(AHUSetting[rhCtrl] == 1)
        humPIDTurnOn();
      AHUSetting[ahuStatus] = AHU_ON;
      changeAHUStatus(AHU_ON);
      setAHUSettingsValues();
    }
  else
    {
      setAHUWarning(AHU_DOOR_OPEN,0);
      buzzerOn();
      getWarningAck();
      buzzerOff();
      while(inputs[AHU_Door] == 0)
        {
          //Remain here until door is open
          updatePIC(1000);
        }
      homeScreen();
    }
  return;
}

void stopAHU_Manual_Mode()
{
  changeAHUStatus(AHU_TURNING_OFF);
  //Turn on A/M Relay, Fire supply and Fire Damper
  if(AHUSetting[fireDampSupCtrl] == 1 || AHUSetting[fireDampRetCtrl] == 1)
    {
      if(AHUSetting[fireDampSupCtrl] == 1)
        outputs[RO_INDEX] = outputs[RO_INDEX] & FIRE_DAMPER_SUPPLY_OFF;
      if(AHUSetting[fireDampRetCtrl] == 1)
        outputs[RO_INDEX] = outputs[RO_INDEX] & FIRE_DAMPER_RETURN_OFF;
      //Switch Fire Damper Relays On and Check Their DIs after 30s
      for(int i = 0; i <= 30; i++)
        updatePIC(1000);
    }
  outputs[RO_INDEX] = outputs[RO_INDEX] & AHU_RELAY_OFF;
  outputs[RO_INDEX] = outputs[RO_INDEX] & AM_RELAY_OFF;
  tempPIDTurnOff();
  if(AHUSetting[rhCtrl] == 1)
    humPIDTurnOff();
  if(VFD[enabled] == 1)
    {
      outputs[VFD_AO] = 0;
    }
  updatePIC(1000);
  AHUSetting[ahuStatus] = AHU_OFF;
  changeAHUStatus(AHU_OFF);
  setAHUSettingsValues();
}

void startAHU()
{
  bool doorLocked = true;
  bool fireSupply = true;
  bool fireReturn = true;
  
  changeAHUStatus(AHU_TURNING_ON);
  
  updatePIC(1000);
  
  if(inputs[AHU_Door] == 1)
    {
      if(AHUSetting[fireDampSupCtrl] == 1 || AHUSetting[fireDampRetCtrl] == 1)
        {
          //If Fire Damper Control = “Y”
          if(AHUSetting[fireDampSupCtrl] == 1)
            {
              outputs[RO_INDEX] = outputs[RO_INDEX] | FIRE_DAMPER_SUPPLY_ON;
              fireSupply = false;
            }
          if(AHUSetting[fireDampRetCtrl] == 1)
            {
              outputs[RO_INDEX] = outputs[RO_INDEX] | FIRE_DAMPER_RETURN_ON;
              fireReturn = false;
            }
    
          //Switch Fire Damper Relays On and Check Their DIs after 30s
          for(int i = 0; i <= 30; i++)
            updatePIC(1000);
          
          //Check fire damper DIs
          if(AHUSetting[fireDampSupCtrl] == 1 && inputs[Fire_Supply] == 1)
            {
              fireSupply = true;
            }
          else
            {
              stopAHU();
              setAHUWarning(FIRE_DAMPER_SUPPLY_OPEN_FAILED,0);
              getWarningAck();
              homeScreen();
              return;
            }
          if(AHUSetting[fireDampRetCtrl] == 1 && inputs[Fire_Return] == 1)
            {
              fireReturn = true;
            }
          else
            {
              stopAHU();
              setAHUWarning(FIRE_DAMPER_RETURN_OPEN_FAILED,0);
              getWarningAck();
              homeScreen();
              return;
            }
        }
    }
  else
    {
      doorLocked = false;
      setAHUWarning(AHU_DOOR_OPEN,0);
      buzzerOn();
      getWarningAck();
      buzzerOff();
      while(inputs[AHU_Door] == 0)
        {
          //Remain here until door is open
          updatePIC(1000);
          if(AHUSetting[ahuMode] == 3)
            setInputsSCADA();
        }
      changeAHUStatus(AHU_OFF);
      homeScreen();
      return;
    }

  if(doorLocked == true && fireSupply == true && fireReturn == true)
    {
      changeAHUStatus(AHU_COUNTDOWN);
      setCountDown();
      //Turn on AHU
      outputs[RO_INDEX] = outputs[RO_INDEX] | AHU_RELAY_ON;
      updatePIC(1000);
      changeAHUStatus(AHU_TURNING_ON);
      int i = 0;
      while((i < 20) && (inputs[AHU_On_Off] == 0))
        {
          updatePIC(1000);
          if(AHUSetting[ahuMode] == 3)
            setInputsSCADA();
          i++;
        }
      if(inputs[AHU_On_Off] == 1)
        {
          //Turn on PIDs
          tempPIDTurnOn();
          if(AHUSetting[rhCtrl] == 1)
            humPIDTurnOn();
          if(VFD[enabled] == 1)
            vfdPIDTurnOn();
          AHUSetting[ahuStatus] = AHU_ON;
          changeAHUStatus(AHU_ON);
          setAHUSettingsValues();
          return;
        }
      else
        {
          //Turn off AHU
          stopAHU();
          setAHUWarning(AHU_FAIL_TO_START,0);
          getWarningAck();
          homeScreen();
          return;
        }
    }
}

void stopAHU()
{
  changeAHUStatus(AHU_TURNING_OFF);
  
  if(AHUSetting[fireDampSupCtrl] == 1 || AHUSetting[fireDampRetCtrl] == 1)
    {
      if(AHUSetting[fireDampSupCtrl] == 1)
        {
          outputs[RO_INDEX] = outputs[RO_INDEX] & FIRE_DAMPER_SUPPLY_OFF;
        }
        
      if(AHUSetting[fireDampRetCtrl] == 1)
        {
          outputs[RO_INDEX] = outputs[RO_INDEX] & FIRE_DAMPER_RETURN_OFF;
        }
      
      //Switch Fire Damper Relays Off and Check Their DIs after 30s
      for(int i = 0; i <= 30; i++)
        updatePIC(1000);
        
      if(AHUSetting[fireDampSupCtrl] == 1 && inputs[Fire_Supply] == 1)
        {
          setAHUWarning(FIRE_DAMPER_SUPPLY_CLOSE_FAILED,0);
          getWarningAck();
          homeScreen();
        }
        
      if(AHUSetting[fireDampRetCtrl] == 1 && inputs[Fire_Return] == 1)
        {
          setAHUWarning(FIRE_DAMPER_RETURN_CLOSE_FAILED,0);
          getWarningAck();
          homeScreen();
        }
    
    }
  outputs[RO_INDEX] = outputs[RO_INDEX] & AHU_RELAY_OFF;
  updatePIC(1500);
  tempPIDTurnOff();
  if(AHUSetting[rhCtrl] == 1)
    humPIDTurnOff();
  if(VFD[enabled] == 1)
    vfdPIDTurnOff();
  AHUSetting[ahuStatus] = AHU_OFF;
  changeAHUStatus(AHU_OFF);
  setAHUSettingsValues();
}

void AHUUpdate()
{
  checkAHUAlarms();
  if(AHUSetting[ahuStatus] == AHU_ON)
    {
      RATempPIDRuntime();
      if(AHUSetting[rhCtrl] == 1)
        RAHumPIDRuntime();
    }
  if(AHUSetting[ahuMode] == 2)
    {
      if(inputs[AHU_On_Off] == 1)
        {
          if(AHUSetting[ahuStatus] == AHU_OFF)
            {
              startAHU_Manual_Mode();
            }
        }
      else
        {
          if(AHUSetting[ahuStatus] == AHU_ON)
            {
              stopAHU_Manual_Mode();
            }
        }
    }
}

void changeAHUStatus(int st)
{
  myNextion.setComponentValue(AHUStatus, st);
  AHUSetting[ahuStatus] = st;
  if(AHUSetting[ahuStatus] == AHU_ON || AHUSetting[ahuStatus] == AHU_OFF)
    {
      prefs.putBool("AHUSetting", true);
      prefs.putBytes("AHUSet", AHUSetting, sizeof(AHUSetting));
    }
  if(AHUSetting[ahuMode] == 3)
    {
      if(AHUSetting[ahuStatus] == AHU_ON || AHUSetting[ahuStatus] == AHU_OFF)
        {
          SCADA_DATA[SCADA_ACK] = 0;
          SCADA_DATA[SCADA_AHU_STATUS] = st;
          setWarningSCADA();
        }
    }
}

void setAHUWarning(int warning, int counts)
{
  homeScreen();
  delay(250);
  myNextion.setComponentValue(WarningNo, warning);
  myNextion.setComponentValue(Reading, counts);
  if(AHUSetting[ahuMode] == 3)
    {
      SCADA_DATA[SCADA_ACK] = 1;
      SCADA_DATA[SCADA_WARNING] = warning;
      SCADA_DATA[SCADA_WARNING_COUNTS] = counts;
      setWarningSCADA();
    }
}

void getWarningAck()
{
  if(AHUSetting[ahuMode] == 3)
    {
      getWarningSCADA();
      while(SCADA_DATA[SCADA_ACK] == 256)
        {
          getWarningSCADA();
          updatePIC(1000);
          setInputsSCADA();
        }
      return;
    }
  while(nextion.available() <= 0)
    {
      while(nextion.available()) 
        {
          // get the new byte:
          char inChar = (char)nextion.read();
          if(inChar == 'y') 
            {
              return;
            }
        }
    }
}

bool getWarningAckWithinTime(int t)
{
  if(AHUSetting[ahuMode] == 3)
    {
      getWarningSCADA();
      while(SCADA_DATA[SCADA_ACK] == 256)
        {
          getWarningSCADA();
          t = t - 150;
          if(t <= 0)
            return false;
        }
      return true;
    }
  while(nextion.available() <= 0)
    {
      while(nextion.available()) 
        {
          // get the new byte:
          char inChar = (char)nextion.read();
          if(inChar == 'y') 
            {
              return true;
            }
        }
      delay(1);
      t--;
      if(t <= 0)
        return false;
    }
}

void buzzerOn()
{
  outputs[RO_INDEX] = outputs[RO_INDEX] | BUZZER_ON;
  updatePIC(10);
}

void buzzerOff()
{
  outputs[RO_INDEX] = outputs[RO_INDEX] & BUZZER_OFF;
  updatePIC(10);
}

void checkAHUAlarms()
{
  if(inputs[AHU_Door] == 0)
    {
      if(AHUSetting[ahuStatus] == AHU_ON)
        {
          if(AHUSetting[ahuMode] == 2)
            stopAHU_Manual_Mode();
          else
            stopAHU();
        }
      setAHUWarning(AHU_DOOR_OPEN, 0);
      buzzerOn();
      getWarningAck();
      buzzerOff();
      while(inputs[AHU_Door] == 0)
        {
          //Remain here until door is open
          updatePIC(1000);
          if(AHUSetting[ahuMode] == 3)
            setInputsSCADA();
        }
      homeScreen();
    }
  
  if(AHUSetting[ahuStatus] == AHU_ON)
    {
      //AHU Trip checking
      if(inputs[AHU_Trip] == 1)
        {
          if(AHUSetting[ahuMode] == 2)
            stopAHU_Manual_Mode();
          else
            stopAHU();
          setAHUWarning(AHU_TRIPPED, 0);
          buzzerOn();
          getWarningAck();
          buzzerOff();
          while(inputs[AHU_Trip] == 1)
            {
              //Remain here until trip is reset
              updatePIC(1000);
              if(AHUSetting[ahuMode] == 3)
                setInputsSCADA();
            }
          homeScreen();
        }
      else if(inputs[AHU_On_Off] == 0)
        {
          if(AHUSetting[ahuMode] == 2)
            {
              stopAHU_Manual_Mode();
              return;
            }
          else
            stopAHU();
          setAHUWarning(AHU_FAILED_DURING_RUNNING, 0);
          buzzerOn();
          getWarningAck();
          buzzerOff();
          homeScreen();
        }
      else if(AHUSetting[fireAlarmIn] == 1)
        {
          if(inputs[Fire_Input] == 0)
            {
              if(AHUSetting[ahuMode] == 2)
                stopAHU_Manual_Mode();
              else
                stopAHU();
              setAHUWarning(FIRE_ALARM, 0);
              buzzerOn();
              getWarningAck();
              buzzerOff();
              while(inputs[Fire_Input] == 0)
                {
                  //Remain here until fire input is reset
                  updatePIC(1000);
                  if(AHUSetting[ahuMode] == 3)
                    setInputsSCADA();
                }
              homeScreen();
            }
        }
      else if(AHUSetting[fireDampSupCtrl] == 1)
        {
          if(inputs[Fire_Supply] == 0)
            {
              if(AHUSetting[ahuMode] == 2)
                stopAHU_Manual_Mode();
              else
                stopAHU();
              setAHUWarning(FIRE_DAMPER_SUPPLY_CLOSED, 0);
              buzzerOn();
              getWarningAck();
              buzzerOff();
              homeScreen();
            }
        }
      else if(AHUSetting[fireDampRetCtrl] == 1)
        {
          if(inputs[Fire_Return] == 0)
            {
              if(AHUSetting[ahuMode] == 2)
                stopAHU_Manual_Mode();
              else
                stopAHU();
              setAHUWarning(FIRE_DAMPER_RETURN_CLOSED, 0);
              buzzerOn();
              getWarningAck();
              buzzerOff();
              homeScreen();
            }
        }
    }
    
  if(inputs[Filter_1] == 1)
    {
      if(millis() - filter_1_last_check_time >= filter_1_checking_interval)
        {
          filter_1_last_check_time = millis();
          setAHUWarning(FILTER1_CLOGGED, 0);
          buzzerOn();
          bool acknowledge = getWarningAckWithinTime(2000);
          if(acknowledge == true)
            filter_1_checking_interval = 86400000;  //Set to next 24 hr.
          else
            filter_1_checking_interval = 10000;
          buzzerOff();
          homeScreen();
        }
    }
  else
    filter_1_checking_interval = 10000;
    
  if(inputs[Filter_2] == 1)
    {
      if(millis() - filter_2_last_check_time >= filter_2_checking_interval)
        {
          filter_2_last_check_time = millis();
          setAHUWarning(FILTER2_CLOGGED, 0);
          buzzerOn();
          bool acknowledge = getWarningAckWithinTime(2000);
          if(acknowledge == true)
            filter_2_checking_interval = 86400000;  //Set to next 24 hr.
          else
            filter_2_checking_interval = 10000;
          buzzerOff();
          homeScreen();
        }
    }
  else
    filter_2_checking_interval = 10000;

  if(AHUSetting[hepaDpt] == 1)
    {
      if(inputs[HEPA_Filter] >= AHUSetting[hepaWarn])
        {
          if(millis() - hepa_last_check_time >= hepa_checking_interval)
            {
              hepa_last_check_time = millis();
              if(inputs[HEPA_Filter] >= AHUSetting[hepaClog])
                setAHUWarning(HEPA_CLOGGED, inputs[HEPA_Filter]);
              else
                setAHUWarning(HEPA_NEAR_CLOGGING, inputs[HEPA_Filter]);
              buzzerOn();
              bool acknowledge = getWarningAckWithinTime(3000);
              if(acknowledge == true)
                hepa_checking_interval = 86400000;  //Set to next 24 hr.
              else
                hepa_checking_interval = 30000;
              buzzerOff();
              homeScreen();
            }
        }
    }
  else
    hepa_checking_interval = 30000;

  if(AHUSetting[fineDpt] == 1)
    {
      if(inputs[Fine_Filter] >= AHUSetting[fineWarn])
        {
          if(millis() - fine_last_check_time >= fine_checking_interval)
            {
              fine_last_check_time = millis();
              if(inputs[Fine_Filter] >= AHUSetting[fineClog])
                setAHUWarning(FINE_CLOGGED, inputs[Fine_Filter]);
              else
                setAHUWarning(FINE_NEAR_CLOGGING, inputs[Fine_Filter]);
              buzzerOn();
              bool acknowledge = getWarningAckWithinTime(3000);
              if(acknowledge == true)
                fine_checking_interval = 86400000;  //Set to next 24 hr.
              else
                fine_checking_interval = 30000;
              buzzerOff();
              homeScreen();
            }
        }
    }
  else
    fine_checking_interval = 30000;

  if(CO2[enabled] == 1)
    {
      int currentCO2Level = 0;
      currentCO2Level = map(inputs[CO2_AI], 0, ADC_MAX_COUNTS, CO2[zero], CO2[full]);
      if(millis() - co2_last_check_time >= co2_checking_interval)
        {
          co2_last_check_time = millis();
          if(currentCO2Level >= AHUSetting[co2Alarm])
            {
              setAHUWarning(CO2_LEVEL_HIGH, currentCO2Level);
              buzzerOn();
              bool acknowledge = getWarningAckWithinTime(3000);
              if(acknowledge == true)
                co2_checking_interval = 86400000;  //Set to next 24 hr.
              else
                co2_checking_interval = 30000;
              buzzerOff();
              homeScreen();
            } 
          else if(currentCO2Level >= AHUSetting[co2High])
            {
              //Open Fresh Air Damper
              outputs[RO_INDEX] = outputs[RO_INDEX] | FRESH_AIR_ON;
              updatePIC(10);
            }
          else if(currentCO2Level <= AHUSetting[co2Low])
            {
              //Close Fresh Air Damper
              outputs[RO_INDEX] = outputs[RO_INDEX] & FRESH_AIR_OFF;
              updatePIC(10);
            }
        }
    }
}
