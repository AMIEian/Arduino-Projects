#include <SPI.h>
#include "ads12xx.h"

int  START = 6;
int  CS = 8;
int  DRDY = 2;

int RESET_PIN = 9;

//Define which ADC to use in the ads12xx.h file

ads12xx ADS;

void setup()
{
  Serial.begin(115200);

  Serial.println("Serial OK...");

  pinMode(RESET_PIN, OUTPUT);

  digitalWrite(RESET_PIN,LOW);

  delay(10);

  digitalWrite(RESET_PIN,HIGH);

  delay(10);
  
  ADS.begin(CS, START, DRDY);  //initialize ADS as object of the ads12xx class

  ADS.Reset();

  delay(10);

  //test_continuousRead();
}

void loop() 
  {
    test_continuousRead();
  }

void test_continuousRead() 
{
  int32_t results[16]  = {};
  uint8_t statusByte;
  uint8_t channel;
  uint8_t valid;
  int32_t regData;

  Serial.println("HI");
  
  ADS.SetRegisterValue(CONFIG0, MUXMOD_AUTO | CLKENB_DISABLE | STAT_ENABLE | CHOP_DISABLE | BYPAS_EXT); //CHOP_ON, external bypass on (for putting an external amplifier)
  
  ADS.SetRegisterValue(CONFIG1, IDLMOD_STANDBY | DLY2_on | DLY1_on | DLY0_on | DRATE_0);
  
  ADS.SetRegisterValue(MUXDIF, 0x00);
  
  ADS.SetRegisterValue(MUXSG0, 0xFF);
  
  ADS.SetRegisterValue(MUXSG1, 0xFF);
  
  statusByte = 0;
  while (1) 
    {
      for (uint8_t i = 0; i < 16 ; i++) 
        {
          ADS.GetConversion1258(&statusByte, &regData);
          channel = (statusByte << 5);
          channel = (channel >> 5);
          results[channel] = regData;
        }
      //Serial.print(millis());
      for (uint8_t k = 0; k < 16; k++) 
        {
          Serial.print(" ");
          Serial.print((double)(5.0 * results[k] / (7864320)), 6);
        }
      Serial.println();
  }
}

void test_AIN0() {
  // struct conversion outputFromADC();
  uint8_t statusByte;
  int32_t regData;
  int32_t volt_val = 0;
  ADS.SetRegisterValue(CONFIG0, MUXMOD_FIXED | CLKENB_ENABLE);
  ADS.SetRegisterValue(CONFIG1, IDLMOD_SLEEP | DLY2_on | DLY1_on | DLY0_on | DRATE_0);
  ADS.SetRegisterValue(MUXSCH, AINP1);
  //Data sheet ADS1258 P25 : 1 LSB=V_REF/780000h
  //0x780000=7864320 dec
  ADS.GetConversion1258(&statusByte, &regData);
  volt_val = regData;
  Serial.print("Status byte = ");
  Serial.println(statusByte);
  Serial.println(volt_val, DEC);
  Serial.println(volt_val, HEX);
  double voltage = (5.0 * volt_val / (7864320));
  Serial.print("AIN0 voltage: ");
  Serial.print(voltage, DEC);
  Serial.println(" V");
}

void test_PulseRead() {
  uint8_t statusByte;
  uint8_t channel;
  uint8_t valid;
  int32_t regData;
  ADS.SetRegisterValue(CONFIG0, MUXMOD_AUTO | CLKENB_ENABLE | STAT_ENABLE | CHOP_ENABLE | BYPAS_EXT); //CHOP_ON, external bypass on (for putting an external amplifier)
  ADS.SetRegisterValue(CONFIG1, IDLMOD_SLEEP | DLY2_on | DLY1_on | DLY0_on | DRATE_0);
  ADS.SetRegisterValue(MUXDIF, 0xFF);
  ADS.SetRegisterValue(MUXSG0, 0x00);
  ADS.SetRegisterValue(MUXSG1, 0x00);
  //Data sheet ADS1258 P25 : 1 LSB=V_REF/780000h
  //0x780000=7864320 dec
  for (uint8_t j = 0; j < 255 ; j++) {
    statusByte = 0;
    ADS.GetConversion1258(&statusByte, &regData);
    valid = statusByte >> 7;
    if (valid) { //leftmost bit of
      channel = (statusByte << 5);
      channel = (channel >> 5);
      Serial.print(j, DEC);
      Serial.print(" ");
      Serial.print(channel, DEC);
      Serial.print(" ");
      Serial.print(valid);
      Serial.print(" ");
      double voltage = (5.0 * regData / (7864320));
      Serial.println(voltage, DEC);
    }
  }
}
