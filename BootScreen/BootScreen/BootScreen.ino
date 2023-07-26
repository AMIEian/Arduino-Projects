//FOR 2.8" TFT
#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include<Fonts/FreeSerifBold9pt7b.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // LCD Reset goes to Analog 4

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

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup()
{
  Serial.begin(9600);
  
  tft.reset();

  uint16_t identifier = tft.readID();
  
  tft.begin(identifier);
  
  tft.setFont(&FreeSerifBold9pt7b);
  tft.setTextColor(BLACK);
  bootScreen();
}

void loop()
{
  
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(BLACK);

  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing

  tft.fillScreen(BLACK);

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(BLACK);

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(BLACK);

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

  return micros() - start;
}

void bootScreen()
{
  
  tft.fillScreen(WHITE);
  tft.setTextSize(2);
  /*
  tft.setCursor(52+3, 90-50+5);
  tft.print("EAGLE");
  tft.setCursor(25+3, 120-50+5);
  tft.print("CONTROL");
  */
  tft.setCursor(22, 150+15);
  tft.print("WELCOME");
  /*
  tft.setTextSize(1);
  tft.setCursor(33, 150+120);
  tft.print("Helpline : 9819895913");
  tft.setCursor(7, 150+20+123);
  tft.print("www. eaglecontrolsystems. in");
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
  //testLines(CYAN);
  */
}

