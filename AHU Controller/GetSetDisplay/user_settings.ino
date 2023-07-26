//User Settings ARRAYS
int16_t UserSetting[2];

void getUserSettingsValues()
{
  UserSetting[setTemp] = myNextion.getComponentValue(SetTemp);
  UserSetting[setHum] = myNextion.getComponentValue(SetHum);
}

void setUserSettingsValues()
{
  myNextion.setComponentValue(SetTemp, UserSetting[setTemp]);
  myNextion.setComponentValue(SetHum, UserSetting[setHum]);
}
