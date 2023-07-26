void getRATempValues()
{
  RATemp[enabled] = myNextion.getComponentValue(RATempEnabled);
  RATemp[dt] = myNextion.getComponentValue(RATempDt);
  RATemp[zero] = myNextion.getComponentValue(RATempZero);
  RATemp[full] = myNextion.getComponentValue(RATempFull);

  outputs[RA_Temp_AI_Damp_Time] = RATemp[dt];
  outputs[RA_Temp_Lower] = RATemp[zero];
  outputs[RA_Temp_Higher] = RATemp[full];
  
  prefs.putBool("RAT", true);
  prefs.putBytes("RATemp", RATemp, sizeof(RATemp));

  for(int i = 0; i < 3; i++)
    RATemp[i] = 99;

  prefs.getBytes("RATemp", RATemp, sizeof(RATemp));
    
  for(int i = 0; i < 3; i++)
    SerialBT.println(RATemp[i]);
}

void setRATempValues()
{
  bool saved = prefs.getBool("RAT");
  if(saved == true)
    {
      SerialBT.println("RATemp Settings Found...");
      SerialBT.println("Setting them to display...");
      
      prefs.getBytes("RATemp", RATemp, sizeof(RATemp));

      myNextion.setComponentValue(RATempEnabled, RATemp[enabled]);
      myNextion.setComponentValue(RATempDt, RATemp[dt]);
      myNextion.setComponentValue(RATempZero, RATemp[zero]);
      myNextion.setComponentValue(RATempFull, RATemp[full]);
    }
  else
    {
      RATemp[enabled] = 1;
      RATemp[dt] = 3;
      RATemp[zero] = 0;
      RATemp[full] = 50;
    }

  outputs[RA_Temp_AI_Damp_Time] = RATemp[dt];
  outputs[RA_Temp_Lower] = RATemp[zero];
  outputs[RA_Temp_Higher] = RATemp[full];
}

void getRATempPIDValues()
{
  RATempPID[pidMode] = myNextion.getComponentValue(RATempPIDMode);
  RATempPID[kp] = myNextion.getComponentValue(RATempPIDKp);
  RATempPID[ki] = myNextion.getComponentValue(RATempPIDKi);
  RATempPID[kd] = myNextion.getComponentValue(RATempPIDKd);
  RATempPID[kt] = myNextion.getComponentValue(RATempPIDKt);
  RATempPID[ramp] = myNextion.getComponentValue(RATempPIDRamp);
  RATempPID[lower] = myNextion.getComponentValue(RATempPIDLower);
  RATempPID[upper] = myNextion.getComponentValue(RATempPIDUpper);

  prefs.putBool("RATP", true);
  prefs.putBytes("RATPID", RATempPID, sizeof(RATempPID));

  for(int i = 0; i < 8; i++)
    RATempPID[i] = 99;

  prefs.getBytes("RATPID", RATempPID, sizeof(RATempPID));
    
  for(int i = 0; i < 8; i++)
    SerialBT.println(RATempPID[i]);
}

void setRATempPIDValues()
{
  bool saved = prefs.getBool("RATP");
  if(saved == true)
    {
      SerialBT.println("RATempPID Settings Found...");
      SerialBT.println("Setting them to display...");
      
      prefs.getBytes("RATPID", RATempPID, sizeof(RATempPID));

      myNextion.setComponentValue(RATempPIDMode, RATempPID[pidMode]);
      myNextion.setComponentValue(RATempPIDKp, RATempPID[kp]);
      myNextion.setComponentValue(RATempPIDKi, RATempPID[ki]);
      myNextion.setComponentValue(RATempPIDKd, RATempPID[kd]);
      myNextion.setComponentValue(RATempPIDKt, RATempPID[kt]);
      myNextion.setComponentValue(RATempPIDRamp, RATempPID[ramp]);
      myNextion.setComponentValue(RATempPIDLower, RATempPID[lower]);
      myNextion.setComponentValue(RATempPIDUpper, RATempPID[upper]);
    }
  else
    {
      RATempPID[pidMode] = 0; //Auto tune
      RATempPID[kp] = 0;
      RATempPID[ki] = 0;
      RATempPID[kd] = 0;
      RATempPID[kt] = 0;
      RATempPID[ramp] = 0;
      RATempPID[lower] = 0;
      RATempPID[upper] = 0;
    }
}

