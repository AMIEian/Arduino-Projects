//Define Variables we'll be connecting to
double humSetpoint, humInput, humOutput;

//Specify the links and initial tuning parameters
double humKp=0, humKi=5, humKd=0;
PID humPID(&humInput, &humOutput, &humSetpoint, humKp, humKi, humKd, REVERSE);

int humWindowSize = 5000;
unsigned long humWindowStartTime;

byte hum_ATuneModeRemember=2;

double hum_aTuneStep=2500, hum_aTuneNoise=1, hum_aTuneStartValue=5000;
unsigned int hum_aTuneLookBack=10;

boolean hum_tuning = true;

PID_ATune hum_aTune(&humInput, &humOutput);

void humPIDTurnOn()
{
  humSetpoint = UserSetting[setHum];
  
  humInput = inputHumidity;
  humOutput = outputs[Hot_Water_AO];
  
  if(RAHumPID[pidMode] == 0)
    {
      //AUTO TUNNING MODE
      SerialBT.print("Auto Tunning Mode...");
      humPID.SetOutputLimits(0, DAC_MAX_COUNTS);
      hum_tuning = prefs.getBool("humTun");
      if(hum_tuning == false)
        {
          SerialBT.print("Performing Auto Tunning...");
          hum_tuning = false;
          hum_changeAutoTune();
          hum_tuning = true;
        }
      else
        {
          hum_tuning = false;
          humKp = prefs.getDouble("humKp");
          humKi = prefs.getDouble("humKi");
          humKd = prefs.getDouble("humKd");
          humPID.SetTunings(humKp, humKi, humKd);
          SerialBT.print("Old Auto Tunning Values Found...");
        }
    }
  else if(RAHumPID[pidMode] == 1)
    {
      //MANUAL TUNNING MODE
      SerialBT.print("Manual Tunning Mode...");
      humPID.SetOutputLimits(0, DAC_MAX_COUNTS);
      hum_tuning = false;
      prefs.putBool("humTun", false);
      humKp = (float)RAHumPID[kp]/10.0;
      humKi = (float)RAHumPID[ki]/10.0;
      humKd = (float)RAHumPID[kd]/10.0;
      humPID.SetTunings(humKp, humKi, humKd);
    }
  else if(RAHumPID[pidMode] == 2)
    {
      //ON-OFF PID
      SerialBT.print("On Off PID Control...");
      hum_tuning = false;
      prefs.putBool("humTun", false);
      humKp = RAHumPID[kp]/10;
      humKi = RAHumPID[ki]/10;
      humKd = RAHumPID[kd]/10;
      humPID.SetTunings(humKp, humKi, humKd);
      //tell the PID to range between 0 and the full window size
      humWindowStartTime = millis();
      humPID.SetOutputLimits(0, humWindowSize);
    }
    
  SerialBT.print("humKp"); SerialBT.println(humKp); 
  SerialBT.print("humKi"); SerialBT.println(humKi); 
  SerialBT.print("humKd"); SerialBT.println(humKd); 
  
  RAHumPID[kt] = RAHumPID[kt] * 1000;
  if(RAHumPID[kt] != 0)
    humPID.SetSampleTime(RAHumPID[kt]);
  else
    humPID.SetSampleTime(1000);

  humPID.SetMode(AUTOMATIC);
}

void humPIDTurnOff()
{
  humPID.SetMode(MANUAL);
  outputs[Hot_Water_AO] = 0;
}

void RAHumPIDRuntime()
{
  humSetpoint = UserSetting[setHum];
  humInput = inputHumidity;
  if(hum_tuning)
  {
    byte val = (hum_aTune.Runtime());
    if (val!=0)
    {
      hum_tuning = false;
    }
    if(!hum_tuning)
    { 
      //we're done, set the tuning parameters
      humKp = hum_aTune.GetKp();
      humKi = hum_aTune.GetKi();
      humKd = hum_aTune.GetKd();
      humPID.SetTunings(humKp, humKi, humKd);
      hum_AutoTuneHelper(false);

      //Save the values for future operations
      prefs.putBool("humTun", true);
      prefs.putDouble("humKp", humKp);
      prefs.putDouble("humKi", humKi);
      prefs.putDouble("humKd", humKd);

      SerialBT.print("Auto Tunning Successful...");
      SerialBT.print("humKp"); SerialBT.println(humKp); 
      SerialBT.print("humKi"); SerialBT.println(humKi); 
      SerialBT.print("humKd"); SerialBT.println(humKd); 
    }
  }
  else
    humPID.Compute();
    
  outputs[Hot_Water_AO] = humOutput;

  SerialBT.print("HumSetPoint"); SerialBT.println(UserSetting[setHum]);
  SerialBT.print("humInput"); SerialBT.println(inputHumidity); 
  SerialBT.print("humOutput"); SerialBT.println(outputs[Hot_Water_AO]);

  if(RAHumPID[pidMode] == 2)
    {
      //ON-OFF PID
      /************************************************
       * turn the output pin on/off based on pid output
       ************************************************/
      if (millis() - humWindowStartTime > humWindowSize)
      { //time to shift the Relay Window
        humWindowStartTime += humWindowSize;
      }
      if (humOutput < millis() - humWindowStartTime) outputs[Hot_Water_AO] = DAC_MAX_COUNTS;  //digitalWrite(RELAY_PIN, HIGH);
      else outputs[Hot_Water_AO] = 0;  //digitalWrite(RELAY_PIN, LOW);
    }
}

void hum_changeAutoTune()
{
 if(!hum_tuning)
  {
    //Set the output to the desired starting frequency.
    humOutput=hum_aTuneStartValue;
    hum_aTune.SetNoiseBand(hum_aTuneNoise);
    hum_aTune.SetOutputStep(hum_aTuneStep);
    hum_aTune.SetLookbackSec((int)hum_aTuneLookBack);
    hum_AutoTuneHelper(true);
    hum_tuning = true;
  }
  else
  { //cancel autotune
    hum_aTune.Cancel();
    hum_tuning = false;
    hum_AutoTuneHelper(false);
  }
}

void hum_AutoTuneHelper(boolean start)
{
  if(start)
    hum_ATuneModeRemember = humPID.GetMode();
  else
    humPID.SetMode(hum_ATuneModeRemember);
}
