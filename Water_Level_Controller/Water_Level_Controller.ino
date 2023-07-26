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

//Drawing related variables
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

#define TANK_ID 0x00
#define ID_MASK 0xF8
#define TANK_DATA_MASK 0x07

#define SINGLE_PHASE 1
#define THREE_PHASE 2

#define FULL true
#define EMPTY false

#define CHECKING_INTERVAL 15000

//Unit type
const int unitType = SINGLE_PHASE;

//PIN Assignment
const int probeLower = 10;  //Black wire of probe
const int probeUpper = 11;  //Red wire of probe with diode
const int startRelay = 12;
const int stopRelay = 13;
const int powerPin = A5;

//Level Variables
boolean upperTankUpperLevel = false;
boolean upperTankLowerLevel = false;
boolean lowerTankUpperLevel = true;
boolean lowerTankLowerLevel = true;

//Motor Controlling Variables
boolean motorIsOn = false;
boolean levelAndPowerOk = false;
boolean levelCheck = false;
boolean powerCheck = false;
int lastDataReceptionTime = 26;

int currentmillis = 0;
int last_check_time = 0;

void setup(void) 
{
  pinMode(probeLower, OUTPUT);
  pinMode(probeUpper, OUTPUT);
  pinMode(startRelay, OUTPUT);
  pinMode(stopRelay, OUTPUT);
  pinMode(powerPin, INPUT);

  digitalWrite(probeUpper, LOW);
  digitalWrite(probeLower, LOW);
  
  digitalWrite(startRelay, LOW);
  digitalWrite(stopRelay, LOW);    
  
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
  drawTankEmpty(UPPER_TANK);
  drawTankEmpty(LOWER_TANK);
  drawMotorStatus(MOTOR_OFF);
  drawPowerStatus(POWER_FAIL);

  drawNetwork();
}

void loop() 
{  
  currentmillis = millis();   //Get current millis
  if(currentmillis - last_check_time >= CHECKING_INTERVAL)
    {
      last_check_time = currentmillis;
      tankLevelCheck();
      powerSupplyCheck();
    }
  //-----------------START - STOP MOTOR ACCORDING TO LEVEL AND POWER CHECK-----------------------//
  if( levelCheck && powerCheck && lastDataReceptionTime <= 25 )
    {
      //Check level & phase flag if it is 0 then motor was off due to Power faillure or level failure Start it again
      if( levelAndPowerOk == false )
        {
          motorStart();
          levelAndPowerOk = true;
          motorIsOn = true;
          drawMotorStatus(MOTOR_ON);
        }
    }
  //If level or Phase is not ok then stop the motor & set it's flag to 0
  else
    {
      //Level or Power is fail check that the motor was running or not if it was running stop it now
      if( levelAndPowerOk == true )
        {
          motorStop();
          levelAndPowerOk = false;
          motorIsOn = false;
          drawMotorStatus(MOTOR_OFF);
        }
    }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    Serial.println(inChar);
    inChar = inChar - 48;
    char id = inChar & ID_MASK;
    Serial.print(id);
    if(id == TANK_ID)
      {
        char data = inChar & TANK_DATA_MASK;
        Serial.print(data);
        if(data == 0x00)
          {
            //Upper tank empty
            upperTankUpperLevel = EMPTY;
            upperTankLowerLevel = EMPTY;
            drawTankEmpty(UPPER_TANK);
          }
        else if(data == 0x01)
          {
            //Upper tank half empty
            upperTankUpperLevel = EMPTY;
            upperTankLowerLevel = FULL;
            drawTankHalfFull(UPPER_TANK);
          }
        else if(data == 0x02)
          {
            //Upper tank half empty
            upperTankUpperLevel = FULL;
            upperTankLowerLevel = FULL;
            drawTankFull(UPPER_TANK);
          }
        lastDataReceptionTime = 0;
        drawNetwork();
      }
  }
}

void tankLevelCheck()
{
  //Check Upper Level of Lower Tank
  pinMode(probeUpper, INPUT);
  pinMode(probeLower, OUTPUT);
  
  digitalWrite(probeLower, HIGH);
  delay(1000);
  if(digitalRead(probeUpper) == HIGH)
    {
      //Tank is full. ie. both levels full
      lowerTankUpperLevel = FULL;
      lowerTankLowerLevel = FULL;
      //Depolarization
      pinMode(probeUpper, OUTPUT);
      pinMode(probeLower, INPUT);
      digitalWrite(probeUpper, HIGH);
      delay(1000);

      pinMode(probeLower, OUTPUT);
      digitalWrite(probeUpper, LOW);
      digitalWrite(probeLower, LOW);
      drawTankFull(LOWER_TANK);
    }
  else
    {
      //Check Lower Level of Tank
      digitalWrite(probeLower, LOW);
      pinMode(probeUpper, OUTPUT);
      pinMode(probeLower, INPUT);
      digitalWrite(probeUpper, HIGH);
      delay(1000);
      if(digitalRead(probeLower) == HIGH)
        {
          //Tank is half empty
          lowerTankUpperLevel = EMPTY;
          lowerTankLowerLevel = FULL;
          drawTankHalfFull(LOWER_TANK);
        }
      else
        {
          //Tank is empty
          lowerTankUpperLevel = EMPTY;
          lowerTankLowerLevel = EMPTY;
          drawTankEmpty(LOWER_TANK);
        }
      pinMode(probeLower, OUTPUT);
      digitalWrite(probeUpper, LOW);
      digitalWrite(probeLower, LOW);
    }

  lastDataReceptionTime ++;
  if(lastDataReceptionTime > 25)
    lastDataReceptionTime = 26;
      
  drawNetwork();
  
  //If motor is running
  if(motorIsOn)
  {
    if( upperTankUpperLevel == EMPTY && lowerTankLowerLevel == FULL )
      levelCheck = true;
    else
      levelCheck = false;
  }
  //If motor is stoped
  else
  {
    if( upperTankLowerLevel == EMPTY && lowerTankUpperLevel == FULL )
      levelCheck = true;
    else
      levelCheck = false;
  }
}

