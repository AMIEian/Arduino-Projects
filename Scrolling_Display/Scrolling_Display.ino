#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMD.h>        //
#include <TimerOne.h>   //
#include "SystemFont5x7.h"
#include "Arial_black_16.h"
#include <EEPROM.h>

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

String inputString = "";         // a string to hold incoming data
char message[400];
int messageLen = 0;
boolean stringComplete = false;  // whether the string is complete
int scrollSpeed = 30;

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void ScanDMD()
{ 
  dmd.scanDisplayBySPI();
}

/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void)
{
   Serial.begin(9600);

   inputString.reserve(400);
   
   //Check for previously stored message
   
   if(EEPROM.read(0) == 1)
	{
		int position = 1;
		char ch = EEPROM.read(position);
		while(ch != '$')
			{
				inputString += ch;
				position++;
				ch = EEPROM.read(position);
			}
	}
	else
		inputString = "30MSGNO MESSAGE TO DISPLAY";
		
   stringComplete = true;
   
   //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
   Timer1.initialize( 3000 );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
   Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()

   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( true );   //true is normal (all pixels off), false is negative (all pixels on)
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void)
{
   // print the string when a newline arrives:
   if (stringComplete) {
	   stringComplete = false;
	   scrollSpeed = inputString.substring(0, inputString.lastIndexOf("MSG")).toInt();
	   messageLen = inputString.substring((inputString.lastIndexOf("MSG")+2)).length();
	   inputString.substring((inputString.lastIndexOf("MSG")+2)).toCharArray(message, messageLen, 1);
       inputString = "";
   }
   dmd.clearScreen( true );
   dmd.selectFont(Arial_Black_16);
   const char *MSG = &message[0];
   dmd.drawMarquee(MSG,strlen(MSG),(32*DISPLAYS_ACROSS)-1,0);   
   long start=millis();
   long timer=start;
   boolean ret = false;
   while(!ret && Serial.available() <= 0){
     if ((timer+scrollSpeed) < millis()) {
       ret = dmd.stepMarquee(-1,0);
       timer=millis();
     }
   }
}

void serialEvent() {
	while (Serial.available()) {
		// get the new byte:
		char inChar = (char)Serial.read();
        // if the incoming character is a newline, set a flag
		// so the main loop can do something about it:
		if (inChar == '$') {
			stringComplete = true;
			EEPROM.write(0,1);
			int position = 1;
			for(int i = 0; i<inputString.length(); i++) {
				EEPROM.write(position,inputString.charAt(i));
				position++;
			}
			EEPROM.write(position,'$');
			return;
		}
                // add it to the inputString:
		inputString += inChar;
	}
}
