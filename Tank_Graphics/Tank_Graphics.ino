#include <SPI.h>
#include "Adafruit_GFX.h"
#include <MCUFRIEND_kbv.h>
#include <Adafruit_TFTLCD.h>
#include<Fonts/FreeSerifBold9pt7b.h>
MCUFRIEND_kbv tft;

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

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

char currentNumber = '0';

void setup(void) 
{
  Serial.begin(9600);
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(WHITE);
  tft.fillRect(0,0 , HALF_WIDTH+1, HEIGHT, BLACK);
  tft.fillRect(0,0 , HALF_WIDTH, HEIGHT, DARK_GRAY);
  
  tft.setFont(&FreeSerifBold9pt7b);
  
  //tft.drawLine(0, 160, 120, 160, BLACK);
  drawTank(UPPER_TANK);
  drawTank(LOWER_TANK);
  drawTankHalfFull(UPPER_TANK);
  drawTankFull(LOWER_TANK);
  drawMotorStatus(MOTOR_ON);
  drawPowerStatus(POWER_OK);
}

void loop() 
{
  
}

void drawTank(int tank)
{
  if(tank == UPPER_TANK)
    {
      //Draw upper tank
      tft.fillRect(X_OFFSET, UPPER_TANK_Y, WALL_THICKNESS, TANK_HEIGHT, TANK_COLOR);
      tft.fillRect(X_OFFSET, UPPER_TANK_Y + TANK_HEIGHT, TANK_WIDTH, WALL_THICKNESS, TANK_COLOR);
      tft.fillRect(TANK_WIDTH, UPPER_TANK_Y, WALL_THICKNESS, TANK_HEIGHT, TANK_COLOR);
      tft.fillRect(X_OFFSET - 5, UPPER_TANK_Y - 5, WALL_THICKNESS * 2, WALL_THICKNESS/2, TANK_COLOR);
      tft.fillRect(TANK_WIDTH - 5, UPPER_TANK_Y - 5, WALL_THICKNESS * 2, WALL_THICKNESS/2, TANK_COLOR);
    }
  else
    {
      //Draw lower tank
      tft.fillRect(X_OFFSET, LOWER_TANK_Y, WALL_THICKNESS, TANK_HEIGHT, TANK_COLOR);
      tft.fillRect(X_OFFSET, LOWER_TANK_Y + TANK_HEIGHT, TANK_WIDTH, WALL_THICKNESS, TANK_COLOR);
      tft.fillRect(TANK_WIDTH, LOWER_TANK_Y, WALL_THICKNESS, TANK_HEIGHT, TANK_COLOR);
      tft.fillRect(X_OFFSET - 5, LOWER_TANK_Y - 5, WALL_THICKNESS * 2, WALL_THICKNESS/2, TANK_COLOR);
      tft.fillRect(TANK_WIDTH - 5, LOWER_TANK_Y - 5, WALL_THICKNESS * 2, WALL_THICKNESS/2, TANK_COLOR);
    }
}

void drawTankFull(int tank)
{
  drawTankEmpty(tank);
  int water_start = 0;
  if(tank == UPPER_TANK)
    water_start = UPPER_TANK_Y + 10;
  else
    water_start = LOWER_TANK_Y + 10;
  //Draw water
  tft.fillRect(X_OFFSET + WALL_THICKNESS, water_start, TANK_WIDTH - WALL_THICKNESS - X_OFFSET, TANK_HEIGHT - 10, BLUE);
}

void drawTankEmpty(int tank)
{
  int water_start = 0;
  if(tank == UPPER_TANK)
    water_start = UPPER_TANK_Y  + 10;
  else
    water_start = LOWER_TANK_Y + 10;
  //Draw water
  tft.fillRect(X_OFFSET + WALL_THICKNESS, water_start, TANK_WIDTH - WALL_THICKNESS - X_OFFSET, TANK_HEIGHT - 10, DARK_GRAY);
}

void drawTankHalfFull(int tank)
{
  drawTankEmpty(tank);
  int water_start = 0;
  if(tank == UPPER_TANK)
    water_start = UPPER_TANK_Y + 10 + TANK_HEIGHT/2;
  else
    water_start = LOWER_TANK_Y + 10 + TANK_HEIGHT/2;
  //Draw water
  tft.fillRect(X_OFFSET + WALL_THICKNESS, water_start, TANK_WIDTH - WALL_THICKNESS - X_OFFSET, TANK_HEIGHT - 10 - TANK_HEIGHT/2, BLUE);
}

void drawMotorStatus(int motor_status)
{
  tft.setCursor(145, 55);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("MOTOR");
  tft.fillRect(HALF_WIDTH + 10, 75, 100, 50, BLACK);
  if(motor_status == MOTOR_ON)
    {
      tft.fillRect(HALF_WIDTH + 15, 80, 100, 50, RED);
      tft.setCursor(157, 115);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("ON");
      tft.setCursor(157+2, 115+2);
      tft.setTextColor(GREEN);
      tft.setTextSize(2);
      tft.print("ON");
    }
  else
    {
      tft.fillRect(HALF_WIDTH + 15, 80, 100, 50, GREEN);
      tft.setCursor(145, 115);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("OFF");
      tft.setCursor(145+2, 115+2);
      tft.setTextColor(RED);
      tft.setTextSize(2);
      tft.print("OFF");
    }
}

void drawPowerStatus(int power_status)
{
  tft.setCursor(145, 155+60);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("POWER");
  tft.fillRect(HALF_WIDTH + 10, 175+60, 100, 50, BLACK);
  if(power_status == POWER_OK)
    {
      tft.fillRect(HALF_WIDTH + 15, 180+60, 100, 50, CYAN);
      tft.setCursor(157, 215+60);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("OK");
      tft.setCursor(157+2, 215+2+60);
      tft.setTextColor(MAGENTA);
      tft.setTextSize(2);
      tft.print("OK");
    }
  else
    {
      tft.fillRect(HALF_WIDTH + 15, 180+60, 100, 50, MAGENTA);
      tft.setCursor(140, 215+60);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("FAIL");
      tft.setCursor(140+2, 215+2+60);
      tft.setTextColor(CYAN);
      tft.setTextSize(2);
      tft.print("FAIL");
    }
}
