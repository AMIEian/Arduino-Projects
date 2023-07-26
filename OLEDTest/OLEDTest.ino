//Analog - A0, A1
//KEYPAD - ESC 2, ENTR 3, DWN 4, UP 5
//RELAY 6

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBold18pt7b.h>  // Add a custom font
#include <Fonts/FreeMonoBold12pt7b.h>  // Add a custom font
#include <Fonts/FreeMonoBold9pt7b.h>  // Add a custom font

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_CENTER_X 64
#define SCREEN_CENTER_Y 32
#define SCREEN_START_X 0
#define SCREEN_START_Y 0
#define SCREEN_END_X 128
#define SCREEN_END_Y 64

#define RECTANGLE_WIDTH 64
#define RECTANGLE_HEIGHT 32

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int RTD_1 = A0;
int RTD_2 = A1;
int key1 = 2;
int key2 = 3;
int key3 = 4;
int key4 = 5;

int RTD_1_Value = 0;
int RTD_2_Value = 0;
int currentKey = 0;
int lastKey = 0;
unsigned long lastKeyTime = 0;
int keyDelay = 10000;

void setup()   
{                
  pinMode(key1,INPUT_PULLUP);
  pinMode(key2,INPUT_PULLUP);
  pinMode(key3,INPUT_PULLUP);
  pinMode(key4,INPUT_PULLUP);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.cp437(true);
  display.setFont(&FreeMonoBold18pt7b);  // Set a custom font
}


void loop() 
{
  /*
  for(float x = 0.0; x <= 9.9; x = x + 0.1)
  {
    //Serial.println(x);
    normalDisplay(x, x, x, x);
    delay(100);
  }
  */
  RTD_1_Value = analogRead(RTD_1);
  delay(50);
  RTD_2_Value = analogRead(RTD_2);
  delay(50);
  
  display.clearDisplay();
  String txt = "1-" + String(RTD_1_Value);
  myText(txt, 3, 30, 1, 1, false);
  txt = "2-" + String(RTD_2_Value);
  myText(txt, 3, 60, 1, 1, false);
  display.display();
  delay(500);
  
  int key = getKeypad();
  if(key != 0)
    Serial.println(key);
}

void normalDisplay(float temp1, float setTemp1, float temp2, float setTemp2)
{
  String val;
  display.clearDisplay();
  //display.drawRect(SCREEN_START_X, SCREEN_START_Y, SCREEN_END_X, SCREEN_END_Y, WHITE);
  
  //UPPER RECTANGLES
  display.drawRect(SCREEN_START_X, SCREEN_START_Y, RECTANGLE_WIDTH - 1 , RECTANGLE_HEIGHT + 10, WHITE);
  display.drawRect(SCREEN_CENTER_X + 1, SCREEN_START_Y, RECTANGLE_WIDTH - 1, RECTANGLE_HEIGHT + 10, WHITE);
  
  //LOWER RECTANGLES
  display.fillRect(SCREEN_CENTER_X + 1, SCREEN_CENTER_Y + 10, RECTANGLE_WIDTH - 1 , RECTANGLE_HEIGHT - 10, WHITE);
  display.fillRect(SCREEN_START_X, SCREEN_CENTER_Y + 10, RECTANGLE_WIDTH - 1, RECTANGLE_HEIGHT - 10, WHITE);
  
  display.setFont(&FreeMonoBold18pt7b);  // Set a custom font
  
  val = String(temp1);
  myText(String(val.charAt(0)), 0 + 8, 32, 1, 1, false);
  myText(String(val.charAt(1)), 0 + 20, 32, 1, 1, false);
  myText(String(val.charAt(2)), 0 + 32, 32, 1, 1, false);

  val = String(temp2);
  myText(String(val.charAt(0)), SCREEN_CENTER_X + 8, 32, 1, 1, false);
  myText(String(val.charAt(1)), SCREEN_CENTER_X + 20, 32, 1, 1, false);
  myText(String(val.charAt(2)), SCREEN_CENTER_X + 32, 32, 1, 1, false);
  
  display.setFont(&FreeMonoBold9pt7b);  // Set a custom font
  
  val = String(setTemp1);
  myText(String(val.charAt(0)), 20 - 1, 58, 1, 0, false);  
  myText(String(val.charAt(1)), 20 + 6, 58, 1, 0, false);
  myText(String(val.charAt(2)), 20 + 12, 58, 1, 0, false);

  val = String(setTemp2);
  myText(String(val.charAt(0)), SCREEN_CENTER_X + 20 - 1, 58, 1, 0, false);  
  myText(String(val.charAt(1)), SCREEN_CENTER_X + 20 + 6, 58, 1, 0, false);
  myText(String(val.charAt(2)), SCREEN_CENTER_X + 20 + 12, 58, 1, 0, false);

  display.fillTriangle(SCREEN_CENTER_X - 10, SCREEN_START_Y + 2, SCREEN_CENTER_X - 10 - 5, SCREEN_START_Y + 7, SCREEN_CENTER_X - 10 + 5, SCREEN_START_Y + 7, WHITE);
  display.fillTriangle(SCREEN_END_X - 10 - 5, SCREEN_START_Y + 3, SCREEN_END_X - 10 + 5, SCREEN_START_Y + 3, SCREEN_END_X - 10, SCREEN_START_Y + 8, WHITE);
  display.display();
}

int getKeypad()
{
  if(digitalRead(key1) == 0)
      currentKey = 1;
  else if(digitalRead(key2) == 0)
      currentKey = 2;
  else if(digitalRead(key3) == 0)
      currentKey = 3;
  else if(digitalRead(key4) == 0)
      currentKey = 4;
  else
      currentKey = 0;
  unsigned long timeDifference = millis() - lastKeyTime;
  if(currentKey != lastKey)
    {
      lastKey = currentKey;
      lastKeyTime = millis();
      return currentKey;
    }
  else
    {
      if(timeDifference >= keyDelay)
        {
          lastKeyTime = millis();
          return currentKey;
        }
      else
        return 0;
    }
}

/*
 * myText(String text, int x, int y,int size, boolean d)
 * text is the text string to be printed
 * x is the integer x position of text
 * y is the integer y position of text
 * z is the text size, 1, 2, 3 etc
 * d is either "true" or "false". Not sure, use true
 */
void myText(String text, int x, int y,int size, int clr, boolean d) 
{
  display.setTextSize(size);
  if(clr == 1)
    display.setTextColor(WHITE);
  else
    display.setTextColor(BLACK);
  display.setCursor(x,y);
  display.println(text);
  if(d)
  {
    display.display();
  }
}
