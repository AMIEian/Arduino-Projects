const int DtatOn = 3;
const int Pulses = 4;
const int LevelVoltage = A0;	//Arduino PIN 14
const int VCC = A4;		//Arduino PIN 18
float AnalogReading;
int Level;
void setup()
{
	Serial.begin(9600);
	pinMode(DtatOn,OUTPUT);
	pinMode(Pulses,OUTPUT);
	digitalWrite(DtatOn,LOW);
	digitalWrite(Pulses,LOW);
}

void loop()
{
	delay(10000);	//10 sec delay for every cycle
	pinMode(VCC,OUTPUT);
	pinMode(LevelVoltage,INPUT);
	//Turn on 5 volt
	digitalWrite(VCC,HIGH);
	delay(1000);
	//Take analog reading to detect level voltage
	AnalogReading = 0;
	for(int readings = 0; readings < 5; readings++)
		{
			AnalogReading = AnalogReading + analogRead(LevelVoltage);
			delay(50);
		}
	//Turn off and reverse voltage for depolarization
	digitalWrite(VCC,LOW);
	pinMode(VCC,INPUT);
	pinMode(LevelVoltage,OUTPUT);
	digitalWrite(LevelVoltage,HIGH);
	delay(1300);
	digitalWrite(LevelVoltage,LOW);
	
	//Convert this reading to actual voltage
	AnalogReading = AnalogReading/1024;
        Serial.println(AnalogReading);
	
	//Find water level for corresponding voltage
	findWaterLevel(AnalogReading);
	Serial.print("Water Level = ");
	Serial.println(Level);
	
	//Send this level to main control unit
	sendData();
}

void findWaterLevel(float volts)
{
	Level = 0;
	if(volts >= 0.0 && volts < 0.99)
		Level = 1;
	else if(volts >= 0.99 && volts < 1.32)
		Level = 2;
	else if(volts >= 1.32 && volts < 1.69)
		Level = 3;
	else if(volts >= 1.69 && volts < 2.08)
		Level = 4;
	else if(volts >= 2.08 && volts < 2.50)
		Level = 5;
	else if(volts >= 2.50 && volts < 2.93)
		Level = 6;
	else if(volts >= 2.93 && volts < 3.37)
		Level = 7;
	else if(volts >= 3.37 && volts < 3.66)
		Level = 8;
	else if(volts >= 3.66 && volts < 4.29)
		Level = 9;
	else if(volts >= 4.29)
		Level = 10;
}

void sendData()
{
	digitalWrite(DtatOn,HIGH);
	delay(1000);
	for(int count = 0; count < Level; count++)
		{
			digitalWrite(Pulses,HIGH);
			delay(1000);
			digitalWrite(Pulses,LOW);
			delay(1000);
		}
	digitalWrite(DtatOn,LOW);
}
