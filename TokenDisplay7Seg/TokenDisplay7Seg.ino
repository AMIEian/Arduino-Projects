/*
MOSI - pin 11
MISO - pin 12
CLK - pin 13
CS - pin 8
Speaker - 9
*/
#include <SdFat.h>
#include <TMRpcm.h>          //  also need to include this library...
#include <SPI.h>

#define SD_ChipSelectPin 8  //use digital pin 4 on arduino Uno, nano etc, or can use other pins

TMRpcm audio;   // create an object for use in this sketch
SdFat sd;

const int Data = 4;
const int Shift = 6;
const int Store = 5;
const int Bell = 3;

byte DisplayCodes[11] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00};
int Token = 0;
int TokenNumber[] = {0,0,0,0};
boolean UpdateToken = false;

void setup() 
{
  audio.speakerPin = 9;
  
  Serial.begin(9600);
  
  pinMode(Data,OUTPUT);
  pinMode(Shift,OUTPUT);
  pinMode(Store,OUTPUT);
  pinMode(Bell,OUTPUT);

  digitalWrite(Data,LOW);
  digitalWrite(Shift,LOW);
  digitalWrite(Store,LOW);
  digitalWrite(Bell,LOW);

  displayTest();
  /*
  for(int i = 0; i <= 4; i++)
    {
      digitalWrite(13,HIGH);
      delay(1000);
      digitalWrite(13,LOW);
      delay(1000);
    }
  */
  if (!sd.begin(SD_ChipSelectPin, SPI_FULL_SPEED)) { return; 
  }else{ Serial.println("SD OK"); }
  
  audio.setVolume(5);
  //audio.volume(1);
  audio.quality(1);
  //audio.play("music.wav");
  delay(1000);
  
  DisplayToken();                                                                  
}

void loop() 
{
  if(UpdateToken)
    {
      //Sound bell
      DisplayBlankToken();
      delay(100);
      digitalWrite(Bell,HIGH);
      delay(100);
      digitalWrite(Bell,LOW);
      delay(100);
      DisplayToken();
      delay(250);
      SpeakToken();
      UpdateToken = false;
      for(int i = 3; i >= 0; i--)
        TokenNumber[i] = 0;
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
        if(inChar >= '0' && inChar <= '9')
          {
            for(int i = 3; i > 0; i--)
              TokenNumber[i] = TokenNumber[i-1];
            TokenNumber[0] = inChar - 48;
          }
        else
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
        if(TokenNumber[i] == 10 || TokenNumber[i] == 0)
          TokenNumber[i] = 10;
        else
          break;
      }
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

void SpeakToken()
{
  int i = 3;
  
  for(i = 3; i >= 0; i--)
      {
        if(TokenNumber[i] == 10 || TokenNumber[i] == 0)
          TokenNumber[i] = 10;
        else
          break;
      }

  if(i == -1)
    return;
    
  audio.play("bell.wav");
  while(audio.isPlaying());
  audio.play("tn.wav");
  while(audio.isPlaying());
  for(int i = 3; i >= 1; i--)
    {
      if(TokenNumber[i] != 10)
        {
          String fileName = "e" + String(TokenNumber[i]) + ".wav";
          //Serial.println(fileName);
          audio.play(fileName);
          while(audio.isPlaying());
        }
    }
  /*
  audio.play("bell.wav");
  while(audio.isPlaying());
  audio.play("tn.wav");
  while(audio.isPlaying());
  for(int i = 3; i >= 1; i--)
    {
      if(TokenNumber[i] != 10)
        {
          String fileName = "h" + String(TokenNumber[i]) + ".wav";
          //Serial.println(fileName);
          audio.play(fileName);
          while(audio.isPlaying());
        }
    }

  audio.play("bell.wav");
  while(audio.isPlaying());
  audio.play("tn.wav");
  while(audio.isPlaying());
  for(int i = 3; i >= 1; i--)
    {
      if(TokenNumber[i] != 10)
        {
          String fileName = "m" + String(TokenNumber[i]) + ".wav";
          //Serial.println(fileName);
          audio.play(fileName);
          while(audio.isPlaying());
        }
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
