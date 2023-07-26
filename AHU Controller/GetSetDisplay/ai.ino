//ANALOG INPUTS ARRAYS - Return Air Temperature
int16_t RATemp[4];
int16_t RATempPID[8];

//ANALOG INPUTS ARRAYS - Return Air Humidity
int16_t RAHum[4];
int16_t RAHumPID[8];

//ANALOG INPUTS ARRAYS - VFD
int16_t VFD[4];
int16_t VFDPID[8];

//ANALOG INPUTS ARRAYS - Air Velocity Sensor
int16_t AVS[4];

//ANALOG INPUTS ARRAYS - CO2 Sensor
int16_t CO2[4];

void getRATempValues()
{
  RATemp[enabled] = myNextion.getComponentValue(RATempEnabled);
  RATemp[dt] = myNextion.getComponentValue(RATempDt);
  RATemp[zero] = myNextion.getComponentValue(RATempZero);
  RATemp[full] = myNextion.getComponentValue(RATempFull);

  outputs[RA_Temp_AI_Damp_Time] = RATemp[dt];
  outputs[RA_Temp_Lower] = RATemp[zero];
  outputs[RA_Temp_Higher] = RATemp[full];
}

void setRATempValues()
{
  myNextion.setComponentValue(RATempEnabled, RATemp[enabled]);
  myNextion.setComponentValue(RATempDt, RATemp[dt]);
  myNextion.setComponentValue(RATempZero, RATemp[zero]);
  myNextion.setComponentValue(RATempFull, RATemp[full]);

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
}

void setRATempPIDValues()
{
  myNextion.setComponentValue(RATempPIDMode, RATempPID[pidMode]);
  myNextion.setComponentValue(RATempPIDKp, RATempPID[kp]);
  myNextion.setComponentValue(RATempPIDKi, RATempPID[ki]);
  myNextion.setComponentValue(RATempPIDKd, RATempPID[kd]);
  myNextion.setComponentValue(RATempPIDKt, RATempPID[kt]);
  myNextion.setComponentValue(RATempPIDRamp, RATempPID[ramp]);
  myNextion.setComponentValue(RATempPIDLower, RATempPID[lower]);
  myNextion.setComponentValue(RATempPIDUpper, RATempPID[upper]);
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
}

void setRAHumValues()
{
  myNextion.setComponentValue(RAHumEnabled, RAHum[enabled]);
  myNextion.setComponentValue(RAHumDt, RAHum[dt]);
  myNextion.setComponentValue(RAHumZero, RAHum[zero]);
  myNextion.setComponentValue(RAHumFull, RAHum[full]);

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
}

void setRAHumPIDValues()
{
  myNextion.setComponentValue(RAHumPIDMode, RAHumPID[pidMode]);
  myNextion.setComponentValue(RAHumPIDKp, RAHumPID[kp]);
  myNextion.setComponentValue(RAHumPIDKi, RAHumPID[ki]);
  myNextion.setComponentValue(RAHumPIDKd, RAHumPID[kd]);
  myNextion.setComponentValue(RAHumPIDKt, RAHumPID[kt]);
  myNextion.setComponentValue(RAHumPIDRamp, RAHumPID[ramp]);
  myNextion.setComponentValue(RAHumPIDLower, RAHumPID[lower]);
  myNextion.setComponentValue(RAHumPIDUpper, RAHumPID[upper]);
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
}

void setVFDValues()
{
  myNextion.setComponentValue(VFDEnabled, VFD[enabled]);
  myNextion.setComponentValue(VFDDt, VFD[dt]);
  myNextion.setComponentValue(VFDZero, VFD[zero]);
  myNextion.setComponentValue(VFDFull, VFD[full]);

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
}

void setVFDPIDValues()
{
  myNextion.setComponentValue(VFDPIDMode, VFDPID[pidMode]);
  myNextion.setComponentValue(VFDPIDKp, VFDPID[kp]);
  myNextion.setComponentValue(VFDPIDKi, VFDPID[ki]);
  myNextion.setComponentValue(VFDPIDKd, VFDPID[kd]);
  myNextion.setComponentValue(VFDPIDKt, VFDPID[kt]);
  myNextion.setComponentValue(VFDPIDRamp, VFDPID[ramp]);
  myNextion.setComponentValue(VFDPIDLower, VFDPID[lower]);
  myNextion.setComponentValue(VFDPIDUpper, VFDPID[upper]);
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
}

void setAVSValues()
{
  myNextion.setComponentValue(AVSEnabled, AVS[enabled]);
  myNextion.setComponentValue(AVSDt, AVS[dt]);
  myNextion.setComponentValue(AVSZero, AVS[zero]);
  myNextion.setComponentValue(AVSFull, AVS[full]);

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
}

void setCO2Values()
{
  myNextion.setComponentValue(CO2Enabled, CO2[enabled]);
  myNextion.setComponentValue(CO2Dt, CO2[dt]);
  myNextion.setComponentValue(CO2Zero, CO2[zero]);
  myNextion.setComponentValue(CO2Full, CO2[full]);

  outputs[CO2_AI_Damp_Time] = CO2[dt];
  outputs[CO2_Lower] = CO2[zero];
  outputs[CO2_Higher] = CO2[full];
}
