void setCurrentTemp()
{
  float val;
  int temp;
  val = map(inputs[RA_Temp_AI], 0, ADC_MAX_COUNTS, RATemp[zero], RATemp[full]);
  temp = val * 10;
  myNextion.setComponentValue(CurrentTemp, temp);
}

void setCurrentHum()
{
  float val;
  int hum;
  val = map(inputs[RA_Humidity_AI], 0, ADC_MAX_COUNTS, RAHum[zero], RAHum[full]);
  hum = val * 10;
  myNextion.setComponentValue(CurrentHum, hum);
}

void setCountDown()
{
  for(int i = 10; i >= 0; i--)
    myNextion.setComponentValue(SecCountDown, i);
}

void updateHomeScreen()
{
  setCurrentTemp();
  setCurrentHum();
}
