//KEYPAD - ESC 2, ENTR 3, DWN 4, UP 5
//RELAY 6

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <Fonts/FreeMonoBold18pt7b.h>  // Add a custom font
#include <Fonts/FreeMonoBold12pt7b.h>  // Add a custom font
#include <Fonts/FreeMonoBold9pt7b.h>  // Add a custom font
#include <Bounce2.h>
#include <EEPROM.h>

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
#define OLED_RESET     8 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int key1 = 2;
int key2 = 3;
int key3 = 4;
int key4 = 5;
int relay1 = 6;
int relay2 = 7;

Bounce k1 = Bounce(); 
Bounce k2 = Bounce();
Bounce k3 = Bounce();
Bounce k4 = Bounce();

float setLowTemp = 0.0;
float setHighTemp = 9.9;
float currentLowTemp = 0.0;
float currentHighTemp = 9.9;
bool lowTempAlarm = false;
bool highTempAlarm = false;
int currentKey = 0;
int lastKey = 0;
unsigned long lastKeyTime = 0;
int keyDelay = 10000;
int lastUpdate = 0;

int offset[] = {0, 0};
float gain[] = {1.0, 1.0};

String inputString;

void setup()   
{                
  pinMode(key1,INPUT_PULLUP);
  pinMode(key2,INPUT_PULLUP);
  pinMode(key3,INPUT_PULLUP);
  pinMode(key4,INPUT_PULLUP);
  pinMode(relay1,OUTPUT);
  digitalWrite(relay1, LOW);
  pinMode(relay2,OUTPUT);
  digitalWrite(relay2, LOW);
  
  k1.attach(key1);
  k1.interval(10);
  k2.attach(key2);
  k2.interval(10);
  k3.attach(key3);
  k3.interval(10);
  k4.attach(key4);
  k4.interval(10);
  
  Serial.begin(9600);
  
  EEPROM.get(0,setLowTemp);
  if(isnan(setLowTemp))
    setLowTemp = 2.9;
    
  EEPROM.get(10,setHighTemp);
  if(isnan(setHighTemp))
    setHighTemp = 6.9;
  /*
  EEPROM.get(20,offset[0]);
  if(isnan(offset[0]))
    offset[0] = 0;  

  EEPROM.get(30,offset[1]);
  if(isnan(offset[1]))
    offset[1] = 1.0;  

  EEPROM.get(40,gain[0]);
  if(isnan(gain[0]))
    gain[0] = 0;  

  EEPROM.get(50,gain[1]);
  if(isnan(gain[1]))
    gain[1] = 1.0;
  */
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.cp437(true);
}


void loop() 
{
  if(millis() - lastUpdate > 250)
    {
      checkAlarms();
      normalDisplay(currentLowTemp, setLowTemp, currentHighTemp, setHighTemp);
      lastUpdate = millis();
    }
  int key = getKeypad();
  if(key != 0)
    {
      bool pass = passwordScreen();
      if(pass == true)
        {
          lowTempSetScreen();
          highTempSetScreen();
          offsetSetScreen();
          gainSetScreen();
        }
    }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    if (inChar == 'a') {
      currentLowTemp = inputString.toFloat();
      inputString = "";
    }
    if (inChar == 'b') {
      currentHighTemp = inputString.toFloat();
      inputString = "";
    }
  }
}

