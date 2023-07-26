#include <SPI.h>
#include "Adafruit_GFX.h"
#include <MCUFRIEND_kbv.h>
#include <Adafruit_TFTLCD.h>
#include<Fonts/FreeSerifBold9pt7b.h>
#include "TouchScreen.h"

MCUFRIEND_kbv tft;

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 140
#define TS_MINY 120
#define TS_MAXX 900
#define TS_MAXY 940

#define MINPRESSURE 100
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

TSPoint p;

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ROZ     0xFBE0
#define LIGHT_GRAY 0xBDF7
#define DARK_GRAY 0x7BEF

#define X_OFFSET 10
#define HEIGHT tft.height()
#define WIDTH tft.width()
#define HALF_HEIGHT tft.height()/2
#define HALF_WIDTH tft.width()/2
#define WALL_THICKNESS 10
#define TANK_HEIGHT 120
#define TANK_WIDTH 100
#define UPPER_TANK_Y 20
#define LOWER_TANK_Y 180
#define TANK_COLOR BLACK
#define UPPER_TANK 1
#define LOWER_TANK 2

#define MOTOR_ON 1
#define MOTOR_OFF 0

#define POWER_OK 1
#define POWER_FAIL 0

#define COMM_OK 1
#define COMM_FAIL 0

char currentNumber = '0';

int waitCount = 0;
int powerStatus = 0;
int motorStatus = 0;

void setup(void) 
{
  pinMode(13,OUTPUT);
  Serial.begin(1200);
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(WHITE);
  tft.fillRect(0,0 , HALF_WIDTH+1, HEIGHT, BLACK);
  tft.fillRect(0,0 , HALF_WIDTH, HEIGHT, DARK_GRAY);
  
  tft.setFont(&FreeSerifBold9pt7b);
  
  delay(2000);
    
  //Send query after every 5 seconds until current status is not received from at motor end
  drawCommStatus(COMM_FAIL);
  Serial.print('#');
  while(Serial.available() <= 0)
    {
      delay(1);
      waitCount++;
      if(waitCount > 5000)
        {
          waitCount = 0;
          Serial.print('#');
        }
    }
  drawCommStatus(COMM_OK);
  
  while(Serial.available()) 
    {
      // get the new byte:
      byte inChar = Serial.read();
      parseData(inChar);
    }
}

void loop() 
{
  delay(10);
  
  digitalWrite(13, HIGH);
  p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
    {
      tft.fillRect(0,0 , HALF_WIDTH+1, HEIGHT, BLACK);
      tft.fillRect(0,0 , HALF_WIDTH, HEIGHT, RED);
      waitCount = 0;
      drawCommStatus(COMM_FAIL);
      Serial.print('@');
      while(Serial.available() <= 0)
        {
          delay(1);
          waitCount++;
          if(waitCount > 10000)
            {
              waitCount = 0;
              Serial.print('@');
            }
        }
      drawCommStatus(COMM_OK);
      while(Serial.available()) 
        {
          // get the new byte:
          byte inChar = Serial.read();
          parseData(inChar);
        }
      while(p.z > MINPRESSURE && p.z < MAXPRESSURE)
        {
          digitalWrite(13, HIGH);
          p = ts.getPoint();
          digitalWrite(13, LOW);
        
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          delay(10);
        }
      tft.fillRect(0,0 , HALF_WIDTH+1, HEIGHT, BLACK);
      tft.fillRect(0,0 , HALF_WIDTH, HEIGHT, DARK_GRAY);
    }
}

void parseData(byte motorData)
  {
     powerStatus = motorData & 0x01;
     motorStatus = motorData & 0x02;

     if(powerStatus == 1)
      drawPowerStatus(POWER_OK);
     else
      drawPowerStatus(POWER_FAIL);

     if(motorStatus == 2)
      drawMotorStatus(MOTOR_ON);
     else
      drawMotorStatus(MOTOR_OFF);
  }

void drawCommStatus(int comm_status)
{
  tft.setCursor(145, 55-30+10);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("COMM.");
  tft.fillRect(HALF_WIDTH + 10, 75-30, 100, 50, BLACK);
  if(comm_status == COMM_OK)
    {
      tft.fillRect(HALF_WIDTH + 15, 80-30, 100, 50, BLUE);
      tft.setCursor(157, 115-30);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("OK");
      tft.setCursor(157+2, 115+2-30);
      tft.setTextColor(YELLOW);
      tft.setTextSize(2);
      tft.print("OK");
    }
  else
    {
      tft.fillRect(HALF_WIDTH + 15, 80-30, 100, 50, YELLOW);
      tft.setCursor(145, 115-30);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("FAIL");
      tft.setCursor(145+2, 115+2-30);
      tft.setTextColor(BLUE);
      tft.setTextSize(2);
      tft.print("FAIL");
    }
}

void drawMotorStatus(int motor_status)
{
  tft.setCursor(145, 55+70+10);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("MOTOR");
  tft.fillRect(HALF_WIDTH + 10, 75+70, 100, 50, BLACK);
  if(motor_status == MOTOR_ON)
    {
      tft.fillRect(HALF_WIDTH + 15, 80+70, 100, 50, RED);
      tft.setCursor(157, 115+70);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("ON");
      tft.setCursor(157+2, 115+2+70);
      tft.setTextColor(GREEN);
      tft.setTextSize(2);
      tft.print("ON");
    }
  else
    {
      tft.fillRect(HALF_WIDTH + 15, 80+70, 100, 50, GREEN);
      tft.setCursor(145, 115+70);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("OFF");
      tft.setCursor(145+2, 115+2+70);
      tft.setTextColor(RED);
      tft.setTextSize(2);
      tft.print("OFF");
    }
}

void drawPowerStatus(int power_status)
{
  tft.setCursor(145, 155+70+10);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("POWER");
  tft.fillRect(HALF_WIDTH + 10, 175+70, 100, 50, BLACK);
  if(power_status == POWER_OK)
    {
      tft.fillRect(HALF_WIDTH + 15, 180+70, 100, 50, CYAN);
      tft.setCursor(157, 215+70);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("OK");
      tft.setCursor(157+2, 215+2+70);
      tft.setTextColor(MAGENTA);
      tft.setTextSize(2);
      tft.print("OK");
    }
  else
    {
      tft.fillRect(HALF_WIDTH + 15, 180+70, 100, 50, MAGENTA);
      tft.setCursor(140, 215+70);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("FAIL");
      tft.setCursor(140+2, 215+2+70);
      tft.setTextColor(CYAN);
      tft.setTextSize(2);
      tft.print("FAIL");
    }
}
