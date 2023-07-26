/*
 * Test.ino
 *
 * Created: 18-Mar-2016 2:15:54 AM
 * Author: AMIEian
 */ 
 int LED = 13;
void setup()
{

	  /* add setup code here, setup code runs once when the processor starts */
      pinMode(LED,OUTPUT);
}

void loop()
{

	  /* add main program code here, this code starts again each time it ends */
      digitalWrite(LED,HIGH);
      delay(1000);
      digitalWrite(LED,LOW);
      delay(1000);
}
