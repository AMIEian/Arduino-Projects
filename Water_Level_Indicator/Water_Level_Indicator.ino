//FOR 2.8" TFT
#include <Adafruit_GFX.h>
//#include <Adafruit_TFTLCD.h>
#include<Fonts/FreeSerifBold9pt7b.h>
#include "TouchScreen.h"
#include <EEPROM.h>
#include <MCUFRIEND_kbv.h>
//MCUFRIEND_kbv tft;

/*
//FOR 2.4" TFT
#include <SPI.h>          // f.k. for Arduino-1.5.2
#include "Adafruit_GFX.h"// Hardwares-specific library
#include <MCUFRIEND_kbv.h>
#include <Adafruit_TFTLCD.h>
#include<Fonts/FreeSerifBold9pt7b.h>
#include "TouchScreen.h"
//MCUFRIEND_kbv tft;
*/

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // LCD Reset goes to Analog 4

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 140
#define TS_MINY 120
#define TS_MAXX 900
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7

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

#define MINPRESSURE 100
#define MAXPRESSURE 1000

//FOR 2.8" TFT
 MCUFRIEND_kbv tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TSPoint p;

//PIN Alias
const int analogPin = A5;   //The pin that the potentiometer is attached to
const int VCC = 10;   //Series of diode
const int Buzzer = 12;  //Buzzer

unsigned int sensorReading = 0;
unsigned int accurate_sensorReading = 0;
int sensor_high = 950;
int sensor_low = 40;
boolean buzzer_mute = false;
char halfSec = 0;
int beepTime;
int ledLevel = 10;
int currentLevel = 0;
int previousLevel = 0;
int x = 0;
boolean screenTouched = false;

int currentmillis = 0;
int halfSecMillis = 0;

String symbol[4][4] = {
  { "7", "8", "9", "/" },
  { "4", "5", "6", "*" },
  { "1", "2", "3", "-" },
  { "C", "0", "=", "+" }
};

void setup()
{
  Serial.begin(9600);
  
  tft.reset();

  uint16_t identifier = tft.readID();
  
  tft.begin(identifier);

  draw_BoxNButtons();
  /*
  tft.setFont(&FreeSerifBold9pt7b);
  
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  
 
  pinMode(Buzzer,OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(Buzzer,HIGH);
  delay(1000);
  digitalWrite(Buzzer,LOW);
 bootScreen();
  //Reading Sensor High from EEPROM
  if(EEPROM.read(0) == 'H')
    {
      EEPROM.get(1,sensor_high);
    }
  //Reading Sensor Low from EEPROM
  if(EEPROM.read(10) == 'L')
    {
      EEPROM.get(11,sensor_low);
    }  
    
  updateDisplay(currentLevel);
  */
}

