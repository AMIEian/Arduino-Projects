//#include <SoftwareSerial.h>
#include <EEPROM.h>

//SoftwareSerial mySerial(10, 11); // RX, TX

const int Data = 4;
const int Shift = 6;
const int Store = 5;

byte DisplayCodes[12] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00,0x40};
long BaudRates[10] = {1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200};
int BaudIndex = 3;
long BaudRate = 9600;
long updateTime = 0;
int Token = 0;
int TokenNumber[] = {0,0,0,0};
boolean UpdateToken = false;
boolean ChangeBaud = false;

boolean Negative = false;
boolean Intpart = true;
boolean Decpart = false;

void setup() 
{ 
  if(EEPROM.read(0) == '#')
    {
      BaudIndex = EEPROM.read(1) - 48;
      BaudRate = BaudRates[BaudIndex];
    }
  Serial.begin(BaudRate);
  //mySerial.begin(9600);
  
  pinMode(Data,OUTPUT);
  pinMode(Shift,OUTPUT);
  pinMode(Store,OUTPUT);

  digitalWrite(Data,LOW);
  digitalWrite(Shift,LOW);
  digitalWrite(Store,LOW);
  
  displayTest();

  TokenNumber[3] = 0;
  TokenNumber[2] = 0;
  TokenNumber[1] = 0;
  TokenNumber[0] = 0;
  
  DisplayToken();
}

void loop() 
{
  //Serial.println("Hi");
  if(UpdateToken)
    {
      DisplayToken();
      updateTime = 0;
      UpdateToken = false;
      Negative = false;
      Intpart = true;
      Decpart = false;
      for(int i = 3; i >= 0; i--)
        TokenNumber[i] = 0;
    }

  delay(1);
  updateTime++;
  if(updateTime > 900000)
    {
      updateTime = 900000;
      DisplayBlankToken();
    }
  /*
  if (mySerial.available()) {
    char mySerChar = (char)mySerial.read();
    if(mySerChar >= '0' && mySerChar <= '9' && ChangeBaud == true)
      {
        ChangeBaud = false;
        BaudIndex = mySerChar - 48;
        BaudRate = BaudRates[BaudIndex];
        //Save new baud rate in EEPROM
        EEPROM.write(0,'#');
        EEPROM.write(1,mySerChar);
        Serial.flush();
        delay(2);
        Serial.end();
        delay(500);
        Serial.begin(BaudRate);
        while(Serial.available())
          Serial.read();
      }
    else if(mySerChar == 'P')
      ChangeBaud = true;
    else
      Serial.write(mySerial.read());
  }
 */
}

void displayTest()
  {
    for(int i = 3; i >= 0; i--)
        {
          for(int j = 9; j >= 0; j--)
            {
              TokenNumber[i] = j;
              DisplayToken();
              delay(100);
            }
        }
  }

void changeToken()
{
  Token = Token + 1;
  if(Token < 10000)
    {
      int divider = 1000;
      int temp = Token;
      for(int i = 3; i >= 0; i--)
        {
          TokenNumber[i] = temp / divider;
          temp = temp - (TokenNumber[i]*divider);
          divider = divider / 10;
        }
    }
  else
    {
      Token = 0;
      for(int i = 3; i >= 0; i--)
        TokenNumber[i] = 0;
    }
  UpdateToken = true;
}

void serialEvent() {
    while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();
        //Serial.print(inChar);

        if(inChar == 'P')
          ChangeBaud = true;
        
        if(inChar == '-')
          Negative = true;
          
        if(inChar == '.')
          { 
            Intpart = false;
            Decpart = true;
          }
          
        if(inChar >= '0' && inChar <= '9')
          {
            if(Intpart)
              {
                for(int i = 3; i > 0; i--)
                  TokenNumber[i] = TokenNumber[i-1];
                TokenNumber[0] = inChar - 48;
              }

            if(ChangeBaud)
              {
                ChangeBaud = false;
                BaudIndex = inChar - 48;
                BaudRate = BaudRates[BaudIndex];
                //Save new baud rate in EEPROM
                EEPROM.write(0,'#');
                EEPROM.write(1,inChar);
                Serial.flush();
                delay(2);
                Serial.end();
                delay(500);
                Serial.begin(BaudRate);
                while(Serial.available())
                  Serial.read();
              }
          }
          
        if(inChar == '\n' || inChar == '\r')
            UpdateToken = true;
    }
}

void DisplayBlankToken()
{
  unsigned short Mask, bits, Flag, digit, number;
  for(digit = 0; digit < 4; digit++)
     {
        number = DisplayCodes[10];
        Mask = 0x01;
        for (bits=0; bits<8; bits++)
           {
              Flag = number & Mask;
              if(Flag == 0) 
                {
                  digitalWrite(Data,LOW);
                  delayMicroseconds(1);
                  digitalWrite(Shift,HIGH);
                  delayMicroseconds(1);
                  digitalWrite(Shift,LOW);
                  delayMicroseconds(1);  
                }
              if(Flag > 0)
                {
                  digitalWrite(Data,HIGH);   
                  delayMicroseconds(1);
                  digitalWrite(Shift,HIGH);
                  delayMicroseconds(1);
                  digitalWrite(Shift,LOW);
                  delayMicroseconds(1);
                }
              Mask = Mask << 1;
           }
     } 
  digitalWrite(Store,HIGH);
  delayMicroseconds(1);
  digitalWrite(Store,LOW);
}

void DisplayToken()
{
  unsigned short Mask, bits, Flag, digit, number;
  for(int i = 3; i > 0; i--)
    {
      if(TokenNumber[i] == 0)
        TokenNumber[i] = 10;
      else
        break;
    }
  if(Negative)
    {
      if(TokenNumber[3] != 10)
        {
          for(int i = 0; i < 3; i++)
            TokenNumber[i] = TokenNumber[i+1];
        }
      TokenNumber[3] = 11;  
    }
  /*  
  for(int i = 3; i >= 0; i--)
    {
      if(TokenNumber[i] == 11)
        Serial.print('-');
      else
        Serial.print(TokenNumber[i]);
    }
  Serial.println();  
  */
  for(digit = 0; digit < 4; digit++)
     {
        number = DisplayCodes[TokenNumber[digit]];
        Mask = 0x01;
        for (bits=0; bits<8; bits++)
           {
              Flag = number & Mask;
              if(Flag == 0) 
                {
                  digitalWrite(Data,LOW);
                  delayMicroseconds(1);
                  digitalWrite(Shift,HIGH);
                  delayMicroseconds(1);
                  digitalWrite(Shift,LOW);
                  delayMicroseconds(1);  
                }
              if(Flag > 0)
                {
                  digitalWrite(Data,HIGH);   
                  delayMicroseconds(1);
                  digitalWrite(Shift,HIGH);
                  delayMicroseconds(1);
                  digitalWrite(Shift,LOW);
                  delayMicroseconds(1);
                }
              Mask = Mask << 1;
           }
     } 
  digitalWrite(Store,HIGH);
  delayMicroseconds(1);
  digitalWrite(Store,LOW);
}
