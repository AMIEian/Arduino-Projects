//AHU Settings ARRAYS
int16_t AHUSetting[20];

void getAHUSettingsValues()
{
  AHUSetting[ahuMode] = myNextion.getComponentValue(AHUMode);
  AHUSetting[ahuStatus] = myNextion.getComponentValue(AHUStatus);
  AHUSetting[vfdSpeed] = myNextion.getComponentValue(VFDSpeed);
  AHUSetting[fineWarn] = myNextion.getComponentValue(FineWarn);
  AHUSetting[fineClog] = myNextion.getComponentValue(FineClog);
  AHUSetting[hepaWarn] = myNextion.getComponentValue(HepaWarn);
  AHUSetting[hepaClog] = myNextion.getComponentValue(HepaClog);
  AHUSetting[co2Low] = myNextion.getComponentValue(CO2Low);
  AHUSetting[co2High] = myNextion.getComponentValue(CO2High);
  AHUSetting[co2Alarm] = myNextion.getComponentValue(CO2Alarm);
  AHUSetting[internalPara] = myNextion.getComponentValue(InternalPara);

  int mask = 1;
  int result;

  result = AHUSetting[internalPara] & mask;
  AHUSetting[trhSens] = result; //0 = EXT, 1 = INT

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  AHUSetting[fineDpt] = result; //0 = N, 1 = Y

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  AHUSetting[hepaDpt] = result; //0 = N, 1 = Y

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  AHUSetting[rhCtrl] = result; //0 = N, 1 = Y

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  AHUSetting[vfdSpeedCtrl] = result; //0 = N, 1 = Y

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  AHUSetting[freshAirDampCtrl] = result; //0 = N, 1 = Y

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  AHUSetting[fireDampSupCtrl] = result; //0 = N, 1 = Y

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  AHUSetting[fireDampRetCtrl] = result; //0 = N, 1 = Y

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  AHUSetting[fireAlarmIn] = result; //0 = N, 1 = Y
}

void setAHUSettingsValues()
{
  myNextion.setComponentValue(AHUMode, AHUSetting[ahuMode]);
  myNextion.setComponentValue(AHUStatus, AHUSetting[ahuStatus]);
  myNextion.setComponentValue(VFDSpeed, AHUSetting[vfdSpeed]);
  myNextion.setComponentValue(FineWarn, AHUSetting[fineWarn]);
  myNextion.setComponentValue(FineClog, AHUSetting[fineClog]);
  myNextion.setComponentValue(HepaWarn, AHUSetting[hepaWarn]);
  myNextion.setComponentValue(HepaClog, AHUSetting[hepaClog]);
  myNextion.setComponentValue(CO2Low, AHUSetting[co2Low]);
  myNextion.setComponentValue(CO2High, AHUSetting[co2High]);
  myNextion.setComponentValue(CO2Alarm, AHUSetting[co2Alarm]);
  
  int result = 0;
  if(AHUSetting[trhSens] == 0)
      result = result & 0xFFFE;
  else
      result = result | 0x0001;

  if(AHUSetting[fineDpt] == 0)
      result = result & 0xFFFD;
  else
      result = result | 0x0002;

  if(AHUSetting[hepaDpt] == 0)
      result = result & 0xFFFB;
  else
      result = result | 0x0004;

  if(AHUSetting[rhCtrl] == 0)
      result = result & 0xFFF7;
  else
      result = result | 0x0008;

  if(AHUSetting[vfdSpeedCtrl] == 0)
      result = result & 0xFFEF;
  else
      result = result | 0x0010;

  if(AHUSetting[freshAirDampCtrl] == 0)
      result = result & 0xFFDF;
  else
      result = result | 0x0020;

  if(AHUSetting[fireDampSupCtrl] == 0)
      result = result & 0xFFBF;
  else
      result = result | 0x0040;

  if(AHUSetting[fireDampRetCtrl] == 0)
      result = result & 0xFF7F;
  else
      result = result | 0x0080;

  if(AHUSetting[fireAlarmIn] == 0)
      result = result & 0xFEFF;
  else
      result = result | 0x0100;

  AHUSetting[internalPara] = result;
  myNextion.setComponentValue(InternalPara, AHUSetting[internalPara]);
}
