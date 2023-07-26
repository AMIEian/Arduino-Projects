#include <LiquidCrystal.h>

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define MOTOR_ON 1
#define MOTOR_OFF 0

#define POWER_OK 1
#define POWER_FAIL 0

int waitCount = 0;
int powerStatus = 0;
int motorStatus = 0;
int getCurrentStatus = 0;

void setup()
  {
   Serial.begin(1200);
   lcd.begin(16, 2);              // start the library
   lcd.setCursor(0,0);

   delay(2000);
 
  //Send query after every 5 seconds until current status is not received from at motor end
  drawCommStatus();
  lcd.setCursor(0,1);
  Serial.print('#');
  lcd.print(".");
  while(Serial.available() <= 0)
    {
      delay(1);
      waitCount++;
      if(waitCount > 5000)
        {
          waitCount = 0;
          Serial.print('#');
          lcd.print(".");
        }
    }
  
  while(Serial.available()) 
    {
      // get the new byte:
      byte inChar = Serial.read();
      parseData(inChar);
    }
 }
 
void loop()
  {
   lcd_key = read_LCD_buttons();  // read the buttons
   if (lcd_key == btnRIGHT || lcd_key == btnUP) 
    {
      waitCount = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SWITCHING ON...");
      lcd.setCursor(0,1);
      Serial.print('C');
      lcd.print(".");
      while(Serial.available() <= 0)
        {
          delay(1);
          waitCount++;
          if(waitCount > 20000)
            {
              waitCount = 0;
              Serial.print('#');
              lcd.print(".");
            }
        }
      while(Serial.available()) 
        {
          // get the new byte:
          byte inChar = Serial.read();
          parseData(inChar);
        }
      while(lcd_key != btnNONE)
        {
          lcd_key = read_LCD_buttons();  // read the buttons
          delay(10);
        }
    }
    
   if (lcd_key == btnLEFT || lcd_key == btnDOWN) 
    {
      waitCount = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SWITCHING OFF...");
      lcd.setCursor(0,1);
      Serial.print('B');
      lcd.print(".");
      while(Serial.available() <= 0)
        {
          delay(1);
          waitCount++;
          if(waitCount > 20000)
            {
              waitCount = 0;
              Serial.print('#');
              lcd.print(".");
            }
        }
      while(Serial.available()) 
        {
          // get the new byte:
          byte inChar = Serial.read();
          parseData(inChar);
        }
      while(lcd_key != btnNONE)
        {
          lcd_key = read_LCD_buttons();  // read the buttons
          delay(10);
        }
    }

   if (lcd_key == btnSELECT) 
    {
      waitCount = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CHECKING STATUS...");
      lcd.setCursor(0,1);
      Serial.print('#');
      lcd.print(".");
      while(Serial.available() <= 0)
        {
          delay(1);
          waitCount++;
          if(waitCount > 20000)
            {
              waitCount = 0;
              Serial.print('#');
              lcd.print(".");
            }
        }
      while(Serial.available()) 
        {
          // get the new byte:
          byte inChar = Serial.read();
          parseData(inChar);
        }
      while(lcd_key != btnNONE)
        {
          lcd_key = read_LCD_buttons();  // read the buttons
          delay(10);
        }
    }
   delay(10);
  }

void serialEvent() 
  {
    while (Serial.available()) 
      {
        byte inChar = Serial.read();
        parseData(inChar);
      }
  }

// read the buttons
int read_LCD_buttons()
  {
   adc_key_in = analogRead(0);      // read the value from the sensor 
   // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
   // we add approx 50 to those values and check to see if we are close
   if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
   // For V1.1 us this threshold
   /*
   if (adc_key_in < 50)   return btnRIGHT;  
   if (adc_key_in < 250)  return btnUP; 
   if (adc_key_in < 450)  return btnDOWN; 
   if (adc_key_in < 650)  return btnLEFT; 
   if (adc_key_in < 850)  return btnSELECT;  
  */ 
   // For V1.0 comment the other threshold and use the one below:
  
   if (adc_key_in < 50)   return btnRIGHT;  
   if (adc_key_in < 195)  return btnUP; 
   if (adc_key_in < 380)  return btnDOWN; 
   if (adc_key_in < 555)  return btnLEFT; 
   if (adc_key_in < 790)  return btnSELECT;   
  
   return btnNONE;  // when all others fail, return this...
  }

void parseData(byte motorData)
  {
     motorStatus = motorData & 0x01;
     powerStatus = motorData & 0x02;

     if(powerStatus == 2)
      drawPowerStatus(POWER_OK);
     else
      drawPowerStatus(POWER_FAIL);

     if(motorStatus == 1)
      drawMotorStatus(MOTOR_ON);
     else
      drawMotorStatus(MOTOR_OFF);
  }

void drawCommStatus()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CONNECTING MOTOR");
}

void drawMotorStatus(int motor_status)
{
  lcd.setCursor(0,0);
  if(motor_status == MOTOR_ON)
    lcd.print("MOTOR = ON");
  else
    lcd.print("MOTOR = OFF");
}

void drawPowerStatus(int power_status)
{
  lcd.clear();
  lcd.setCursor(0,1);
  if(power_status == POWER_OK)
    lcd.print("POWER = OK");
  else
    lcd.print("POWER = FAIL");
}
