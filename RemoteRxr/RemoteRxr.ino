const int Relay_1 = 2;
const int Relay_2 = 3;
const int Relay_3 = 4;
const int Relay_4 = 5;
const int Relay_5 = 6;
const int Relay_6 = 7;
const int Relay_7 = 8;
const int Relay_8 = 9;
const int Relay_9 = 10;
const int Relay_10 = 11;
const int Relay_11 = 12;
const int Relay_12 = A0;
const int Relay_13 = A1;
const int Relay_14 = A2;
const int Relay_15 = A3;
const int Relay_16 = A4;

void allPinsDown()
	{
		digitalWrite(Relay_1,HIGH);
		digitalWrite(Relay_2,HIGH);
		digitalWrite(Relay_3,HIGH);
		digitalWrite(Relay_4,HIGH);
		digitalWrite(Relay_5,HIGH);
		digitalWrite(Relay_6,HIGH);
		digitalWrite(Relay_7,HIGH);
		digitalWrite(Relay_8,HIGH);
		digitalWrite(Relay_9,HIGH);
		digitalWrite(Relay_10,HIGH);
		digitalWrite(Relay_11,HIGH);
		digitalWrite(Relay_12,HIGH);
		digitalWrite(Relay_13,HIGH);
		digitalWrite(Relay_14,HIGH);
		digitalWrite(Relay_15,HIGH);
		digitalWrite(Relay_16,HIGH);
	}

void setup()
	{
		pinMode(Relay_1,OUTPUT);
		pinMode(Relay_2,OUTPUT);
		pinMode(Relay_3,OUTPUT);
		pinMode(Relay_4,OUTPUT);
		pinMode(Relay_5,OUTPUT);
		pinMode(Relay_6,OUTPUT);
		pinMode(Relay_7,OUTPUT);
		pinMode(Relay_8,OUTPUT);
		pinMode(Relay_9,OUTPUT);
		pinMode(Relay_10,OUTPUT);
		pinMode(Relay_11,OUTPUT);
		pinMode(Relay_12,OUTPUT);
		pinMode(Relay_13,OUTPUT);
		pinMode(Relay_14,OUTPUT);
		pinMode(Relay_15,OUTPUT);
		pinMode(Relay_16,OUTPUT);
		
		allPinsDown();
		
		Serial.begin(9600);
	}
	
void loop()
	{
		
	}
	
void serialEvent() 
	{
		while (Serial.available()) 
			{
				// get the new byte:
				char inChar = (char)Serial.read();
				Serial.print(inChar);
				if (inChar == 'X') 
					{
						allPinsDown();
					}
				else if(inChar == 'A')
					{
						digitalWrite(Relay_1,LOW);
					}
				else if(inChar == 'B')
					{
						digitalWrite(Relay_2,LOW);
					}
				else if(inChar == 'C')
					{
						digitalWrite(Relay_3,LOW);
					}
				else if(inChar == 'D')
					{
						digitalWrite(Relay_4,LOW);
					}
				else if(inChar == 'E')
					{
						digitalWrite(Relay_5,LOW);
					}
				else if(inChar == 'F')
					{
						digitalWrite(Relay_6,LOW);
					}
				else if(inChar == 'G')
					{
						digitalWrite(Relay_7,LOW);
					}
				else if(inChar == 'H')
					{
						digitalWrite(Relay_8,LOW);
					}
				else if(inChar == 'I')
					{
						digitalWrite(Relay_9,LOW);
					}
				else if(inChar == 'J')
					{
						digitalWrite(Relay_10,LOW);
					}
				else if(inChar == 'K')
					{
						digitalWrite(Relay_11,LOW);
					}
				else if(inChar == 'L')
					{
						digitalWrite(Relay_12,LOW);
					}
				else if(inChar == 'M')
					{
						digitalWrite(Relay_13,LOW);
					}
				else if(inChar == 'N')
					{
						digitalWrite(Relay_14,LOW);
					}
				else if(inChar == 'O')
					{
						digitalWrite(Relay_15,LOW);
					}
				else if(inChar == 'P')
					{
						digitalWrite(Relay_16,LOW);
					}
			}
	}