void checkAlarms()
{
  if(currentLowTemp <= setLowTemp || currentHighTemp >= setHighTemp)
    {
      lowTempAlarm = true;
      digitalWrite(relay1, HIGH);
      digitalWrite(relay2, HIGH);
    }
  if(currentLowTemp > setLowTemp || currentHighTemp < setHighTemp)
    {
      lowTempAlarm = false;
      digitalWrite(relay1, LOW);
      digitalWrite(relay2, LOW);
    }
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
  display.fillRect(SCREEN_CENTER_X + 1, SCREEN_CENTER_Y, RECTANGLE_WIDTH - 1 , RECTANGLE_HEIGHT - 16, WHITE);
  display.fillRect(SCREEN_START_X, SCREEN_CENTER_Y, RECTANGLE_WIDTH - 1, RECTANGLE_HEIGHT - 16, WHITE);

  display.drawRoundRect(SCREEN_START_X, 49, SCREEN_WIDTH/2, 15, 5, WHITE);
  display.drawRoundRect(SCREEN_CENTER_X + 1, 49, SCREEN_WIDTH/2-1, 15, 5, WHITE);
  
  display.setFont(&FreeMonoBold12pt7b);
  val = String(temp1,1);
  myText(String(val.charAt(0)), 0 + 12, 24, 1, 1, false);
  myText(String(val.charAt(1)), 0 + 24, 24, 1, 1, false);
  myText(String(val.charAt(2)), 0 + 36, 24, 1, 1, false);

  val = String(temp2,1);
  myText(String(val.charAt(0)), SCREEN_CENTER_X + 12, 24, 1, 1, false);
  myText(String(val.charAt(1)), SCREEN_CENTER_X + 24, 24, 1, 1, false);
  myText(String(val.charAt(2)), SCREEN_CENTER_X + 36, 24, 1, 1, false);
  
  display.setFont(&FreeMonoBold9pt7b);  // Set a custom font
  myText("COLD", 9, 61, 1, 1, false);
  myText("HOT", SCREEN_CENTER_X + 15, 61, 1, 1, false);
  val = String(setTemp1,1);
  myText(val, 15, 45, 1, 0, false);
  
  val = String(setTemp2,1);
  myText(val, SCREEN_CENTER_X + 15, 45, 1, 0, false);
  
  if(lowTempAlarm)
    //display.fillTriangle(SCREEN_CENTER_X - 10, SCREEN_START_Y + 2, SCREEN_CENTER_X - 10 - 5, SCREEN_START_Y + 7, SCREEN_CENTER_X - 10 + 5, SCREEN_START_Y + 7, WHITE);
    display.fillTriangle(SCREEN_CENTER_X - 10 - 5, SCREEN_START_Y + 3, SCREEN_CENTER_X - 10 + 5, SCREEN_START_Y + 3, SCREEN_CENTER_X - 10, SCREEN_START_Y + 8, WHITE);
  if(highTempAlarm)
    //display.fillTriangle(SCREEN_END_X - 10 - 5, SCREEN_START_Y + 3, SCREEN_END_X - 10 + 5, SCREEN_START_Y + 3, SCREEN_END_X - 10, SCREEN_START_Y + 8, WHITE);
    display.fillTriangle(SCREEN_END_X - 10, SCREEN_START_Y + 2, SCREEN_END_X - 10 - 5, SCREEN_START_Y + 7, SCREEN_END_X - 10 + 5, SCREEN_START_Y + 7, WHITE);
  display.display();
}