void loop()
{
  /*
  currentmillis = millis();   //Get current millis
  if(currentmillis - halfSecMillis >= 500)
        {
            halfSecMillis = currentmillis;
            halfSec++;
        }
 
  digitalWrite(13, HIGH);
  p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
    {
     // p.x = tft.width()-(map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
     // p.y = tft.height()-(map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
     // if(p.x > 120 && p.x < 220)
       // {
         // if(p.y > 280 && p.y < 310)
          //  {
              if(buzzer_mute)
                {
                  buzzer_mute = false;
                  updateDisplay(currentLevel);
                }
              else
                {
                  buzzer_mute = true;
                  updateDisplay(currentLevel);
                }
            
         // else if(p.y > 110 && p.y < 210)
          //  {
              //calibrate();
            //}
       // }
    }
  
  if(halfSec % 10 == 0)
  {
    //10 sec completed. Check level.
    pinMode(VCC,OUTPUT);
    pinMode(analogPin,INPUT);
    //Turn on 5 volt
    digitalWrite(VCC,HIGH);
    delay(1000);
    //Read the potentiometer and get average
    accurate_sensorReading = 0;
    for(int t = 0; t < 5; t++)
      {
         sensorReading = 0;
         for(int readings = 0; readings < 5; readings++)
          {
            sensorReading = sensorReading + analogRead(analogPin);
            delay(50);
          }
         //Find Average to get final result
         sensorReading = (int)sensorReading/5;
         accurate_sensorReading = accurate_sensorReading + sensorReading;
         delay(100);
      }
   
    accurate_sensorReading = (int)accurate_sensorReading/5;

    Serial.println(accurate_sensorReading);

    //TABLE
    if(accurate_sensorReading < 160)
      ledLevel = 0;
    else if(accurate_sensorReading >= 160 && accurate_sensorReading < 217) 
      ledLevel = 1;
    else if(accurate_sensorReading >= 217 && accurate_sensorReading < 287) 
      ledLevel = 2;
    else if(accurate_sensorReading >= 287 && accurate_sensorReading < 357) 
      ledLevel = 3;
    else if(accurate_sensorReading >= 357 && accurate_sensorReading < 427) 
      ledLevel = 4;
    else if(accurate_sensorReading >= 427&& accurate_sensorReading < 497) 
      ledLevel = 5;
    else if(accurate_sensorReading >= 497 && accurate_sensorReading <567)  //638) 
      ledLevel = 6; 
    else if(accurate_sensorReading >= 567 && accurate_sensorReading <637) //716
      ledLevel = 7;
    else if(accurate_sensorReading >= 637 && accurate_sensorReading < 707) //756
      ledLevel = 8;
    else if(accurate_sensorReading >= 707 && accurate_sensorReading < 777) 
      ledLevel = 9;
    else 
      ledLevel = 10;
      
    currentLevel = ledLevel * 10;
    
    if(currentLevel != previousLevel)
      {
        updateDisplay(currentLevel);
        previousLevel = currentLevel;
      }
    //Turn off and reverse voltage for depolarization
    digitalWrite(VCC,LOW);
    pinMode(VCC,INPUT);
    pinMode(analogPin,OUTPUT);
    digitalWrite(analogPin,HIGH);
    delay(1750);
    digitalWrite(analogPin,LOW);
  }
  if(halfSec == 30)
  {
    if(ledLevel > 8)
    {
      if(!buzzer_mute)
      {
        digitalWrite(Buzzer,HIGH);
        delay(3000);
        digitalWrite(Buzzer,LOW);
        delay(1000);
      }
    }
  }
  if(halfSec == 60)
  {
    if(ledLevel > 8 || ledLevel < 2)
    {
      if(!buzzer_mute)
      {
        digitalWrite(Buzzer,HIGH);
        delay(3000);
        digitalWrite(Buzzer,LOW);
        delay(1000);
      }
    }
  }
  if(halfSec == 90)
  {
    if(ledLevel > 8)
    {
      if(buzzer_mute)
      {
        digitalWrite(Buzzer,HIGH);
        delay(3000);
        digitalWrite(Buzzer,LOW);
        delay(1000);
      }
    }
  }
  if(halfSec == 120)
  {
    //Play animation
    int color;
    switch(random(3))
      {
        case 0:
          color = RED;
          break;
        case 1:
          color = BLUE;
          break;
        case 2:
          color = YELLOW;
          break;
        case 3:
          color = GREEN;
          break;
        default:
          color = CYAN;
      }
    tft.fillScreen(BLACK);
    
    testFilledCircles(10, color);
    delay(1000);
    testCircles(10, WHITE);
    delay(1000);
    
    updateDisplay(currentLevel);
    
    //Reset half sec counter
    halfSec = 0;
    if(ledLevel >= 8 || ledLevel <= 2)
    {
      if(buzzer_mute)
      {
        digitalWrite(Buzzer,HIGH);
        delay(3000);
        digitalWrite(Buzzer,LOW);
        delay(1000);
      }
    }
  }
*/
}

