//We always have to include the library
#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(12,11,10,1);

/* we always wait a bit between updates of the display */
unsigned long delaytime=250;

void setup() {
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
}


/*
 This method will display the characters for the
 word "Arduino" one after the other on digit 0. 
 */
void writeArduinoOn7Segment() {
  lc.setChar(0,0,'a',false);
  delay(delaytime);
  lc.setRow(0,0,0x05);
  delay(delaytime);
  lc.setChar(0,0,'d',false);
  delay(delaytime);
  lc.setRow(0,0,0x1c);
  delay(delaytime);
  lc.setRow(0,0,B00010000);
  delay(delaytime);
  lc.setRow(0,0,0x15);
  delay(delaytime);
  lc.setRow(0,0,0x1D);
  delay(delaytime);
  lc.clearDisplay(0);
  delay(delaytime);
} 

/*
  This method will scroll all the hexa-decimal
 numbers and letters on the display. You will need at least
 four 7-Segment digits. otherwise it won't really look that good.
 */
void scrollDigits() {
  for(int i=0;i<13;i++) {
    lc.setDigit(0,3,i,false);
    lc.setDigit(0,2,i+1,false);
    lc.setDigit(0,1,i+2,false);
    lc.setDigit(0,0,i+3,false);
    delay(delaytime);
  }
  lc.clearDisplay(0);
  delay(delaytime);
}

void loop() { 
  //writeArduinoOn7Segment();
  //scrollDigits();
  Monday();
  delay(1000);
  Tuesday();
  delay(1000);
  Wednesday();
  delay(1000);
  Thursday();
  delay(1000);
  Friday();
  delay(1000);
  Saturday();
  delay(1000);
  Sunday();
  delay(1000);
}

void Monday()
{
  lc.clearDisplay(0);
  lc.setRow(0,5,0x66);
  lc.setRow(0,4,0x72);
  lc.setRow(0,3,0x1D);
  lc.setRow(0,2,0x15);
}

void Tuesday()
{
  lc.clearDisplay(0);
  lc.setRow(0,5,0x70);
  lc.setRow(0,4,0x46);
  lc.setRow(0,3,0x3e);
  lc.setRow(0,2,0x4f);
}

void Wednesday()
{
  lc.clearDisplay(0);
  lc.setRow(0,5,0x1e);
  lc.setRow(0,4,0x3c);
  lc.setRow(0,3,0x4f);
  lc.setRow(0,2,0x7e);
}

void Thursday()
{
  lc.clearDisplay(0);
  lc.setRow(0,5,0x70);
  lc.setRow(0,4,0x46);
  lc.setRow(0,3,0x37);
  lc.setRow(0,2,0x3e);
}

void Friday()
{
  lc.clearDisplay(0);
  lc.setRow(0,4,0x47);
  lc.setRow(0,3,0x05);
  lc.setRow(0,2,0x04);
}

void Saturday()
{
  lc.clearDisplay(0);
  lc.setRow(0,5,0x5b);
  lc.setRow(0,4,0x77);
  lc.setRow(0,3,0x70);
  lc.setRow(0,2,0x46);
}

void Sunday()
{
  lc.clearDisplay(0);
  lc.setRow(0,4,0x5b);
  lc.setRow(0,3,0x1c);
  lc.setRow(0,2,0x15);
}
