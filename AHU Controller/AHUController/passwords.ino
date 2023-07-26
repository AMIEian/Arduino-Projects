void getPasswordsValues()
{
  PASSWORD[userPin] = myNextion.getComponentValue(UserPin);
  PASSWORD[factoryPin] = myNextion.getComponentValue(FactoryPin);
  PASSWORD[maintainPin] = myNextion.getComponentValue(MaintainPin);

  prefs.putBool("PAS", true);
  prefs.putBytes("PASS", PASSWORD, sizeof(PASSWORD));

  for(int i = 0; i < 3; i++)
    PASSWORD[i] = 99;

  prefs.getBytes("PASS", PASSWORD, sizeof(PASSWORD));
    
  for(int i = 0; i < 3; i++)
    SerialBT.println(PASSWORD[i]);
}

void setPasswordsValues()
{
  bool saved = prefs.getBool("PAS");
  if(saved == true)
    {
      SerialBT.println("Passwords Settings Found...");
      SerialBT.println("Setting them to display...");
      
      prefs.getBytes("PASS", PASSWORD, sizeof(PASSWORD));

      myNextion.setComponentValue(UserPin, PASSWORD[userPin]);
      myNextion.setComponentValue(FactoryPin, PASSWORD[factoryPin]);
      myNextion.setComponentValue(MaintainPin, PASSWORD[maintainPin]);
    }
  else
    {
      PASSWORD[userPin] = 123456;
      PASSWORD[factoryPin] = 135246;
      PASSWORD[maintainPin] = 654321;
    }
}
