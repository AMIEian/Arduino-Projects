#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
//#include <MCUFRIEND_kbv.h>
#include <Adafruit_TFTLCD.h>
#include "TouchScreen.h"
#include <avr/pgmspace.h>

// This is calibration data for the raw touch data to the screen coordinates
#define IsWithin(x, a, b) ((x>=a)&&(x<=b))

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // LCD Reset goes to Analog 4

#define YP A3
#define XM A2
#define YM 9
#define XP 8

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
#define MINPRESSURE 100
#define MAXPRESSURE 1000

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

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint p;
int touch_X, touch_Y;
//MCUFRIEND_kbv tft;
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

const char Mobile_KB[3][13] PROGMEM = {
  {0, 13, 10, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
  {1, 12, 9, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'},
  {3, 10, 7, 'Z', 'X', 'C', 'V', 'B', 'N', 'M'},
};

const char Mobile_NumKeys[3][13] PROGMEM = {
  {0, 13, 10, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
  {0, 13, 10, '-', '/', ':', ';', '(', ')', '$', '&', '@', '"'},
  {5, 8, 5, '.', '\,', '?', '!', '\''}
};

const char Mobile_SymKeys[3][13] PROGMEM = {
  {0, 13, 10, '[', ']', '{', '}', '#', '%', '^', '*', '+', '='},
  {4, 9, 6, '_', '\\', '|', '~', '<', '>'}, //4
  {5, 8, 5, '.', '\,', '?', '!', '\''}
};

const char textLimit = 25;
char MyBuffer[textLimit];

void MakeKB_Button(const char type[][13])
{
  tft.setTextSize(2);
  tft.setTextColor(0xffff, 0xf000);
  for (int y = 0; y < 3; y++)
  {
    int ShiftRight = 15 * pgm_read_byte(&(type[y][0]));
    for (int x = 3; x < 13; x++)
    {
      if (x >= pgm_read_byte(&(type[y][1]))) break;

      drawButton(15 + (30 * (x - 3)) + ShiftRight, 100 + (30 * y), 20, 25); // this will draw the button on the screen by so many pixels
      tft.setCursor(20 + (30 * (x - 3)) + ShiftRight, 105 + (30 * y));
      tft.print(char(pgm_read_byte(&(type[y][x]))));
    }
  }
  //ShiftKey
  drawButton(15, 160, 35, 25);
  tft.setCursor(27, 168);
  tft.print('^');

  //Special Characters
  drawButton(15, 190, 35, 25);
  tft.setCursor(21, 195);
  tft.print(F("SP"));

  //BackSpace
  drawButton(270, 160, 35, 25);
  tft.setCursor(276, 165);
  tft.print(F("BS"));

  //Return
  drawButton(270, 190, 35, 25);
  tft.setCursor(276, 195);
  tft.print(F("RT"));

  //Spacebar
  drawButton(60, 190, 200, 25);
  tft.setCursor(105, 195);
  tft.print(F("SPACE BAR"));
}

void drawButton(int x, int y, int w, int h)
{
  // grey
  tft.fillRoundRect(x - 3, y + 3, w, h, 3, 0x8888); //Button Shading

  // white
  tft.fillRoundRect(x, y, w, h, 3, 0xffff);// outter button color

  //red
  tft.fillRoundRect(x + 1, y + 1, w - 1 * 2, h - 1 * 2, 3, 0xf800); //inner button color
}

void GetKeyPress(char * textBuffer)
{
  char key = 0;
  static bool shift = false, special = false, back = false, lastSp = false, lastSh = false;
  static char bufIndex = 0;
  touch_X = 0; 
  touch_Y = 0;

  digitalWrite(13, HIGH);
  p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
    Serial.print("PX = ");
    Serial.println(p.x);
    Serial.print("PY = ");
    Serial.println(p.y);
    
    touch_Y = tft.height()-(map(p.x, TS_MINX, TS_MAXX, tft.height(), 0));
    touch_X = tft.width() - (tft.width()-(map(p.y, TS_MINY, TS_MAXY, 0, tft.width())));
    
    //touch_X = p.x;
    //touch_Y = p.y;
    
    Serial.print("X = ");
    Serial.println(touch_X);
    Serial.print("Y = ");
    Serial.println(touch_Y);

    Serial.print("TFT WIDTH = ");
    Serial.println(tft.width());
    Serial.print("TFT HEIGHT = ");
    Serial.println(tft.height());
    
    //ShiftKey
    if (TouchButton(15, 160, 35, 25))
    {
      shift = !shift;
      delay(200);
    }

    //Special Characters
    if (TouchButton(15, 190, 35, 25))
    {
      special = !special;
      delay(200);
    }

    if (special != lastSp || shift != lastSh)
    {
      if (special)
      {
        if (shift)
        {
          tft.fillScreen(BLUE);
          MakeKB_Button(Mobile_SymKeys);
        }
        else
        {
          tft.fillScreen(BLUE);
          MakeKB_Button(Mobile_NumKeys);
        }
      }
      else
      {
        tft.fillScreen(BLUE);
        MakeKB_Button(Mobile_KB);
        tft.setTextColor(0xffff, 0xf800);
      }

      if (special)
        tft.setTextColor(0x0FF0, 0xf800);
      else
        tft.setTextColor(0xFFFF, 0xf800);

      tft.setCursor(21, 195);
      tft.print(F("SP"));

      if (shift)
        tft.setTextColor(0x0FF0, 0xf800);
      else
        tft.setTextColor(0xffff, 0xf800);

      tft.setCursor(27, 168);
      tft.print('^');

      lastSh = shift;

      lastSp = special;
      lastSh = shift;
    }

    for (int y = 0; y < 3; y++)
    {
      int ShiftRight;
      if (special)
      {
        if (shift)
          ShiftRight = 15 * pgm_read_byte(&(Mobile_SymKeys[y][0]));
        else
          ShiftRight = 15 * pgm_read_byte(&(Mobile_NumKeys[y][0]));
      }
      else
        ShiftRight = 15 * pgm_read_byte(&(Mobile_KB[y][0]));

      for (int x = 3; x < 13; x++)
      {
        if (x >=  (special ? (shift ? pgm_read_byte(&(Mobile_SymKeys[y][1])) : pgm_read_byte(&(Mobile_NumKeys[y][1]))) : pgm_read_byte(&(Mobile_KB[y][1])) )) break;

        if (TouchButton(15 + (30 * (x - 3)) + ShiftRight, 100 + (30 * y), 20, 25)) // this will draw the button on the screen by so many pixels
        {
          if (bufIndex < (textLimit - 1))
          {
            delay(200);

            if (special)
            {
              if (shift)
                textBuffer[bufIndex] = pgm_read_byte(&(Mobile_SymKeys[y][x]));
              else
                textBuffer[bufIndex] = pgm_read_byte(&(Mobile_NumKeys[y][x]));
            }
            else
              textBuffer[bufIndex] = (pgm_read_byte(&(Mobile_KB[y][x])) + (shift ? 0 : ('a' - 'A')));

            bufIndex++;
          }
          break;
        }
      }
    }

    //Spacebar
    if (TouchButton(60, 190, 200, 25))
    {
      textBuffer[bufIndex++] = ' ';
      delay(200);
    }

    //BackSpace
    if (TouchButton(270, 160, 35, 25))
    {
      if ((bufIndex) > 0)
        bufIndex--;
      textBuffer[bufIndex] = 0;
      tft.setTextColor(0, BLUE);
      tft.setCursor(15, 80);
      tft.print(F("                          "));
      delay(200);
    }

    //Return
    if (TouchButton(270, 190, 35, 25))
    {
      Serial.println(textBuffer);
      while (bufIndex > 0)
      {
        bufIndex--;
        textBuffer[bufIndex] = 0;
      }

      tft.setTextColor(0, BLUE);
      tft.setCursor(15, 80);
      tft.print(F("                         "));
    }
  }
  tft.setTextColor(0xffff, 0xf800);
  tft.setCursor(15, 80);
  tft.print(textBuffer);
}


void setup(void)
{
  Serial.begin(115200);

  tft.reset();

  uint16_t identifier = tft.readID();
  
  tft.begin(identifier);
  
  tft.fillScreen(BLUE);
  // origin = left,top landscape (USB left upper)
  tft.setRotation(1);
  MakeKB_Button(Mobile_KB);
}

void loop()
{
  // See if there's any  touch data for us
  GetKeyPress(MyBuffer);
}


byte TouchButton(int x, int y, int w, int h)
{
  return (IsWithin(touch_X, x, x + w) & IsWithin(touch_Y, y, y + h));
}
