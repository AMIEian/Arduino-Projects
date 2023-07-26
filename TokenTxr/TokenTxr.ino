#include <Keypad.h>
#include <TM1637Display.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

// Module connection pins (Digital Pins)
#define CLK 10
#define DIO 11
#define buzzer 12

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
TM1637Display display(CLK, DIO);

int token = 0;
int keyCount = 0;

void setup(){
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  display.setBrightness(0x0f);
}
  
void loop(){
  char key = keypad.getKey();
  
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
            display.showNumberDec(token, false);
            keyCount++;
          }
      }
    if(key == '#')
      {
        if(token < 999)
          token++;
        display.showNumberDec(token, false);
      }
    if(key == '*')
      {
        if(token > 0)
          token--;
        display.showNumberDec(token, false);
      }
    if(key == 'A')
      {
        if(token < 994)
          token = token + 5;
        display.showNumberDec(token, false);
      }
    if(key == 'B')
      {
        if(token >= 5)
          token = token - 5;
        display.showNumberDec(token, false);
      }
    if(key == 'C')
      {
        if(token > 0)
          token = 0;
        display.showNumberDec(token, false);
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
}

void sendToken(int tkn)
{
  if (tkn<100) Serial.print('0');
  if (tkn<10) Serial.print('0');
  Serial.print(tkn);
  Serial.print('0');
  Serial.print('#');
}