void getRAHumValues()
{
  RAHum[enabled] = myNextion.getComponentValue(RAHumEnabled);
  RAHum[dt] = myNextion.getComponentValue(RAHumDt);
  RAHum[zero] = myNextion.getComponentValue(RAHumZero);
  RAHum[full] = myNextion.getComponentValue(RAHumFull);

  outputs[RA_Humidity_AI_Damp_Time] = RAHum[dt];
  outputs[RA_Humidity_Lower] = RAHum[zero];
  outputs[RA_Humidity_Higher] = RAHum[full];

  prefs.putBool("RAH", true);
  prefs.putBytes("RAHum", RAHum, sizeof(RAHum));

  for(int i = 0; i < 3; i++)
    RAHum[i] = 99;

  prefs.getBytes("RAHum", RAHum, sizeof(RAHum));
    
  for(int i = 0; i < 3; i++)
    SerialBT.println(RAHum[i]);
}

void setRAHumValues()
{
  bool saved = prefs.getBool("RAH");
  if(saved == true)
    {
      SerialBT.println("RAHum Settings Found...");
      SerialBT.println("Setting them to display...");
      
      prefs.getBytes("RAHum", RAHum, sizeof(RAHum));

      myNextion.setComponentValue(RAHumEnabled, RAHum[enabled]);
      myNextion.setComponentValue(RAHumDt, RAHum[dt]);
      myNextion.setComponentValue(RAHumZero, RAHum[zero]);
      myNextion.setComponentValue(RAHumFull, RAHum[full]);
    }
  else
    {
      RAHum[enabled] = 0;
      RAHum[dt] = 0;
      RAHum[zero] = 0;
      RAHum[full] = 0;
    }
    
  outputs[RA_Humidity_AI_Damp_Time] = RAHum[dt];
  outputs[RA_Humidity_Lower] = RAHum[zero];
  outputs[RA_Humidity_Higher] = RAHum[full];
}

void getRAHumPIDValues()
{
  RAHumPID[pidMode] = myNextion.getComponentValue(RAHumPIDMode);
  RAHumPID[kp] = myNextion.getComponentValue(RAHumPIDKp);
  RAHumPID[ki] = myNextion.getComponentValue(RAHumPIDKi);
  RAHumPID[kd] = myNextion.getComponentValue(RAHumPIDKd);
  RAHumPID[kt] = myNextion.getComponentValue(RAHumPIDKt);
  RAHumPID[ramp] = myNextion.getComponentValue(RAHumPIDRamp);
  RAHumPID[lower] = myNextion.getComponentValue(RAHumPIDLower);
  RAHumPID[upper] = myNextion.getComponentValue(RAHumPIDUpper);

  prefs.putBool("RAHP", true);
  prefs.putBytes("RAHPID", RAHumPID, sizeof(RAHumPID));

  for(int i = 0; i < 8; i++)
    RAHumPID[i] = 99;

  prefs.getBytes("RAHPID", RAHumPID, sizeof(RAHumPID));
    
  for(int i = 0; i < 8; i++)
    SerialBT.println(RAHumPID[i]);
}

void setRAHumPIDValues()
{
  bool saved = prefs.getBool("RAHP");
  if(saved == true)
    {
      SerialBT.println("RAHumPID Settings Found...");
      SerialBT.println("Setting them to display...");
      
      prefs.getBytes("RAHPID", RAHumPID, sizeof(RAHumPID));

      myNextion.setComponentValue(RAHumPIDMode, RAHumPID[pidMode]);
      myNextion.setComponentValue(RAHumPIDKp, RAHumPID[kp]);
      myNextion.setComponentValue(RAHumPIDKi, RAHumPID[ki]);
      myNextion.setComponentValue(RAHumPIDKd, RAHumPID[kd]);
      myNextion.setComponentValue(RAHumPIDKt, RAHumPID[kt]);
      myNextion.setComponentValue(RAHumPIDRamp, RAHumPID[ramp]);
      myNextion.setComponentValue(RAHumPIDLower, RAHumPID[lower]);
      myNextion.setComponentValue(RAHumPIDUpper, RAHumPID[upper]);
    }
  else
    {
      RAHumPID[pidMode] = 0; //Auto tune
      RAHumPID[kp] = 0;
      RAHumPID[ki] = 0;
      RAHumPID[kd] = 0;
      RAHumPID[kt] = 0;
      RAHumPID[ramp] = 0;
      RAHumPID[lower] = 0;
      RAHumPID[upper] = 0;
    }
}

