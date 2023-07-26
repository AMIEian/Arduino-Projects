#include <PS2Keyboard.h>
#include <TimerOne.h>

const int DataPin = 8;
const int IRQpin =  3;
const int RangeRed = 4;
const int RangeGreen = 5;
const int RangeBlue = 6;
const int PowerRed = 9;
const int PowerGreen = 10;
const int PowerBlue = 11;
const int Buzzer = 12;
const int PowerInput = A0;

boolean keyPressed = false;
int charsCommunicated = 0;

PS2Keyboard keyboard;
void linkChecker()
	{
		if(keyPressed == false)
			{
				Serial.print('Z');
				updateComCounter(1);
			}
		digitalWrite(RangeGreen,LOW);
		digitalWrite(RangeRed,LOW);
		digitalWrite(RangeBlue,LOW);
		if(charsCommunicated <= 10)
			digitalWrite(RangeGreen,HIGH);
		else if(charsCommunicated > 10 && charsCommunicated <= 20)
			digitalWrite(RangeBlue,HIGH);
		else if(charsCommunicated > 20)
			digitalWrite(RangeRed,HIGH);
	}
void updateComCounter(int chars)
	{
		charsCommunicated = charsCommunicated + chars;
		if(charsCommunicated > 20)
			charsCommunicated = 21;
		if(charsCommunicated < 0)
			charsCommunicated = 0;
	}
void setup() {
	pinMode(RangeRed,OUTPUT);
	pinMode(RangeGreen,OUTPUT);
	pinMode(RangeBlue,OUTPUT);
	pinMode(PowerInput,INPUT);
	pinMode(PowerRed,OUTPUT);
	pinMode(PowerGreen,OUTPUT);
	pinMode(PowerBlue,OUTPUT);
	pinMode(Buzzer,OUTPUT);
	
	delay(1000);
	keyboard.begin(DataPin, IRQpin);
	Serial.begin(9600);
	
	//initialize TimerOne's interrupt/CPU
	Timer1.initialize( 10000 );           //period in microseconds to call .
	Timer1.attachInterrupt( linkChecker );   //attach the Timer1 interrupt to linkChecker function
}

void loop() {
	if (keyboard.available()) {
		
		// read the next key
		char c = keyboard.read();
		
		// check for command keys
		if (c == PS2_ENTER) {
			Serial.print('P');
			} else if (c == '0') {
			Serial.print('A');
			} else if (c == '1') {
			Serial.print('B');
			} else if (c == '2') {
			Serial.print('C');
			} else if (c == '3') {
			Serial.print('D');
			} else if (c == '4') {
			Serial.print('E');
			} else if (c == '5') {
			Serial.print('F');
			} else if (c == '6') {
			Serial.print('G');
			} else if (c == '7') {
			Serial.print('H');
			} else if (c == '8') {
			Serial.print('I');
			} else if (c == '9') {
			Serial.print('J');
			}else if (c == '.') {
			Serial.print('K');
			}else if (c == '+') {
			Serial.print('L');
			}else if (c == '-') {
			Serial.print('M');
			}else if (c == '*') {
			Serial.print('N');
			}else if (c == '/') {
			Serial.print('O');
			} else {
			// otherwise, just print all normal characters
			Serial.print(c);
		}
	}
}