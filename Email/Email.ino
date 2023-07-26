/*

Email Shield Example

This example shows an application on 1Sheeld's email shield.

By using this example, you can send an email when the app's
Push button is pressed.

OPTIONAL:
To reduce the library compiled size and limit its memory usage, you
can specify which shields you want to include in your sketch by
defining CUSTOM_SETTINGS and the shields respective INCLUDE_ define. 

*/

#define CUSTOM_SETTINGS
#define INCLUDE_EMAIL_SHIELD

/* Include 1Sheeld library. */
#include <OneSheeld.h>

const int inp = 2;
boolean mailSent = false;

void setup() 
{
  pinMode(inp, INPUT_PULLUP);
  /* Start communication.*/
  OneSheeld.begin();
}

void loop()
{
	if(digitalRead(inp) == LOW && mailSent == false)
    {
      Camera.setFlash(ON);
      Camera.rearCapture();
      delay(10000);
      //SMS.send("7972084065","Button pressed!");
      Email.attachLastPicture("diasurg@gmail.com","Button pressed!","Hi, someone pressed the button!");
      //Email.attachLastPicture("nitin4july@gmail.com","Button pressed!","Hi, someone pressed the button!");
      mailSent = true;
    }
}

void onButtonStatusChanged(bool isPressed)
{
  /* Check the buttons status. */
  if(isPressed)
  {
  	/* Send an email. */
    Email.send("example@example.com","Button pressed!","Hi, someone pressed the button!");
  }
}
