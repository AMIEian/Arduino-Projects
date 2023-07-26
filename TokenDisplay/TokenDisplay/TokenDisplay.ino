#include <SPI.h>
#include "Adafruit_GFX.h"
#include <MCUFRIEND_kbv.h>
#include <Adafruit_TFTLCD.h>
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

char currentNumber = '0';

void setup(void) {
  Serial.begin(9600);
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.drawSevenSegNumber(currentNumber-48,1,40,8,310,30,RED);
}

void loop() {
  
}
void serialEvent() {
    while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();
        if(inChar >= '0' && inChar <= '9')
          {
            Serial.print(currentNumber);
            currentNumber = inChar;
            tft.fillScreen(BLACK);
            tft.drawSevenSegNumber(currentNumber-48,1,40,8,310,30,RED);
          }
    }
}
