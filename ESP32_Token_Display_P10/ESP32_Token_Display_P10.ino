#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMD.h>        //
#include <Ticker.h>   //
#include "MyBigFont.h"
#include "SystemFont5x7.h"
#include "BluetoothSerial.h"
#include "XT_DAC_Audio.h";
#include "bell.h";
#include "tn.h";
#include "e0.h";
#include "e1.h";
#include "e2.h";
#include "e3.h";
#include "e4.h";
#include "e5.h";
#include "e6.h";
#include "e7.h";
#include "e8.h";
#include "e9.h";

#define NUMBER_OF_DIGITS 4

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
Ticker ticker;

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

int Token = 0;
int TokenNumber[] = {0,0,0,0};
boolean UpdateToken = false;

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void ScanDMD()
{ 
  dmd.scanDisplayBySPI();
}

void setup() {
   //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
   //Timer1.initialize( 5000 );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
   //Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
   ticker.attach_ms(1, ScanDMD);
   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( true );   //true is normal (all pixels off), false is negative (all pixels on)
   dmd.selectFont(angka_besar_kuru);

  Serial.begin(115200);

  SerialBT.begin("Eagle Token Display"); //Bluetooth device name
  delay(1000);
  
  displayTest();
  DisplayBlankToken();
  DisplayToken();
}

void loop() {
  DacAudio.FillBuffer();          // This needs only be in your main loop once, suggest here at the top.
  if(UpdateToken)
    {
      //Sound bell
      DisplayBlankToken();
      delay(100);
      /*
      digitalWrite(Bell,HIGH);
      delay(100);
      digitalWrite(Bell,LOW);
      delay(100);
      */
      //DisplayMarquee();
      DisplayToken();
      delay(250);
      SpeakToken();
      UpdateToken = false;
      for(int i = NUMBER_OF_DIGITS; i >= 0; i--)
        TokenNumber[i] = 0;
    }
  serialEvent();
}

void DisplayMarquee()
{
   dmd.clearScreen( true );
   dmd.selectFont(System5x7);
   for (byte x=0;x<DISPLAYS_ACROSS;x++) {
     for (byte y=0;y<DISPLAYS_DOWN;y++) {
       dmd.drawString(  2+(32*x),  1+(16*y), "Token", 5, GRAPHICS_NORMAL );
       dmd.drawString(  2+(32*x),  9+(16*y), "Number", 6, GRAPHICS_NORMAL );
     }
   }
   delay( 2000 );
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
}

void DisplayBlankToken()
{
  dmd.clearScreen( true );
}

void DisplayToken()
{
   for(int i = 3; i > 0; i--)
      {
        if(TokenNumber[i] == 10 || TokenNumber[i] == 0)
          TokenNumber[i] = -48;
        else
          break;
      }
   dmd.selectFont(angka_besar_kuru);
   dmd.clearScreen( true );
   dmd.drawChar(  0,  0, char(TokenNumber[3]+48), GRAPHICS_NORMAL );
   dmd.drawChar(  11,  0, char(TokenNumber[2]+48), GRAPHICS_NORMAL );
   dmd.drawChar( 22,  0, char(TokenNumber[1]+48), GRAPHICS_NORMAL );
}

void SpeakToken()
{
  if(TokenNumber[1] == -48)
    return;
    
  Sequence.RemoveAllPlayItems();            // Clear out any previous playlist
  
  Sequence.AddPlayItem(&dingDong);
  Sequence.AddPlayItem(&tokenNo);
  
  for(int i = 3; i > 0; i--)
    {
      if(TokenNumber[i] != 10)
        AddNumberToSequence(TokenNumber[i]);
    }
  DacAudio.Play(&Sequence);                 // Play the sequence, will not wait here to complete, works independently of your code
}

void displayTest()
  {
    for(int i = 1; i <= 3; i++)
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