void getVFDValues()
{
  VFD[enabled] = myNextion.getComponentValue(VFDEnabled);
  VFD[dt] = myNextion.getComponentValue(VFDDt);
  VFD[zero] = myNextion.getComponentValue(VFDZero);
  VFD[full] = myNextion.getComponentValue(VFDFull);

  outputs[VFD_AI_Damp_Time] = VFD[dt];
  outputs[VFD_Lower] = VFD[zero];
  outputs[VFD_Higher] = VFD[full];

  prefs.putBool("VFD", true);
  prefs.putBytes("VFDVal", VFD, sizeof(VFD));

  for(int i = 0; i < 3; i++)
    VFD[i] = 99;

  prefs.getBytes("VFDVal", VFD, sizeof(VFD));
    
  for(int i = 0; i < 3; i++)
    SerialBT.println(VFD[i]);
}

void setVFDValues()
{
  bool saved = prefs.getBool("VFD");
  if(saved == true)
    {
      SerialBT.println("VFD Settings Found...");
      SerialBT.println("Setting them to display...");
      
      prefs.getBytes("VFDVal", VFD, sizeof(VFD));

      myNextion.setComponentValue(VFDEnabled, VFD[enabled]);
      myNextion.setComponentValue(VFDDt, VFD[dt]);
      myNextion.setComponentValue(VFDZero, VFD[zero]);
      myNextion.setComponentValue(VFDFull, VFD[full]);
    }
  else
    {
      VFD[enabled] = 0;
      VFD[dt] = 0;
      VFD[zero] = 0;
      VFD[full] = 0;
    }

  outputs[VFD_AI_Damp_Time] = VFD[dt];
  outputs[VFD_Lower] = VFD[zero];
  outputs[VFD_Higher] = VFD[full];
}

void getVFDPIDValues()
{
  VFDPID[pidMode] = myNextion.getComponentValue(VFDPIDMode);
  VFDPID[kp] = myNextion.getComponentValue(VFDPIDKp);
  VFDPID[ki] = myNextion.getComponentValue(VFDPIDKi);
  VFDPID[kd] = myNextion.getComponentValue(VFDPIDKd);
  VFDPID[kt] = myNextion.getComponentValue(VFDPIDKt);
  VFDPID[ramp] = myNextion.getComponentValue(VFDPIDRamp);
  VFDPID[lower] = myNextion.getComponentValue(VFDPIDLower);
  VFDPID[upper] = myNextion.getComponentValue(VFDPIDUpper);

  prefs.putBool("VFDP", true);
  prefs.putBytes("VFDPID", VFDPID, sizeof(VFDPID));

  for(int i = 0; i < 8; i++)
    VFDPID[i] = 99;

  prefs.getBytes("VFDPID", VFDPID, sizeof(VFDPID));
    
  for(int i = 0; i < 8; i++)
    SerialBT.println(VFDPID[i]);
}

void setVFDPIDValues()
{
  bool saved = prefs.getBool("VFDP");
  if(saved == true)
    {
      SerialBT.println("VFDPID Settings Found...");
      SerialBT.println("Setting them to display...");
      
      prefs.getBytes("VFDPID", VFDPID, sizeof(VFDPID));

      myNextion.setComponentValue(VFDPIDMode, VFDPID[pidMode]);
      myNextion.setComponentValue(VFDPIDKp, VFDPID[kp]);
      myNextion.setComponentValue(VFDPIDKi, VFDPID[ki]);
      myNextion.setComponentValue(VFDPIDKd, VFDPID[kd]);
      myNextion.setComponentValue(VFDPIDKt, VFDPID[kt]);
      myNextion.setComponentValue(VFDPIDRamp, VFDPID[ramp]);
      myNextion.setComponentValue(VFDPIDLower, VFDPID[lower]);
      myNextion.setComponentValue(VFDPIDUpper, VFDPID[upper]);
    }
  else
    {
      VFDPID[pidMode] = 0; //Auto tune
      VFDPID[kp] = 0;
      VFDPID[ki] = 0;
      VFDPID[kd] = 0;
      VFDPID[kt] = 0;
      VFDPID[ramp] = 0;
      VFDPID[lower] = 0;
      VFDPID[upper] = 0;
    }
}