int getKeypad()
{
  int value; 
  currentKey = 0;
  k1.update();
  value = k1.read();
  if(value == LOW)
      currentKey = 4;
  else
    {
      k2.update();
      value = k2.read();
      if(value == LOW)
        currentKey = 3;
      else
        {
          k3.update();
          value = k3.read();
          if(value == LOW)
            currentKey = 2;
          else
            {
              k4.update();
              value = k4.read();
              if(value == LOW)
                currentKey = 1;                
            }
        }
    } 
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

bool passwordScreen()
{
  display.clearDisplay();
  display.drawRect(SCREEN_START_X, SCREEN_START_Y, SCREEN_END_X, SCREEN_END_Y, WHITE);
  //display.setFont(&FreeMonoBold18pt7b);  // Set a custom font
  display.setFont(&FreeMonoBold12pt7b);
  myText("ENTER", 3, 20, 1, 1, false);
  myText("PIN", 84, 20, 1, 1, false);
  myText("- - - -", 15, 60, 1, 1, false);
  display.display();
  String pinText = "";
  String password = "";
  for(int i = 0; i < 4; i++)
    {
      int key;
      key = getKeypad();
      while(key == 0)
        key = getKeypad();
      password = password + String(key);
      display.clearDisplay();
      display.drawRect(SCREEN_START_X, SCREEN_START_Y, SCREEN_END_X, SCREEN_END_Y, WHITE);
      //display.setFont(&FreeMonoBold18pt7b);  // Set a custom font
      display.setFont(&FreeMonoBold12pt7b);
      myText("ENTER", 3, 20, 1, 1, false);
      myText("PIN", 84, 20, 1, 1, false);
      display.setFont(&FreeMonoBold12pt7b);
      myText("- - - -", 15, 60, 1, 1, false);
      pinText = pinText + "* ";
      myText(pinText, 15, 50, 1, 1, false);
      display.display();
    }
  if(password == "1342")
    {
      //drawFace(true);
      return true;
    }
  else
    {
      //drawFace(false);
      return false;
    }
}

void lowTempSetScreen()
{
  keyDelay = 250;
  float temp = setLowTemp;
  bool saved = false;
  int key;
  key = getKeypad();
  while(key != 4)
    {
      key = getKeypad();
      if(key == 1)
        {
          temp = temp + 0.1;
          if(temp >= 10.0)
            temp = 9.9;
        }
      if(key == 2)
        {
          temp = temp - 0.1;
          if(temp <= 0.0)
            temp = 0.0;
        }
      if(key == 3)
        {
          setLowTemp = temp;
          EEPROM.put(0,setLowTemp);
          saved = true;
        }
      display.clearDisplay();
      display.drawRect(SCREEN_START_X, SCREEN_START_Y, SCREEN_END_X, SCREEN_END_Y, WHITE);
      display.setFont(&FreeMonoBold9pt7b);  // Set a custom font
      myText("SET LOWER", 15, 15, 1, 1, false);
      myText("TEMP.", 40, 33, 1, 1, false);
      display.setFont(&FreeMonoBold12pt7b);
      if(saved)
        {
          myText("SAVED", 30, 58, 1, 1, false);
          delay(2000);
          saved = false;
        }
      else
        myText(String(temp,1), 43, 58, 1, 1, false);
      display.display();
    }
  keyDelay = 10000;
}

void highTempSetScreen()
{
  keyDelay = 250;
  float temp = setHighTemp;
  bool saved = false;
  int key;
  key = getKeypad();
  while(key != 4)
    {
      key = getKeypad();
      if(key == 1)
        {
          temp = temp + 0.1;
          if(temp >= 10.0)
            temp = 9.9;
        }
      if(key == 2)
        {
          temp = temp - 0.1;
          if(temp <= 0.0)
            temp = 0.0;
        }
      if(key == 3)
        {
          setHighTemp = temp;
          EEPROM.put(10,setHighTemp);
          saved = true;
        }
      display.clearDisplay();
      display.drawRect(SCREEN_START_X, SCREEN_START_Y, SCREEN_END_X, SCREEN_END_Y, WHITE);
      display.setFont(&FreeMonoBold9pt7b);  // Set a custom font
      myText("SET HIGHER", 9, 15, 1, 1, false);
      myText("TEMP.", 40, 33, 1, 1, false);
      display.setFont(&FreeMonoBold12pt7b);
      if(saved)
        {
          myText("SAVED", 30, 58, 1, 1, false);
          delay(2000);
          saved = false;
        }
      else
        myText(String(temp,1), 43, 58, 1, 1, false);
      display.display();
    }
  keyDelay = 10000;
}

void offsetSetScreen()
{
  keyDelay = 250;
  int temp = offset[0];
  bool saved = false;
  int key;
  key = getKeypad();
  while(key != 4)
    {
      key = getKeypad();
      if(key == 1)
        {
          temp = temp + 1;
          if(temp >= 10)
            temp = 20;
        }
      if(key == 2)
        {
          temp = temp - 1;
          if(temp <= -10)
            temp = -20;
        }
      if(key == 3)
        {
          offset[0] = temp;
          Serial.print(temp);
          Serial.print('a');
          EEPROM.put(20,temp);
          saved = true;
        }
      display.clearDisplay();
      display.drawRect(SCREEN_START_X, SCREEN_START_Y, SCREEN_END_X, SCREEN_END_Y, WHITE);
      display.setFont(&FreeMonoBold9pt7b);  // Set a custom font
      myText("SET OFFSET", 9, 15, 1, 1, false);
      myText("TEMP.1", 30, 33, 1, 1, false);
      display.setFont(&FreeMonoBold12pt7b);
      if(saved)
        {
          myText("SAVED", 30, 58, 1, 1, false);
          delay(2000);
          saved = false;
        }
      else
        myText(String(temp), 50, 58, 1, 1, false);
      display.display();
    }
    
  temp = offset[1];
  saved = false;
  key = getKeypad();
  while(key != 4)
    {
      key = getKeypad();
      if(key == 1)
        {
          temp = temp + 1;
          if(temp >= 10)
            temp = 10;
        }
      if(key == 2)
        {
          temp = temp - 1;
          if(temp <= -10)
            temp = -10;
        }
      if(key == 3)
        {
          offset[1] = temp;
          Serial.print(temp);
          Serial.print('b');
          EEPROM.put(30,temp);
          saved = true;
        }
      display.clearDisplay();
      display.drawRect(SCREEN_START_X, SCREEN_START_Y, SCREEN_END_X, SCREEN_END_Y, WHITE);
      display.setFont(&FreeMonoBold9pt7b);  // Set a custom font
      myText("SET OFFSET", 9, 15, 1, 1, false);
      myText("TEMP.2", 30, 33, 1, 1, false);
      display.setFont(&FreeMonoBold12pt7b);
      if(saved)
        {
          myText("SAVED", 30, 58, 1, 1, false);
          delay(2000);
          saved = false;
        }
      else
        myText(String(temp), 50, 58, 1, 1, false);
      display.display();
    }
  keyDelay = 10000;
}

void gainSetScreen()
{
  keyDelay = 250;
  float temp = gain[0];
  bool saved = false;
  int key;
  key = getKeypad();
  while(key != 4)
    {
      key = getKeypad();
      if(key == 1)
        {
          temp = temp + 0.01;
          if(temp >= 1.2)
            temp = 1.2;
        }
      if(key == 2)
        {
          temp = temp - 0.01;
          if(temp <= 0.80)
            temp = 0.80;
        }
      if(key == 3)
        {
          gain[0] = temp;
          Serial.print(temp);
          Serial.print('c');
          EEPROM.put(40,temp);
          saved = true;
        }
      display.clearDisplay();
      display.drawRect(SCREEN_START_X, SCREEN_START_Y, SCREEN_END_X, SCREEN_END_Y, WHITE);
      display.setFont(&FreeMonoBold9pt7b);  // Set a custom font
      myText("SET GAIN", 20, 15, 1, 1, false);
      myText("TEMP.1", 30, 33, 1, 1, false);
      display.setFont(&FreeMonoBold12pt7b);
      if(saved)
        {
          myText("SAVED", 30, 58, 1, 1, false);
          delay(2000);
          saved = false;
        }
      else
        myText(String(temp), 35, 58, 1, 1, false);
      display.display();
    }
    
  temp = gain[1];
  saved = false;
  key = getKeypad();
  while(key != 4)
    {
      key = getKeypad();
      if(key == 1)
        {
          temp = temp + 0.01;
          if(temp >= 1.2)
            temp = 1.2;
        }
      if(key == 2)
        {
          temp = temp - 0.01;
          if(temp <= 0.80)
            temp = 0.80;
        }
      if(key == 3)
        {
          gain[1] = temp;
          Serial.print(temp);
          Serial.print('d');
          EEPROM.put(50,temp);
          saved = true;
        }
      display.clearDisplay();
      display.drawRect(SCREEN_START_X, SCREEN_START_Y, SCREEN_END_X, SCREEN_END_Y, WHITE);
      display.setFont(&FreeMonoBold9pt7b);  // Set a custom font
      myText("SET GAIN", 20, 15, 1, 1, false);
      myText("TEMP.2", 30, 33, 1, 1, false);
      display.setFont(&FreeMonoBold12pt7b);
      if(saved)
        {
          myText("SAVED", 30, 58, 1, 1, false);
          delay(2000);
          saved = false;
        }
      else
        myText(String(temp), 35, 58, 1, 1, false);
      display.display();
    }
  keyDelay = 10000;
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
/*
void drawFace(bool happy)
{
  display.clearDisplay();
  display.drawRect(SCREEN_START_X, SCREEN_START_Y, SCREEN_END_X, SCREEN_END_Y, WHITE);
  display.drawCircle(display.width()/2, display.height()/2, 20, SSD1306_WHITE);
  display.fillCircle(display.width()/2 - 8, display.height()/2 - 7, 5, SSD1306_WHITE);
  display.fillCircle(display.width()/2 + 8, display.height()/2 - 7, 5, SSD1306_WHITE);
  
  
  if(happy)
    {
      display.fillCircle(display.width()/2 - 8, display.height()/2 - 9, 2, SSD1306_BLACK);
      display.fillCircle(display.width()/2 + 8, display.height()/2 - 9, 2, SSD1306_BLACK);
      display.fillCircle(display.width()/2, display.height()/2 + 13, 2, SSD1306_WHITE);
      display.setRotation(1);
      display.setFont(&FreeMonoBold9pt7b);
      myText(")", display.width()/2 + 7, display.height()/2 + 3, 1, 1, false);
    }
  else
    {
      display.fillCircle(display.width()/2 - 8, display.height()/2 - 5, 2, SSD1306_BLACK);
      display.fillCircle(display.width()/2 + 8, display.height()/2 - 5, 2, SSD1306_BLACK);
      display.setRotation(1);
      display.setFont(&FreeMonoBold9pt7b);
      myText("(", display.width()/2 + 3, display.height()/2 + 3, 1, 1, false);
    }
  display.setRotation(0);
  display.display();
  delay(2000);
}
*/