void updateDisplay(int level)
{
  tft.fillScreen(WHITE);
  tft.setTextColor(BLACK);
  tft.fillRect(0, 0, 101, tft.height(), BLACK);
  int ledHeight = tft.height()/10;
  int offset = 3;
  int count = level/10;
  
  for(int i = 1; i <= 10; i++)
  {
    if(i<=count)
    {
      if(i <= 2)
      {
        tft.fillRect(0+offset, tft.height() - (i*ledHeight) + offset, 100 - (2*offset), ledHeight - (2*offset), RED);
        tft.fillRect(101, tft.height() - (i*ledHeight), tft.width() - 102, ledHeight, CYAN);
      }
      else if(i > 2 && i <= 8)
      {
        tft.fillRect(0+offset, tft.height() - (i*ledHeight) + offset, 100 - (2*offset), ledHeight - (2*offset), YELLOW);
        tft.fillRect(101, tft.height() - (i*ledHeight), tft.width() - 102, ledHeight, CYAN);
      }
      else
      {
        tft.fillRect(0+offset, tft.height() - (i*ledHeight) + offset, 100 - (2*offset), ledHeight - (2*offset), GREEN);
        tft.fillRect(101, tft.height() - (i*ledHeight), tft.width() - 102, ledHeight, CYAN);
      }
    }
    else
    {
      tft.fillRect(0+offset, tft.height() - (i*ledHeight) + offset, 100 - (2*offset), ledHeight - (2*offset), LIGHT_GRAY);
      tft.fillRect(101, tft.height() - (i*ledHeight), tft.width() - 102, ledHeight, WHITE);
    }
  }
  
  tft.setCursor(111, 90);
  tft.setTextSize(2);
  tft.print("LEVEL");
  
  tft.setCursor(123, 235);
  tft.setTextSize(1);
  tft.print("(PERCENT)");
  
  tft.fillRect(118, 278, 104, 34, BLACK);
  tft.fillRect(120, 280, 100, 30, ROZ);
  
  if(buzzer_mute == false)
    {
      tft.setCursor(143, 300);
      tft.setTextSize(1);
      tft.print("MUTE");
    }
  else
    {
      tft.setCursor(127, 300);
      tft.setTextSize(1);
      tft.print("UN-MUTE");
    }
  
  if(level < 100)
  {
    tft.drawSevenSegNumber(level,2,110,110,100,10,BLACK);
  }
  else
  {
    tft.drawSevenSegNumber(100,3,82,120,80,7,BLACK);
  }
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(BLACK);
  start = micros();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                w = tft.width()  + radius,
                h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
  return micros() - start;
}

void bootScreen()
{
  //testLines(CYAN);
  tft.setTextSize(2);
  tft.setCursor(52+3, 90-50);
  tft.print("EAGLE");
  tft.setCursor(25+3, 120-50);
  tft.print("CONTROL");
  tft.setCursor(32+3, 150-50);
  tft.print("SYSTEMS");
  tft.setTextSize(1);
  tft.setCursor(33, 150+120+5);
  tft.print("Helpline : 9819895913");
  tft.setCursor(7, 150+20+123+5);
  tft.print("www. eaglecontrolsystems. in");
  for(int i = 0; i<=4; i++)
    {
      tft.fillCircle(tft.width()/2, 150+32, 70, RED);
      delay(1000);
      tft.fillCircle(tft.width()/2, 150+32, 60, GREEN);
      delay(1000);
      tft.fillCircle(tft.width()/2, 150+32, 50, BLUE);
      delay(1000);
      tft.fillCircle(tft.width()/2, 150+32, 40, YELLOW);
      delay(1000);
      tft.fillCircle(tft.width()/2, 150+32, 30, MAGENTA);
      delay(1000);
      tft.fillCircle(tft.width()/2, 150+32, 20, WHITE);
      delay(1000);
      tft.fillCircle(tft.width()/2, 150+32, 10, CYAN);
      delay(1000);
    }
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setCursor(22, 150+15);
  tft.print("WELCOME");
  delay(3000);
}

