#include "Bounce2.h"
//PIN Alias
const int analogPin = A0;   //The pin that the potentiometer is attached to
const int VCC = A4;		//Series of diode
const int Buzzer = 12;	//Buzzer
const int Mute = 13;	//Push button to mute buzzer
const int Mute_Indication = A1;	//Buzzer mute indication LED
int ledPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};   // An array of pin numbers to which LEDs are attached (Bar Graph)
const int ledCount = 10;    //The number of LEDs in the bar graph
float sensorReading = 0;
boolean buzzer_mute = false;
char halfSec = 0;
int beepTime;
int ledLevel;
//Debouncer for mute button
Bounce muteBouncer = Bounce();
void setup() 
	{
		//Loop over the pin array and set them all to output:
		for (int thisLed = 0; thisLed < ledCount; thisLed++) 
			{
				pinMode(ledPins[thisLed], OUTPUT);
			}
		pinMode(Buzzer,OUTPUT);
		pinMode(Mute,INPUT);
		pinMode(Mute_Indication,OUTPUT);
		muteBouncer.attach(Mute);
		muteBouncer.interval(10);
	}

void loop() 
	{
		delay(500);	//500 msec delay for every cycle
		halfSec++;
		if(halfSec == 20)
			{
				//10 sec completed. Check level.
				pinMode(VCC,OUTPUT);
				pinMode(analogPin,INPUT);
				//Turn on 5 volt
				digitalWrite(VCC,HIGH);
				delay(1000);
				//Read the potentiometer and get average
				sensorReading = 0;
				for(int readings = 0; readings < 5; readings++)
					{
						sensorReading = sensorReading + analogRead(analogPin);
						delay(50);
					}
				//Find Average to get final result
				sensorReading = (int)sensorReading/5;
				//Map the result to a range from 0 to the number of LEDs:
				ledLevel = map(sensorReading, 100, 900, 0, ledCount);
				// loop over the LED array:
				for (int thisLed = 0; thisLed < ledCount; thisLed++)
					{
						//If the array element's index is less than ledLevel, turn the pin for this element on:
						if (thisLed < ledLevel)
							{
								digitalWrite(ledPins[thisLed], HIGH);
							}
						//Turn off all pins higher than the ledLevel:
						else
							{
								digitalWrite(ledPins[thisLed], LOW);
							}
					}
				//Turn off and reverse voltage for depolarization
				digitalWrite(VCC,LOW);
				pinMode(VCC,INPUT);
				pinMode(analogPin,OUTPUT);
				digitalWrite(analogPin,HIGH);
				delay(1300);
				digitalWrite(analogPin,LOW);
			}
		if(halfSec == 60)
			{
				if(ledLevel > 8 || ledLevel < 2)
					{
						if(buzzer_mute)
							{
								digitalWrite(Buzzer,HIGH);
								delay(1000);
								digitalWrite(Buzzer,LOW);
								delay(1000);
							}
					}
			}
		if(halfSec == 120)
			{
				//Reset half sec counter
				halfSec = 0;
				if(ledLevel >= 8 || ledLevel <= 2)
					{
						if(buzzer_mute)
							{
								digitalWrite(Buzzer,HIGH);
								delay(1000);
								digitalWrite(Buzzer,LOW);
								delay(1000);
							}
					}	
			}
		//Get input from mute button 
		muteBouncer.update();
		if(muteBouncer.read())
			{
				buzzer_mute = !buzzer_mute;
				if(buzzer_mute)
					digitalWrite(Mute_Indication,HIGH);
				else
					digitalWrite(Mute_Indication,LOW);
				while(muteBouncer.read())
					{
						//Remain here until button is released
					}
			}
	}