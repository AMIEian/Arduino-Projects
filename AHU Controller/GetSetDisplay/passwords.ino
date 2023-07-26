//PASSWORDS ARRAYS
int16_t PASSWORD[3];

void getPasswordsValues()
{
  PASSWORD[userPin] = myNextion.getComponentValue(UserPin);
  PASSWORD[factoryPin] = myNextion.getComponentValue(FactoryPin);
  PASSWORD[maintainPin] = myNextion.getComponentValue(MaintainPin);
}

void setPasswordsValues()
{
  myNextion.setComponentValue(UserPin, PASSWORD[userPin]);
  myNextion.setComponentValue(FactoryPin, PASSWORD[factoryPin]);
  myNextion.setComponentValue(MaintainPin, PASSWORD[maintainPin]);
}
