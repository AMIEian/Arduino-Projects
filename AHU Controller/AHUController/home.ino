int oldTemp, oldHum;

void setCurrentTemp()
{
  if(inputs[SA_Temp] != oldTemp)
    {
      myNextion.setComponentValue(CurrentTemp, inputTemperature);
      oldTemp = inputTemperature;
    }
}

void setCurrentHum()
{
  if(inputs[SA_Humidity] != oldHum)
    {
      myNextion.setComponentValue(CurrentHum, inputHumidity);
      oldHum = inputHumidity;
    }
}

void setCountDown()
{
  for(int i = 10; i >= 0; i--)
    {
      myNextion.setComponentValue(SecCountDown, i);
      delay(1000);
    }
}

void updateHomeScreen()
{
  setCurrentTemp();
  setCurrentHum();
}

void homeScreen()
{
  myNextion.init();
  myNextion.setComponentValue(CurrentTemp, inputTemperature);
  myNextion.setComponentValue(CurrentHum, inputHumidity);
}

void homeRefresh()
{
  //myNextion.init();
  myNextion.setComponentValue(CurrentTemp, inputTemperature);
  myNextion.setComponentValue(CurrentHum, inputHumidity);
}
