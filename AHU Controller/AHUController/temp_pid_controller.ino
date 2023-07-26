//Define Variables we'll be connecting to
double tempSetpoint, tempInput, tempOutput;

//Specify the links and initial tuning parameters
double tempKp=0, tempKi=5, tempKd=0;
PID tempPID(&tempInput, &tempOutput, &tempSetpoint, tempKp, tempKi, tempKd, REVERSE);

int tempWindowSize = 5000;
unsigned long tempWindowStartTime;

byte temp_ATuneModeRemember=2;

double temp_aTuneStep=2500, temp_aTuneNoise=1, temp_aTuneStartValue=5000;
unsigned int temp_aTuneLookBack=10;

boolean temp_tuning = true;

PID_ATune temp_aTune(&tempInput, &tempOutput);

void tempPIDTurnOn()
{
  tempSetpoint = UserSetting[setTemp];
  tempInput = inputTemperature;
  tempOutput = outputs[Chilled_Water_AO];
  
  if(RATempPID[pidMode] == 0)
    {
      //AUTO TUNNING MODE
      SerialBT.print("Auto Tunning Mode...");
      tempPID.SetOutputLimits(0, DAC_MAX_COUNTS);
      temp_tuning = prefs.getBool("tempTun");
      if(temp_tuning == false)
        {
          SerialBT.print("Performing Auto Tunning...");
          temp_tuning = false;
          temp_changeAutoTune();
          temp_tuning = true;
        }
      else
        {
          temp_tuning = false;
          tempKp = prefs.getDouble("tempKp");
          tempKi = prefs.getDouble("tempKi");
          tempKd = prefs.getDouble("tempKd");
          tempPID.SetTunings(tempKp, tempKi, tempKd);
          SerialBT.print("Old Auto Tunning Values Found...");
        }
    }
  else if(RATempPID[pidMode] == 1)
    {
      //MANUAL TUNNING MODE
      SerialBT.print("Manual Tunning Mode...");
      tempPID.SetOutputLimits(0, DAC_MAX_COUNTS);
      temp_tuning = false;
      prefs.putBool("tempTun", false);
      tempKp = (float)RATempPID[kp]/10.0;
      tempKi = (float)RATempPID[ki]/10.0;
      tempKd = (float)RATempPID[kd]/10.0;
      tempPID.SetTunings(tempKp, tempKi, tempKd);
    }
  else if(RATempPID[pidMode] == 2)
    {
      //ON-OFF PID
      SerialBT.print("On Off PID Control...");
      temp_tuning = false;
      prefs.putBool("tempTun", false);
      tempKp = RATempPID[kp]/10;
      tempKi = RATempPID[ki]/10;
      tempKd = RATempPID[kd]/10;
      tempPID.SetTunings(tempKp, tempKi, tempKd);
      //tell the PID to range between 0 and the full window size
      tempWindowStartTime = millis();
      tempPID.SetOutputLimits(0, tempWindowSize);
    }
    
  SerialBT.print("tempKp"); SerialBT.println(tempKp); 
  SerialBT.print("tempKi"); SerialBT.println(tempKi); 
  SerialBT.print("tempKd"); SerialBT.println(tempKd); 
  
  RATempPID[kt] = RATempPID[kt] * 1000;
  if(RATempPID[kt] != 0)
    tempPID.SetSampleTime(RATempPID[kt]);
  else
    tempPID.SetSampleTime(1000);
    
  tempPID.SetMode(AUTOMATIC);
}

void tempPIDTurnOff()
{
  tempPID.SetMode(MANUAL);
  outputs[Chilled_Water_AO] = 0;
}

void RATempPIDRuntime()
{
  tempSetpoint = UserSetting[setTemp];
  tempInput = inputTemperature;
  if(temp_tuning)
  {
    byte val = (temp_aTune.Runtime());
    if (val!=0)
    {
      temp_tuning = false;
    }
    if(!temp_tuning)
    { 
      //we're done, set the tuning parameters
      tempKp = temp_aTune.GetKp();
      tempKi = temp_aTune.GetKi();
      tempKd = temp_aTune.GetKd();
      tempPID.SetTunings(tempKp, tempKi, tempKd);
      temp_AutoTuneHelper(false);

      //Save the values for future operations
      prefs.putBool("tempTun", true);
      prefs.putDouble("tempKp", tempKp);
      prefs.putDouble("tempKi", tempKi);
      prefs.putDouble("tempKd", tempKd);

      SerialBT.print("Auto Tunning Successful...");
      SerialBT.print("tempKp"); SerialBT.println(tempKp); 
      SerialBT.print("tempKi"); SerialBT.println(tempKi); 
      SerialBT.print("tempKd"); SerialBT.println(tempKd); 
    }
  }
  else
    tempPID.Compute();
    
  outputs[Chilled_Water_AO] = tempOutput;

  SerialBT.print("TempSetPoint"); SerialBT.println(UserSetting[setTemp]);
  SerialBT.print("tempInput"); SerialBT.println(inputTemperature); 
  SerialBT.print("tempOutput"); SerialBT.println(outputs[Chilled_Water_AO]);

  if(RATempPID[pidMode] == 2)
    {
      //ON-OFF PID
      /************************************************
       * turn the output pin on/off based on pid output
       ************************************************/
      if (millis() - tempWindowStartTime > tempWindowSize)
      { //time to shift the Relay Window
        tempWindowStartTime += tempWindowSize;
      }
      if (tempOutput < millis() - tempWindowStartTime) outputs[Chilled_Water_AO] = DAC_MAX_COUNTS;  //digitalWrite(RELAY_PIN, HIGH);
      else outputs[Chilled_Water_AO] = 0;  //digitalWrite(RELAY_PIN, LOW);
    }
}

void temp_changeAutoTune()
{
 if(!temp_tuning)
  {
    //Set the output to the desired starting frequency.
    tempOutput=temp_aTuneStartValue;
    temp_aTune.SetNoiseBand(temp_aTuneNoise);
    temp_aTune.SetOutputStep(temp_aTuneStep);
    temp_aTune.SetLookbackSec((int)temp_aTuneLookBack);
    temp_AutoTuneHelper(true);
    temp_tuning = true;
  }
  else
  { //cancel autotune
    temp_aTune.Cancel();
    temp_tuning = false;
    temp_AutoTuneHelper(false);
  }
}

void temp_AutoTuneHelper(boolean start)
{
  if(start)
    temp_ATuneModeRemember = tempPID.GetMode();
  else
    tempPID.SetMode(temp_ATuneModeRemember);
}