void getAVSValues()
{
  AVS[enabled] = myNextion.getComponentValue(AVSEnabled);
  AVS[dt] = myNextion.getComponentValue(AVSDt);
  AVS[zero] = myNextion.getComponentValue(AVSZero);
  AVS[full] = myNextion.getComponentValue(AVSFull);

  outputs[Air_Velocity_AI_Damp_Time] = AVS[dt];
  outputs[Air_Velocity_Lower] = AVS[zero];
  outputs[Air_Velocity_Higher] = AVS[full];

  prefs.putBool("AVS", true);
  prefs.putBytes("AVSVal", AVS, sizeof(AVS));

  for(int i = 0; i < 3; i++)
    AVS[i] = 99;

  prefs.getBytes("AVSVal", AVS, sizeof(AVS));
    
  for(int i = 0; i < 3; i++)
    SerialBT.println(AVS[i]);
}

void setAVSValues()
{
  bool saved = prefs.getBool("AVS");
  if(saved == true)
    {
      SerialBT.println("AV Sensor Settings Found...");
      SerialBT.println("Setting them to display...");
      
      prefs.getBytes("AVSVal", AVS, sizeof(AVS));

      myNextion.setComponentValue(AVSEnabled, AVS[enabled]);
      myNextion.setComponentValue(AVSDt, AVS[dt]);
      myNextion.setComponentValue(AVSZero, AVS[zero]);
      myNextion.setComponentValue(AVSFull, AVS[full]);
    }
  else
    {
      AVS[enabled] = 0;
      AVS[dt] = 0;
      AVS[zero] = 0;
      AVS[full] = 0;
    }

  outputs[Air_Velocity_AI_Damp_Time] = AVS[dt];
  outputs[Air_Velocity_Lower] = AVS[zero];
  outputs[Air_Velocity_Higher] = AVS[full];
}

void getCO2Values()
{
  CO2[enabled] = myNextion.getComponentValue(CO2Enabled);
  CO2[dt] = myNextion.getComponentValue(CO2Dt);
  CO2[zero] = myNextion.getComponentValue(CO2Zero);
  CO2[full] = myNextion.getComponentValue(CO2Full);

  outputs[CO2_AI_Damp_Time] = CO2[dt];
  outputs[CO2_Lower] = CO2[zero];
  outputs[CO2_Higher] = CO2[full];

  prefs.putBool("CO2", true);
  prefs.putBytes("CO2Val", CO2, sizeof(CO2));

  for(int i = 0; i < 3; i++)
    CO2[i] = 99;

  prefs.getBytes("CO2Val", CO2, sizeof(CO2));
    
  for(int i = 0; i < 3; i++)
    SerialBT.println(CO2[i]);
}

void setCO2Values()
{
  bool saved = prefs.getBool("CO2");
  if(saved == true)
    {
      SerialBT.println("CO2 Sensor Settings Found...");
      SerialBT.println("Setting them to display...");
      
      prefs.getBytes("CO2Val", CO2, sizeof(CO2));

      myNextion.setComponentValue(CO2Enabled, CO2[enabled]);
      myNextion.setComponentValue(CO2Dt, CO2[dt]);
      myNextion.setComponentValue(CO2Zero, CO2[zero]);
      myNextion.setComponentValue(CO2Full, CO2[full]);
    }
  else
    {
      CO2[enabled] = 0;
      CO2[dt] = 0;
      CO2[zero] = 0;
      CO2[full] = 0;
    }

  outputs[CO2_AI_Damp_Time] = CO2[dt];
  outputs[CO2_Lower] = CO2[zero];
  outputs[CO2_Higher] = CO2[full];
}

void getAISettings()
{
  getRATempValues();
  getRAHumValues();
  getVFDValues();
  getAVSValues();
  getCO2Values();
}

void setAISettings()
{
  setRATempValues();
  setRAHumValues();
  setVFDValues();
  setAVSValues();
  setCO2Values();
}

void getPIDSettings()
{
  getRATempPIDValues();
  getRAHumPIDValues();
  getVFDPIDValues();
}

void setPIDSettings()
{
  setRATempPIDValues();
  setRAHumPIDValues();
  setVFDPIDValues();
}
