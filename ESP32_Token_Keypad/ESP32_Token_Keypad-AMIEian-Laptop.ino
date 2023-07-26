#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {13, 12, 14, 27};//{36, 39, 34, 35}; //connect to the row pinouts of the keypad
byte colPins[COLS] =   {26, 25, 33, 32};//{32, 33, 25, 26};//{27, 14, 12, 13};//connect to the column pinouts of the keypad
//LiquidCrystal lcd(22,23,5,18,19,21);

//Set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

//Set LCD address, number of columns and rows
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

#define buzzer 15

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int token = 0;
int keyCount = 0;

String name = "Eagle Token Display";  //"ESP32_Unleashed";
//char *pin = "1234"; //<- standard pin would be provided by default
bool connected;

void setup(){
  Serial.begin(115200);
  for(int i = 0; i < 10; i++)
  {
    digitalWrite(buzzer, HIGH);
    Serial.println(i);
    delay(1000);
    digitalWrite(buzzer, LOW);
    delay(1000);
  }
  /*
  SerialBT.begin("Eagle Keypad", true); 
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();  

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  
  connected = false;
  connected = SerialBT.connect(name);
 
  if(connected) 
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Connected...!");
    } 
  else 
    {
      while(!SerialBT.connected(10000)) 
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Connecting...");
          lcd.setCursor(0, 1);
          lcd.print("Check Display..."); 
        }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Connected...!");
    }
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  TOKEN NUMBER");
  */
}
  
void loop(){
  char key = keypad.getKey();
  if (key){
    Serial.println(key);
  }
  /*
  if (key){
    if(key >= '0' && key <= '9')
      {
        if(keyCount == 3)
          {
            keyCount = 0;
            token = 0;
          }
        if(token == 0 && key == '0')
          token = 0;
        else
          {
            token = token * 10 + int(key-48);
            displayLCDToken(token);
            keyCount++;
          }
      }
    if(key == '#')
      {
        if(token < 999)
          token++;
        displayLCDToken(token);
      }
    if(key == '*')
      {
        if(token > 0)
          token--;
        displayLCDToken(token);
      }
    if(key == 'A')
      {
        if(token < 994)
          token = token + 5;
        displayLCDToken(token);
      }
    if(key == 'B')
      {
        if(token >= 5)
          token = token - 5;
        displayLCDToken(token);
      }
    if(key == 'C')
      {
        if(token > 0)
          token = 0;
        displayLCDToken(token);
      }
    if(key == 'D')
      {
        sendToken(token);
        keyCount = 3;
      }
    digitalWrite(buzzer, HIGH);
    delay(250);
    digitalWrite(buzzer, LOW);
    delay(250);
  }
  */
}

void displayLCDToken(int tkn)
{
  Serial.println(tkn);
  // clears the display to print new message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  TOKEN NUMBER");
  lcd.setCursor(6,1);
  if(tkn == 0)
    lcd.print("   ");
  else
    lcd.print(tkn);
}

void sendToken(int tkn)
{
  if (tkn<100)
    {
      Serial.print('0');
      SerialBT.print('0');
    }
  if (tkn<10)
    {
      Serial.print('0');
      SerialBT.print('0');
    }
  Serial.print(tkn);
  SerialBT.print(tkn);
  Serial.print('0');
  SerialBT.print('0');
  Serial.print('#');
  SerialBT.print('#');
}
