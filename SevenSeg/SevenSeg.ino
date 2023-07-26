#include <SPI.h>          // f.k. for Arduino-1.5.2
#include "Adafruit_GFX.h"// Hardwares-specific library
#include <MCUFRIEND_kbv.h>
#include <Adafruit_TFTLCD.h>
MCUFRIEND_kbv tft;


// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

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
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

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

//Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
int x = 0;

void setup(void) {
  Serial.begin(9600);
 
  tft.reset();

  uint16_t identifier = tft.readID();

  tft.begin(identifier);
  
  tft.fillScreen(BLACK);

  tft.drawSevenSegNumber(8,1,40,8,310,30,RED);
}

void loop() {
  /*
  for(x = 100; x >= 0; x = x - 10)
    {
      updateDisplay(x);
      delay(10000);
    }
  for(x = 0; x <= 100; x = x + 10)
    {
      updateDisplay(x);
      delay(10000);
    }
  */
}
void serialEvent() {
    while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();
        if(inChar >= '0' && inChar <= '9')
          {
            tft.fillScreen(BLACK);
            tft.drawSevenSegNumber(inChar-48,1,40,8,310,30,RED);
          }
    }
}
/*
void updateDisplay(int level)
  {
    //tft.fillRect(105, 108, tft.width() - 105, 103, WHITE);
    tft.fillScreen(WHITE);
    
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
    
    if(level < 100)
      {
        tft.drawSevenSegNumber(level,2,110,110,100,10,BLACK);
      }
    else
      {
        tft.drawSevenSegNumber(100,3,82,120,80,7,BLACK);
      }
  }
*/
