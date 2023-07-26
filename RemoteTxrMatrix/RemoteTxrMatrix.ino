#include <Keypad.h>
#include <Bounce2.h>

#define FULL_RANGE 255
#define OK_RANGE 128
#define NO_RANGE 0

const int RangeIndicator = 9; // Analog output pin that the LED is attached to
const int PowerIndicator = 10;
const int RedSwitch = 11;
const int YellowSwitch = 12;
const int BlueSwitch = 13;
const int Buzzer = A0;
const int StopSwitch = A1;
const int PowerCheck = A2;

int analogReading;
int power;

Bounce RedButton = Bounce();
Bounce YellowButton = Bounce();
Bounce BlueButton = Bounce();
Bounce StopButton = Bounce();

int comCounter = 0;
int range = 0;
char matrixKey = 0x00;
char toggleSwitches = 0x00;
char combinedValue = 0x00;
boolean KeyPadPressed = false;

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns

//define the cymbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  {0x02,0x03,0x04},
  {0x05,0x06,0x07},
  {0x08,0x09,0x0A},
  {0x0B,0x0C,0x0D}
};

byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
  Serial.begin(9600);
  
  pinMode(RangeIndicator,OUTPUT);
  pinMode(PowerIndicator,OUTPUT);
  pinMode(RedSwitch,INPUT_PULLUP);
  pinMode(YellowSwitch,INPUT_PULLUP);
  pinMode(BlueSwitch,INPUT_PULLUP);
  pinMode(StopSwitch,INPUT_PULLUP);
  pinMode(Buzzer,OUTPUT);
  
  RedButton.attach(RedSwitch);
  RedButton.interval(5);
  YellowButton.attach(YellowSwitch);
  YellowButton.interval(5);
  BlueButton.attach(BlueSwitch);
  BlueButton.interval(5);
  StopButton.attach(StopSwitch);
  StopButton.interval(5);
  
  keypad.addEventListener(keypadEvent); // Add an event listener for this keypad
}
  
void loop(){
  if(KeyPadPressed == false)
    matrixKey = keypad.getKey();
  if(KeyPadPressed == true)
    char customKey = keypad.getKey();
  
  RedButton.update();
  YellowButton.update();
  BlueButton.update();
  StopButton.update();
  if(RedButton.read() == LOW)
    toggleSwitches = toggleSwitches | 0x01;
  else
    toggleSwitches = toggleSwitches & 0x0E;
    
  if(YellowButton.read() == LOW)
    toggleSwitches = toggleSwitches | 0x02;
  else
    toggleSwitches = toggleSwitches & 0x0D;
  
  if(BlueButton.read() == LOW)
    toggleSwitches = toggleSwitches | 0x04;
  else
    toggleSwitches = toggleSwitches & 0x0B;
    
  if(StopButton.read() == LOW)
    toggleSwitches = toggleSwitches | 0x08;
  else
    toggleSwitches = toggleSwitches & 0x07;
  
  combinedValue = (matrixKey << 4) | toggleSwitches;
  Serial.print(combinedValue);
  updateComCounter(1);
  
  if(comCounter < 50)
    range = FULL_RANGE;
  if(comCounter >= 50 && comCounter < 100)
    range = OK_RANGE;
  if(comCounter >= 100)
    range = NO_RANGE;
  analogWrite(RangeIndicator,range);
  
  analogReading = analogRead(PowerCheck);
  power = map(analogReading, 438, 538, 200, 255);
  analogWrite(PowerIndicator,power);
  
  delay(1);
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    updateComCounter(-1);
  }
}

// Taking care of some special events.
void keypadEvent(KeypadEvent key){
    switch (keypad.getState()){
    case PRESSED:
        //Serial.print("PRESSED");
        KeyPadPressed = true;
        digitalWrite(Buzzer,HIGH);
        break;

    case RELEASED:
        //Serial.print("RELEASED");
        digitalWrite(Buzzer,LOW);
        KeyPadPressed = false;
        matrixKey = 0x00;
        break;
    
    case HOLD:
        break;
    }
}

void updateComCounter(int chars)
{
  if(chars == -1 && comCounter == 101)
    {
      comCounter = 0;
      return;
    }
    
  comCounter = comCounter + chars;
  if(comCounter > 100)
    comCounter = 101;
  if(comCounter < 0)
    comCounter = 0;
}