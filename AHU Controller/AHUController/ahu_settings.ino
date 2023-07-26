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
  if(result != 0)
    AHUSetting[trhSens] = 1; //0 = EXT, 1 = INT
  else
    AHUSetting[trhSens] = 0;

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  if(result != 0)
    AHUSetting[fineDpt] = 1; //0 = N, 1 = Y
  else
    AHUSetting[fineDpt] = 0;

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  if(result != 0)
    AHUSetting[hepaDpt] = 1; //0 = N, 1 = Y
  else
    AHUSetting[hepaDpt] = 0;

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  if(result != 0)
    AHUSetting[rhCtrl] = 1; //0 = N, 1 = Y
  else
    AHUSetting[rhCtrl] = 0;

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  if(result != 0)
    AHUSetting[vfdSpeedCtrl] = 1; //0 = N, 1 = Y
  else
    AHUSetting[vfdSpeedCtrl] = 0;
    
  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  if(result != 0)
    AHUSetting[freshAirDampCtrl] = 1; //0 = N, 1 = Y
  else
    AHUSetting[freshAirDampCtrl] = 0;

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  if(result != 0)
    AHUSetting[fireDampSupCtrl] = 1; //0 = N, 1 = Y
  else
    AHUSetting[fireDampSupCtrl] = 0;

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  if(result != 0)
    AHUSetting[fireDampRetCtrl] = 1; //0 = N, 1 = Y
  else
    AHUSetting[fireDampRetCtrl] = 0;

  mask = mask << 1;
  result = AHUSetting[internalPara] & mask;
  if(result != 0)
    AHUSetting[fireAlarmIn] = 1; //0 = N, 1 = Y
  else
    AHUSetting[fireAlarmIn] = 0;

  prefs.putBool("AHUSetting", true);
  prefs.putBytes("AHUSet", AHUSetting, sizeof(AHUSetting));

  for(int i = 0; i < 20; i++)
    AHUSetting[i] = 99;

  prefs.getBytes("AHUSet", AHUSetting, sizeof(AHUSetting));
    
  for(int i = 0; i < 20; i++)
    SerialBT.println(AHUSetting[i]);
}

void setAHUSettingsValues()
{
  bool saved = prefs.getBool("AHUSetting");
  if(saved == true)
    {
      SerialBT.println("AHU Settings Found...");
      SerialBT.println("Setting them to display...");
      
      prefs.getBytes("AHUSet", AHUSetting, sizeof(AHUSetting));

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
  else
    {
      AHUSetting[ahuMode] = 1;
      AHUSetting[ahuStatus] = 0;
      AHUSetting[vfdSpeed] = 0;
      AHUSetting[fineWarn] = 0;
      AHUSetting[fineClog] = 0;
      AHUSetting[hepaWarn] = 0;
      AHUSetting[hepaClog] = 0;
      AHUSetting[co2Low] = 0;
      AHUSetting[co2High] = 0;
      AHUSetting[co2Alarm] = 0;
      AHUSetting[internalPara] = 1;
      AHUSetting[trhSens] = 1;
      AHUSetting[fineDpt] = 0;
      AHUSetting[hepaDpt] = 0;
      AHUSetting[rhCtrl] = 0;
      AHUSetting[vfdSpeedCtrl] = 0;
      AHUSetting[freshAirDampCtrl] = 0;
      AHUSetting[fireDampSupCtrl] = 0;
      AHUSetting[fireDampRetCtrl] = 0;
      AHUSetting[fireAlarmIn] = 0;
    }
}
