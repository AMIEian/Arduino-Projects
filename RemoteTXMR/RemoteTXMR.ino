#include <hidboot.h>
#include <usbhub.h>

#define FULL_RANGE 255
#define OK_RANGE 128
#define NO_RANGE 0

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

const int Buzzer = A0;
const int RangeIndicator = 3; // Analog output pin that the LED is attached to
const int PowerIndicator = 4;

char currentChar = 'X';
int comCounter = 0;
int range = 0;

int debouncer = 0;

class KbdRptParser : public KeyboardReportParser
{
  void PrintKey(uint8_t mod, uint8_t key);

  protected:
  void OnControlKeysChanged(uint8_t before, uint8_t after);

  void OnKeyDown  (uint8_t mod, uint8_t key);
  void OnKeyUp  (uint8_t mod, uint8_t key);
  void OnKeyPressed(uint8_t key);
};

void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
  /*MODIFIERKEYS mod;
  *((uint8_t*)&mod) = m;
  Serial.print((mod.bmLeftCtrl   == 1) ? "C" : " ");
  Serial.print((mod.bmLeftShift  == 1) ? "S" : " ");
  Serial.print((mod.bmLeftAlt    == 1) ? "A" : " ");
  Serial.print((mod.bmLeftGUI    == 1) ? "G" : " ");

  Serial.print(" >");
  PrintHex<uint8_t>(key, 0x80);
  Serial.print("< ");

  Serial.print((mod.bmRightCtrl   == 1) ? "C" : " ");
  Serial.print((mod.bmRightShift  == 1) ? "S" : " ");
  Serial.print((mod.bmRightAlt    == 1) ? "A" : " ");
  Serial.println((mod.bmRightGUI    == 1) ? "G" : " ");*/
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
   uint8_t c = OemToAscii(mod, key);
   if (c)
      OnKeyPressed(c);
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {
  /*
  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;

  if (beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl) {
    Serial.println("LeftCtrl changed");
  }
  if (beforeMod.bmLeftShift != afterMod.bmLeftShift) {
    Serial.println("LeftShift changed");
  }
  if (beforeMod.bmLeftAlt != afterMod.bmLeftAlt) {
    Serial.println("LeftAlt changed");
  }
  if (beforeMod.bmLeftGUI != afterMod.bmLeftGUI) {
    Serial.println("LeftGUI changed");
  }

  if (beforeMod.bmRightCtrl != afterMod.bmRightCtrl) {
    Serial.println("RightCtrl changed");
  }
  if (beforeMod.bmRightShift != afterMod.bmRightShift) {
    Serial.println("RightShift changed");
  }
  if (beforeMod.bmRightAlt != afterMod.bmRightAlt) {
    Serial.println("RightAlt changed");
  }
  if (beforeMod.bmRightGUI != afterMod.bmRightGUI) {
    Serial.println("RightGUI changed");
  }
  */
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
    if(debouncer >= 2)
       {
          digitalWrite(Buzzer,LOW);
      currentChar = 'X';
          debouncer = 0;
       }
    else
       {
          debouncer++;
       }
}

void KbdRptParser::OnKeyPressed(uint8_t key)
{
    if(key == 19)
    {
      currentChar = 'P';
      debouncer = 3;
    }
  else if((char)key == '.')
    {
      currentChar = 'A';
      debouncer = 0;
    }
  else if((char)key == '0')
    {
      currentChar = 'B';
      debouncer = 0;  
    }
  else if((char)key == '1')
    {
      currentChar = 'C';
      debouncer = 0;  
    }
  else if((char)key == '2')
    {
      currentChar = 'D';
      debouncer = 0;
    }
  else if((char)key == '3')
    {
      currentChar = 'E';
      debouncer = 0;
    }
  else if((char)key == '4')
    {
      currentChar = 'F';
      debouncer = 0;
    }
  else if((char)key == '5')
    {
      currentChar = 'G';
      debouncer = 0;
    }
  else if((char)key == '6')
    {
      currentChar = 'H';
      debouncer = 0;
    }
  else if((char)key == '7')
    {
      currentChar = 'I';
      debouncer = 0;
    }
  else if((char)key == '8')
    {
      currentChar = 'J';
      debouncer = 0;
    }
  else if((char)key == '9')
    {
      currentChar = 'K';
      debouncer = 0;
    }
  else if((char)key == '+')
    {
      currentChar = 'L';
      debouncer = 3;
    }
  else if((char)key == '-')
    {
      currentChar = 'M';
      debouncer = 3;
    }
  else if((char)key == '*')
    {
      currentChar = 'N';
      debouncer = 3;
    }
  else if((char)key == '/')
    {
      currentChar = 'O';
      debouncer = 3;
    }
    Serial.print(currentChar);
  digitalWrite(Buzzer,HIGH);
};

USB     Usb;
//USBHub     Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);

uint32_t next_time;

KbdRptParser Prs;

void updateComCounter(int chars)
{
  if(chars == -1 && comCounter == 21)
    {
      comCounter = 0;
      return;
    }
    
  comCounter = comCounter + chars;
  if(comCounter > 20)
    comCounter = 21;
  if(comCounter < 0)
    comCounter = 0;
}

void dataTransmitter()
{
    Serial.print(currentChar);
  updateComCounter(1);
}

void setup()
{
  Serial.begin( 9600 );
  #if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  #endif
  Serial.println("Start");

  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");

  delay( 200 );

  next_time = millis() + 5000;
  
  HidKeyboard.SetReportParser(0, &Prs);
    
  pinMode(Buzzer,OUTPUT);
  pinMode(RangeIndicator,OUTPUT);
  pinMode(PowerIndicator,OUTPUT);
  digitalWrite(PowerIndicator,HIGH);
}

void loop()
{
  Usb.Task();
    
  if(comCounter < 10)
    range = FULL_RANGE;
  if(comCounter >= 10 && comCounter < 20)
    range = OK_RANGE;
  if(comCounter >= 20)
    range = NO_RANGE;
  analogWrite(RangeIndicator,range);
  
    dataTransmitter();
  delay(5);
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    updateComCounter(-1);
  }
}