void powerSupplyCheck()
{
  //-------------------POWER CHECK-----------------------//
  if(unitType == SINGLE_PHASE)
    {
      powerCheck = true;
      drawPowerStatus(POWER_OK);
    }
  else
    {
      if(analogRead(powerPin) > 750)
        {
          powerCheck = true;
          drawPowerStatus(POWER_OK);
        }
      else
        {
          powerCheck = false;
          drawPowerStatus(POWER_FAIL);
        }
    }
}

void motorStart()
{
  if(unitType == SINGLE_PHASE)
    {
      digitalWrite(startRelay, HIGH);
      delay(1000);
    }
  else
    {
      digitalWrite(startRelay, HIGH);
      delay(10000);
      digitalWrite(startRelay, LOW);
      delay(1000);
    }
}

void motorStop()
{
  if(unitType == SINGLE_PHASE)
    {
      digitalWrite(startRelay, LOW);
      delay(1000);
    }
  else
    {
      digitalWrite(stopRelay, HIGH);
      delay(10000);
      digitalWrite(stopRelay, LOW);
      delay(1000);
    }
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
  tft.setCursor(145, 55+10);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("MOTOR");
  tft.fillRect(HALF_WIDTH + 10, 75+10, 100, 50, BLACK);
  if(motor_status == MOTOR_ON)
    {
      tft.fillRect(HALF_WIDTH + 15, 80+10, 100, 50, RED);
      tft.setCursor(157, 115+10);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("ON");
      tft.setCursor(157+2, 115+2+10);
      tft.setTextColor(GREEN);
      tft.setTextSize(2);
      tft.print("ON");
    }
  else
    {
      tft.fillRect(HALF_WIDTH + 15, 80+10, 100, 50, GREEN);
      tft.setCursor(145, 115+10);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("OFF");
      tft.setCursor(145+2, 115+2+10);
      tft.setTextColor(RED);
      tft.setTextSize(2);
      tft.print("OFF");
    }
}

void drawPowerStatus(int power_status)
{
  tft.setCursor(145, 155+50);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("POWER");
  tft.fillRect(HALF_WIDTH + 10, 175+50, 100, 50, BLACK);
  if(power_status == POWER_OK)
    {
      tft.fillRect(HALF_WIDTH + 15, 180+50, 100, 50, CYAN);
      tft.setCursor(157, 215+50);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("OK");
      tft.setCursor(157+2, 215+2+50);
      tft.setTextColor(MAGENTA);
      tft.setTextSize(2);
      tft.print("OK");
    }
  else
    {
      tft.fillRect(HALF_WIDTH + 15, 180+50, 100, 50, MAGENTA);
      tft.setCursor(140, 215+50);
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      tft.print("FAIL");
      tft.setCursor(140+2, 215+2+50);
      tft.setTextColor(CYAN);
      tft.setTextSize(2);
      tft.print("FAIL");
    }
}

void drawNetwork()
{
  int xStart = WIDTH - 13;
  int yStart = 23;
  int bars = 0;
  int barHeight = 4;
  //Erase previous signals
  tft.fillRect(WIDTH-50, 0, 50, 50, WHITE);
  
  tft.fillRect(WIDTH-11, 5, 10, 3, BLACK);
  tft.fillRect(WIDTH-7, 7, 3, 23, BLACK);
  
  if(lastDataReceptionTime <= 5)
    bars = 5;
  else if(lastDataReceptionTime > 5 && lastDataReceptionTime <= 10)
    bars = 4;
  else if(lastDataReceptionTime > 10 && lastDataReceptionTime <= 15)
    bars = 3;
  else if(lastDataReceptionTime > 15 && lastDataReceptionTime <= 20)
    bars = 2;
  else if(lastDataReceptionTime > 20 && lastDataReceptionTime <= 25)
    bars = 1;
  else if (lastDataReceptionTime > 25)
    {
      bars = 0;
      tft.drawLine(WIDTH-25, 15, WIDTH-12, 25, RED);
      tft.drawLine(WIDTH-26, 15, WIDTH-13, 25, RED);
      tft.drawLine(WIDTH-12, 15, WIDTH-25, 25, RED);
      tft.drawLine(WIDTH-13, 15, WIDTH-26, 25, RED);
    }

  while(bars > 0)
    {
      tft.fillRect(xStart, yStart, 3, barHeight, BLACK);
      xStart = xStart - 5;
      yStart = yStart - 4;
      barHeight = barHeight + 4;
      bars--;
    }
}

