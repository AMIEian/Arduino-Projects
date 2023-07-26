void getUserSettingsValues()
{
  UserSetting[setTemp] = myNextion.getComponentValue(SetTemp);
  UserSetting[setHum] = myNextion.getComponentValue(SetHum);
  UserSetting[setVFD] = myNextion.getComponentValue(VFDSpeed);

  prefs.putBool("User", true);
  prefs.putBytes("USet", UserSetting, sizeof(UserSetting));
  
  for(int i = 0; i < 3; i++)
    UserSetting[i] = 99;

  prefs.getBytes("USet", UserSetting, sizeof(UserSetting));
    
  for(int i = 0; i < 3; i++)
    SerialBT.println(UserSetting[i]);
}

void setUserSettingsValues()
{
  bool saved = prefs.getBool("User");
  if(saved == true)
    {
      SerialBT.println("User Settings Found...");
      SerialBT.println("Setting them to display...");
      prefs.getBytes("USet", UserSetting, sizeof(UserSetting));

      myNextion.setComponentValue(SetTemp, UserSetting[setTemp]);
      myNextion.setComponentValue(SetHum, UserSetting[setHum]);
    }
  else
    {
      UserSetting[setTemp] = 250;
      
      if(AHUSetting[rhCtrl] == 1)
        UserSetting[setHum] = 550;
      else
        UserSetting[setHum] = 0;
        
      if(VFD[enabled] == 1)
        UserSetting[setVFD] = 100;
      else
        UserSetting[setVFD] = 0;
    }
}
