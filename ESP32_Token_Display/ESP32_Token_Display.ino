#include "BluetoothSerial.h"
#include "XT_DAC_Audio.h"
#include "bell.h"
#include "tn.h"
#include "e0.h"
#include "e1.h"
#include "e2.h"
#include "e3.h"
#include "e4.h"
#include "e5.h"
#include "e6.h"
#include "e7.h"
#include "e8.h"
#include "e9.h"

#define NUMBER_OF_DIGITS 4

const int Data = 22;//22;
const int Shift = 27;
const int Store = 26;
const int Bell = 33;

byte DisplayCodes[11] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00};
int Token = 0;
int TokenNumber[] = {0,0,0,0};
boolean UpdateToken = false;

XT_Wav_Class dingDong(bell);
XT_Wav_Class tokenNo(tn);
XT_Wav_Class Zero(e0);          
XT_Wav_Class One(e1);          
XT_Wav_Class Two(e2);           
XT_Wav_Class Three(e3);           
XT_Wav_Class Four(e4);           
XT_Wav_Class Five(e5);       
XT_Wav_Class Six(e6);        
XT_Wav_Class Seven(e7);          
XT_Wav_Class Eight(e8);           
XT_Wav_Class Nine(e9);   

XT_DAC_Audio_Class DacAudio(25,0);//(25,0);    // Create the main player class object. 
                                      // Use GPIO 25, one of the 2 DAC pins and timer 0
                                      
XT_Sequence_Class Sequence;               // The sequence object, you add your sounds above to this object (see setup below)

BluetoothSerial SerialBT;

void setup() {
  pinMode(Data,OUTPUT);
  pinMode(Shift,OUTPUT);
  pinMode(Store,OUTPUT);
  pinMode(Bell,OUTPUT);

  digitalWrite(Data,LOW);
  digitalWrite(Shift,LOW);
  digitalWrite(Store,LOW);
  digitalWrite(Bell,LOW);

  //Serial.begin(9600);

  SerialBT.begin("Eagle Token Display"); //Bluetooth device name
  delay(1000);
  
  displayTest();
  DisplayToken();
}

void loop() {
  DacAudio.FillBuffer();          // This needs only be in your main loop once, suggest here at the top.
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
      for(int i = NUMBER_OF_DIGITS; i >= 0; i--)
        TokenNumber[i] = 0;
    }
  serialEvent();
}

void changeToken()
{
  Token = Token + 1;
  if(Token < 10000)
    {
      int divider = 1000;
      int temp = Token;
      for(int i = NUMBER_OF_DIGITS; i >= 0; i--)
        {
          TokenNumber[i] = temp / divider;
          temp = temp - (TokenNumber[i]*divider);
          divider = divider / 10;
        }
    }
  else
    {
      Token = 0;
      for(int i = NUMBER_OF_DIGITS; i >= 0; i--)
        TokenNumber[i] = 0;
    }
  UpdateToken = true;
}

void serialEvent() {
  
    while (SerialBT.available()) {
        // get the new byte:
        char inChar = (char)SerialBT.read();
        if(inChar >= '0' && inChar <= '9')
          {
            for(int i = NUMBER_OF_DIGITS; i > 0; i--)
              TokenNumber[i] = TokenNumber[i-1];
            TokenNumber[0] = inChar - 48;
          }
        else
          UpdateToken = true;
    }
    
    /*
    while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();
        if(inChar >= '0' && inChar <= '9')
          {
            for(int i = NUMBER_OF_DIGITS; i > 0; i--)
              TokenNumber[i] = TokenNumber[i-1];
            TokenNumber[0] = inChar - 48;
          }
        else
          UpdateToken = true;
    }
    */
}

void DisplayBlankToken()
{
  unsigned short Mask, bits, Flag, digit, number;
  for(digit = 0; digit < NUMBER_OF_DIGITS; digit++)
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
    //Disable leading zeroes
    for(int i = (NUMBER_OF_DIGITS - 1); i > 0; i--)
      {
        if(TokenNumber[i] == 10 || TokenNumber[i] == 0)
          TokenNumber[i] = 10;
        else
          break;
      }
    //Pump the data into displays from 0 to max
    for(digit = 0; digit < NUMBER_OF_DIGITS; digit++)
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
  
  for(i = NUMBER_OF_DIGITS; i >= 0; i--)
      {
        if(TokenNumber[i] == 10 || TokenNumber[i] == 0)
          TokenNumber[i] = 10;
        else
          break;
      }

  if(i == -1)
    return;
    
  Sequence.RemoveAllPlayItems();            // Clear out any previous playlist
  
  Sequence.AddPlayItem(&dingDong);
  Sequence.AddPlayItem(&tokenNo);
  
  for(int i = (NUMBER_OF_DIGITS-1); i > 0; i--)
    {
      if(TokenNumber[i] != 10)
        {
          AddNumberToSequence(TokenNumber[i]);
        }
    }
  DacAudio.Play(&Sequence);                 // Play the sequence, will not wait here to complete, works independently of your code
}

void displayTest()
  {
    for(int i = (NUMBER_OF_DIGITS-1); i > 0; i--)
        {
          for(int j = 9; j >= 0; j--)
            {
              TokenNumber[i] = j;
              DisplayToken();
              delay(100);
            }
        }
  }

void AddNumberToSequence(int TheNumber)
  {
    // Adds in the wav for the single 0-9 number passed in as a char
  
    switch(TheNumber)
      {
        case 0 : Sequence.AddPlayItem(&Zero);break;
        case 1 : Sequence.AddPlayItem(&One);break;
        case 2 : Sequence.AddPlayItem(&Two);break;
        case 3 : Sequence.AddPlayItem(&Three);break;
        case 4 : Sequence.AddPlayItem(&Four);break;
        case 5 : Sequence.AddPlayItem(&Five);break;
        case 6 : Sequence.AddPlayItem(&Six);break;
        case 7 : Sequence.AddPlayItem(&Seven);break;
        case 8 : Sequence.AddPlayItem(&Eight);break;
        case 9 : Sequence.AddPlayItem(&Nine);break;
      }
  }
