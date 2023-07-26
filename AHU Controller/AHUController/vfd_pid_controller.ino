//Define Variables we'll be connecting to
double vfdSetpoint, vfdInput, vfdOutput;

//Specify the links and initial tuning parameters
double vfdKp=0, vfdKi=5, vfdKd=0;
PID vfdPID(&vfdInput, &vfdOutput, &vfdSetpoint, vfdKp, vfdKi, vfdKd, REVERSE);

int vfdWindowSize = 5000;
unsigned long vfdWindowStartTime;

byte vfd_ATuneModeRemember=2;

double vfd_aTuneStep=2500, vfd_aTuneNoise=1, vfd_aTuneStartValue=5000;
unsigned int vfd_aTuneLookBack=10;

boolean vfd_tuning = true;

PID_ATune vfd_aTune(&vfdInput, &vfdOutput);

void vfdPIDTurnOn()
{
  float t = UserSetting[setVFD] / (VFD[full] * 10);
  vfdSetpoint = t * ADC_MAX_COUNTS;
  
  vfdInput = inputs[VFD_Speed_AI];
  vfdOutput = outputs[VFD_AO];
  
  if(VFDPID[pidMode] == 0)
    {
      //AUTO TUNNING MODE
      SerialBT.print("Auto Tunning Mode...");
      vfdPID.SetOutputLimits(0, DAC_MAX_COUNTS);
      vfd_tuning = prefs.getBool("vfdTun");
      if(vfd_tuning == false)
        {
          SerialBT.print("Performing Auto Tunning...");
          vfd_tuning = false;
          vfd_changeAutoTune();
          vfd_tuning = true;
        }
      else
        {
          vfd_tuning = false;
          vfdKp = prefs.getDouble("vfdKp");
          vfdKi = prefs.getDouble("vfdKi");
          vfdKd = prefs.getDouble("vfdKd");
          vfdPID.SetTunings(vfdKp, vfdKi, vfdKd);
          SerialBT.print("Old Auto Tunning Values Found...");
        }
    }
  else if(VFDPID[pidMode] == 1)
    {
      //MANUAL TUNNING MODE
      SerialBT.print("Manual Tunning Mode...");
      vfdPID.SetOutputLimits(0, DAC_MAX_COUNTS);
      vfd_tuning = false;
      prefs.putBool("vfdTun", false);
      vfdKp = (float)VFDPID[kp]/10.0;
      vfdKi = (float)VFDPID[ki]/10.0;
      vfdKd = (float)VFDPID[kd]/10.0;
      vfdPID.SetTunings(vfdKp, vfdKi, vfdKd);
    }
  else if(VFDPID[pidMode] == 2)
    {
      //ON-OFF PID
      SerialBT.print("On Off PID Control...");
      vfd_tuning = false;
      prefs.putBool("vfdTun", false);
      vfdKp = VFDPID[kp]/10;
      vfdKi = VFDPID[ki]/10;
      vfdKd = VFDPID[kd]/10;
      vfdPID.SetTunings(vfdKp, vfdKi, vfdKd);
      //tell the PID to range between 0 and the full window size
      vfdWindowStartTime = millis();
      vfdPID.SetOutputLimits(0, vfdWindowSize);
    }
    
  SerialBT.print("vfdKp"); SerialBT.println(vfdKp); 
  SerialBT.print("vfdKi"); SerialBT.println(vfdKi); 
  SerialBT.print("vfdKd"); SerialBT.println(vfdKd); 
  
  VFDPID[kt] = VFDPID[kt] * 1000;
  if(VFDPID[kt] != 0)
    vfdPID.SetSampleTime(VFDPID[kt]);
  else
    vfdPID.SetSampleTime(1000);
    
  vfdPID.SetMode(AUTOMATIC);
}

void vfdPIDTurnOff()
{
  vfdPID.SetMode(MANUAL);
  outputs[Chilled_Water_AO] = 0;
}

void VFDPIDRuntime()
{
  float t = UserSetting[setVFD] / (VFD[full] * 10);
  vfdSetpoint = t * ADC_MAX_COUNTS;
  
  vfdInput = inputs[VFD_Speed_AI];
  
  if(vfd_tuning)
  {
    byte val = (vfd_aTune.Runtime());
    if (val!=0)
    {
      vfd_tuning = false;
    }
    if(!vfd_tuning)
    { 
      //we're done, set the tuning parameters
      vfdKp = vfd_aTune.GetKp();
      vfdKi = vfd_aTune.GetKi();
      vfdKd = vfd_aTune.GetKd();
      vfdPID.SetTunings(vfdKp, vfdKi, vfdKd);
      vfd_AutoTuneHelper(false);

      //Save the values for future operations
      prefs.putBool("vfdTun", true);
      prefs.putDouble("vfdKp", vfdKp);
      prefs.putDouble("vfdKi", vfdKi);
      prefs.putDouble("vfdKd", vfdKd);

      SerialBT.print("Auto Tunning Successful...");
      SerialBT.print("vfdKp"); SerialBT.println(vfdKp); 
      SerialBT.print("vfdKi"); SerialBT.println(vfdKi); 
      SerialBT.print("vfdKd"); SerialBT.println(vfdKd); 
    }
  }
  else
    vfdPID.Compute();
    
  outputs[VFD_AO] = vfdOutput;

  SerialBT.print("VFDSetPoint"); SerialBT.println(vfdSetpoint);
  SerialBT.print("vfdInput"); SerialBT.println(inputs[VFD_Speed_AI]); 
  SerialBT.print("vfdOutput"); SerialBT.println(outputs[VFD_AO]);

  if(VFDPID[pidMode] == 2)
    {
      //ON-OFF PID
      /************************************************
       * turn the output pin on/off based on pid output
       ************************************************/
      if (millis() - vfdWindowStartTime > vfdWindowSize)
      { //time to shift the Relay Window
        vfdWindowStartTime += vfdWindowSize;
      }
      if (vfdOutput < millis() - vfdWindowStartTime) outputs[VFD_AO] = DAC_MAX_COUNTS;  //digitalWrite(RELAY_PIN, HIGH);
      else outputs[VFD_AO] = 0;  //digitalWrite(RELAY_PIN, LOW);
    }
}

void vfd_changeAutoTune()
{
 if(!vfd_tuning)
  {
    //Set the output to the desired starting frequency.
    vfdOutput=vfd_aTuneStartValue;
    vfd_aTune.SetNoiseBand(vfd_aTuneNoise);
    vfd_aTune.SetOutputStep(vfd_aTuneStep);
    vfd_aTune.SetLookbackSec((int)vfd_aTuneLookBack);
    vfd_AutoTuneHelper(true);
    vfd_tuning = true;
  }
  else
  { //cancel autotune
    vfd_aTune.Cancel();
    vfd_tuning = false;
    vfd_AutoTuneHelper(false);
  }
}

void vfd_AutoTuneHelper(boolean start)
{
  if(start)
    vfd_ATuneModeRemember = vfdPID.GetMode();
  else
    vfdPID.SetMode(vfd_ATuneModeRemember);
}