void calibrate()
{
  int currentTime, entryTime;
  tft.fillScreen(WHITE);
  tft.setTextColor(BLACK);
  tft.fillRect(tft.width()/2-52, 278, 104, 34, BLACK);
  tft.fillRect(tft.width()/2-50, 280, 100, 30, ROZ);
  tft.setCursor(tft.width()/2-20, 300);
  tft.setTextSize(1);
  tft.print("SUB");

  tft.fillRect(tft.width()/2-52, 15, 104, 34, BLACK);
  tft.fillRect(tft.width()/2-50, 17, 100, 30, ROZ);
  tft.setCursor(tft.width()/2-20, 37);
  tft.setTextSize(1);
  tft.print("ADD");
  
  if(currentLevel < 100)
    tft.drawSevenSegNumber(currentLevel,2,70,100,80,8,BLUE);
  else
    tft.drawSevenSegNumber(currentLevel,3,70,100,80,8,BLUE);
  
  tft.setTextColor(RED);
  tft.setCursor(0, 220);
  tft.setTextSize(1);
  tft.print("   Calibration can only be done at 100% of water level!");
  
  currentTime = millis();
  entryTime = millis();

  while(currentTime - entryTime < 10000)
    {
      //delay(500);
      currentTime = millis();
      
      digitalWrite(13, HIGH);
      p = ts.getPoint();
      digitalWrite(13, LOW);

      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
  
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
        {
          p.x = tft.width()-(map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
          p.y = tft.height()-(map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
          if(p.x > (tft.width()/2-52) && p.x < (tft.width()/2+52))
            {
              if(p.y > 280 && p.y < 310)
                {
                  entryTime = millis();
                  if(currentLevel > 10)
                    currentLevel = currentLevel - 10;
                  if(currentLevel == 10)
                    {
                      pinMode(VCC,OUTPUT);
                      pinMode(analogPin,INPUT);
                      //Turn on 5 volt
                      digitalWrite(VCC,HIGH);
                      delay(1000);
                      //Read the potentiometer and get average
                      sensorReading = 0;
                      for(int readings = 0; readings < 5; readings++)
                        {
                          sensorReading = sensorReading + analogRead(analogPin);
                          delay(50);
                        }
                      //Find Average to get final result
                      sensorReading = (int)sensorReading/5;
                      sensor_low = sensorReading;
                      EEPROM.write(10,'L');
                      EEPROM.put(11,sensor_low);
                      tft.fillRect(0,200,tft.width(),50,WHITE);
                      tft.setCursor(0, 220);
                      tft.setTextSize(1);
                      tft.print("LOWER LEVEL CALIBRATED ...!");
                    }
                  tft.fillRect(0,100, tft.width(), 100, WHITE);
                  tft.drawSevenSegNumber(currentLevel,2,70,100,80,8,BLUE);
                }
              else if(p.y > 17 && p.y < 47)
                {
                  entryTime = millis();
                  if(currentLevel < 100)
                    currentLevel = currentLevel + 10;
                  if(currentLevel == 100)
                    {
                      pinMode(VCC,OUTPUT);
                      pinMode(analogPin,INPUT);
                      //Turn on 5 volt
                      digitalWrite(VCC,HIGH);
                      delay(1000);
                      //Read the potentiometer and get average
                      sensorReading = 0;
                      for(int readings = 0; readings < 5; readings++)
                        {
                          sensorReading = sensorReading + analogRead(analogPin);
                          delay(50);
                        }
                      //Find Average to get final result
                      sensorReading = (int)sensorReading/5;
                      sensor_high = sensorReading;
                      EEPROM.write(0,'H');
                      EEPROM.put(1,sensor_high);
                      tft.fillRect(0,200,tft.width(),50,WHITE);
                      tft.setCursor(0, 220);
                      tft.setTextSize(1);
                      tft.print("HIGHER LEVEL CALIBRATED ...!");
                      tft.fillRect(0,100, tft.width(), 100, WHITE);
                      tft.drawSevenSegNumber(currentLevel,3,70,100,80,8,BLUE);
                    }
                  else
                    {
                      tft.fillRect(0,100, tft.width(), 100, WHITE);
                      tft.drawSevenSegNumber(currentLevel,2,70,100,80,8,BLUE);  
                    }
                }
            }
        }
    }
  updateDisplay(currentLevel);
}

void draw_BoxNButtons()
{
  //Draw the Result Box
  tft.fillRect(0, 0, 240, 80, CYAN);

 //Draw First Column
  tft.fillRect  (0,260,60,60,RED);
  tft.fillRect  (0,200,60,60,BLACK);
  tft.fillRect  (0,140,60,60,BLACK);
  tft.fillRect  (0,80,60,60,BLACK);

 //Draw Third Column  
  tft.fillRect  (120,260,60,60,GREEN);
  tft.fillRect  (120,200,60,60,BLACK);
  tft.fillRect  (120,140,60,60,BLACK);
  tft.fillRect  (120,80,60,60,BLACK);

  //Draw Secound & Fourth Column  
  for (int b=260; b>=80; b-=60)
 { tft.fillRect  (180,b,60,60,BLUE); 
   tft.fillRect  (60,b,60,60,BLACK);}

  //Draw Horizontal Lines
  for (int h=80; h<=320; h+=60)
  tft.drawFastHLine(0, h, 240, WHITE);

  //Draw Vertical Lines
  for (int v=0; v<=240; v+=60)
  tft.drawFastVLine(v, 80, 240, WHITE);

  //Display keypad lables 
  for (int j=0;j<4;j++) {
    for (int i=0;i<4;i++) {
      tft.setCursor(22 + (60*i), 100 + (60*j));
      tft.setTextSize(3);
      tft.setTextColor(WHITE);
      tft.println(symbol[j][i]);
    }
  }
}
