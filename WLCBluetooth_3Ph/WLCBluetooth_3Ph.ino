/*
Actual IC pin connections
M_Clear = 1

RX = 2
TX = 3

VT = 4
UU = 5
UL = 6

VCC = 7
VDD = 8

CRYSTAL = 9
CRYSTAL = 10

LCD_D4 = 11
LCD_D5 = 12
LCD_D6 = 13
LCD_D7 = 14

LED = 15

LCD_E = 16

Phase_1 = 17

LCD_RS = 18

Phase_2 = 19

VCC = 20
AREF(VCC) = 21
GND = 22

LL = 23

Start_Relay = 24
Stop_Relay = 25

Phase_3 = 26

LU = 27
UN-USED = 28

ARDUINO / Program pin connections -

RX = 0
TX = 1

VT = 2

UU = 3
UL = 4

LCD_D4 = 5
LCD_D5 = 6
LCD_D6 = 7
LCD_D7 = 8

LED = 9

LCD_E = 10

Phase_1 = 11

LCD_RS = 12

Phase_2 = 13

LL = 14 / A0

Start_Relay = 15 / A1
Stop_Relay = 16 / A2

Phase_3 = 17 / A3

LU = 18 / A4

UN-USED = 19 / A5
*/

#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "Bounce2.h"

LiquidCrystal lcd(12, 10, 5, 6, 7, 8);

Bounce UUBouncer = Bounce();
Bounce ULBouncer = Bounce();

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

//PIN assignment
const int UU = 3;
const int UL = 4;
const int LU = 18;
const int LL = 14;
const int VT_Interrupt = 0;	//PIN 2
const int LED = 9;
const int Phase_1 = 11;
const int Phase_2 = 13;
const int Phase_3 = 17;  //A3
const int Start_Relay = 15;  //A1
const int Stop_Relay = 16;  //A2

//Controlling variables
int ON_TIME = 10;
int OFF_TIME = 50;
boolean Motor_Flag = false;
boolean LP_Flag = false;
boolean UU_Flag = false;
boolean UL_Flag = false;
boolean LU_Flag = false;
boolean LL_Flag = false;
boolean Level_Check = false;
boolean Power_Check = false;
boolean Timer_Needed = false;
int timer = 0;
int count = 0;
unsigned char txr_not_received = 0;
//int currentmillis = 0;
//int preveousmillis = 0;

byte correct[8] = {
	0b00000,
	0b00001,
	0b00011,
	0b10110,
	0b11100,
	0b01000,
	0b00000,
	0b00000
};

byte wrong[8] = {
	0b00000,
	0b11011,
	0b01110,
	0b00100,
	0b01110,
	0b11011, 
	0b00000,
	0b00000
};

void setup()
{
	//Configure Input / Output pins
	pinMode(UU, INPUT);
	pinMode(UL,INPUT);
	pinMode(LED, OUTPUT);
	pinMode(Phase_1, INPUT);
	pinMode(Phase_2, INPUT);
	pinMode(Phase_3, INPUT);
	pinMode(Start_Relay, OUTPUT);
	pinMode(Stop_Relay, OUTPUT);
	
	//Attach debouncer to pins
	UUBouncer.attach(UU);
	UUBouncer.interval(5);
	ULBouncer.attach(UL);
	ULBouncer.interval(5);
	
	//Turn off both relays
	digitalWrite(Start_Relay, LOW);
	digitalWrite(Stop_Relay, LOW);
	
	lcd.begin(20, 4);
	//LED blinking and Boot Screen
	boot_screen();
	// create a new character
	lcd.createChar(0, correct);
	// create a new character
	lcd.createChar(1, wrong);
	
	Serial.begin(9600);
	
	inputString.reserve(20);
	
	//Read motor flag and on/off time from eeprom
	if(EEPROM.read(0) == 1)
		Motor_Flag = true;
	else
		Motor_Flag = false;
		
	//Read Timer Needed flag from EEPROM
	if(EEPROM.read(1) == 1)
		Timer_Needed = true;
	else
		Timer_Needed = false;
	
	//If timer needed flag is set, read on and off time
	if(Timer_Needed)
		{
			uint8_t lower_byte, higher_byte;
			//Get On Time from EEPROM
			lower_byte = EEPROM.read(2);
			higher_byte = EEPROM.read(3);
			ON_TIME = higher_byte;
			ON_TIME = ON_TIME << 8;
			ON_TIME = ON_TIME + lower_byte;
			//Get Off Time from EEPROM
			lower_byte = EEPROM.read(4);
			higher_byte = EEPROM.read(5);
			OFF_TIME = higher_byte;
			OFF_TIME = OFF_TIME << 8;
			OFF_TIME = OFF_TIME + lower_byte; 
		}
	
	attachInterrupt(VT_Interrupt,RXReceived,RISING);
}

void loop()
{	
	// print the string when a newline arrives:
	if (stringComplete) {
		ON_TIME = inputString.substring(1, inputString.lastIndexOf('$')).toInt();
		OFF_TIME = inputString.substring(inputString.lastIndexOf('$')+1, inputString.lastIndexOf('#')).toInt();
		if(ON_TIME != 0 && OFF_TIME != 0)
				{
					Timer_Needed = true;
					//Save Timer Needed flag and On / Off time in EEPROM
					EEPROM.write(1,1);
					//Save ON TIME
					uint8_t higer_byte, lower_byte;
					lower_byte = ON_TIME & 0x00FF;
					higer_byte = ON_TIME >> 8;
					EEPROM.write(2,lower_byte);
					EEPROM.write(3,higer_byte);
					//Save OFF TIME
					lower_byte = OFF_TIME & 0x00FF;
					higer_byte = OFF_TIME >> 8;
					EEPROM.write(4,lower_byte);
					EEPROM.write(5,higer_byte);
				}
		else
			{
				Timer_Needed = false;
				EEPROM.write(1,0);
			}
			
		// clear the string:
		inputString = "";
		stringComplete = false;
		//Update the display
		update_display();
	}
	
	//---------------LEVEL CHECK-------------------------//
	//If motor is running
	if(Motor_Flag)
	{
		txr_not_received ++;
		
		if( UU_Flag == false && LL_Flag == true )
			Level_Check = true;
		else
			Level_Check = false;
	}
	//If motor is stoped
	else
	{
		if( (UU_Flag == false && UL_Flag == false) && (LU_Flag == true && LL_Flag == true) )
			Level_Check = true;
		else
			Level_Check = false;
	}
	
	//-------------------POWER CHECK-----------------------//
	if(digitalRead(Phase_1) == HIGH && digitalRead(Phase_2) == HIGH && digitalRead(Phase_3) == HIGH)
		Power_Check = true;
	else
		Power_Check = false;
		
	//-----------------START - STOP MOTOR ACCORDING TO LEVEL AND POWER CHECK-----------------------//
	if( Level_Check && Power_Check && txr_not_received <= 25 )
	{
		//Check level & phase flag if it is 0 then motor was off due to Power faillure or level failure Start it again
		if( LP_Flag == false )
		{
			Motor_Start();
			LP_Flag = true;
			Motor_Flag = true;
			EEPROM.write(0,1);
			if(Timer_Needed)
			{
				timer = 0;  //Reset timer
				count = 0;  //Reset 10 sec counter
			}
			update_display();
		}
	}
	//If level or Phase is not ok then stop the motor & set it's flag to 0
	else
	{
		//Level or Power is fail check that the motor was running or not if it was running stop it now
		if( LP_Flag )
		{
			Motor_Stop();
			LP_Flag = false;
			Motor_Flag = false;
			EEPROM.write(0,0);
			update_display();
		}
	}
	//Check timer while motor is on
	if(Timer_Needed && LP_Flag)
	{
		if(timer >= ON_TIME)
		{
			//Turn off the motor and remain here for off time
			Motor_Stop();
			LP_Flag = false;
			update_display();
			timer = 0;
			while(timer <= OFF_TIME)
			{
				for(count = 0; count < 6; count++)
				{
					delay(10000);  //10 SEC DELAY
					update_display();
				}
				//One minute completed
				timer++;
			}
		}
		else
		{
			count++;
			if(count >= 6)
			{
				timer++;
				count = 0;
			}
		}
	}
	
	update_display();
	delay(10000);  //10 SEC DELAY
}

void serialEvent() {
	while (Serial.available()) {
		// get the new byte:
		char inChar = (char)Serial.read();
		// add it to the inputString:
		inputString += inChar;
		// if the incoming character is a newline, set a flag
		// so the main loop can do something about it:
		if (inChar == '#') {
			inputString += '\n';
			stringComplete = true;
		}
	}
}

void RXReceived()
{
	if(txr_not_received < 2)
		{
			txr_not_received++;
			return;
		}
		
	txr_not_received = 2;
	
	//Check UL
	ULBouncer.update();
	if( ULBouncer.read() )
		UL_Flag = true;
	else
		UL_Flag = false;
	
	//Check UU
	UUBouncer.update();
	if(UUBouncer.read())
	{
		UU_Flag = true;
		UL_Flag = true;
	}
	//UU IS OUT OF WATER
	else
		UU_Flag = false;
	
	//Check LU
	pinMode(LU, INPUT);
	pinMode(LL,OUTPUT);
	digitalWrite(LL, HIGH);
	delay(1000);
	if(digitalRead(LU) == HIGH)
		LU_Flag = true;
	else
		LU_Flag = false;
		
	//Check LL
	pinMode(LL, INPUT);
	pinMode(LU,OUTPUT);
	digitalWrite(LU,HIGH);
	delay(1000);
	if(digitalRead(LL) == HIGH)
		LL_Flag = true;
	else
		LL_Flag = false;
	
	pinMode(LL,OUTPUT);
	digitalWrite(LL,LOW);
	digitalWrite(LU,LOW);
	update_display();
}

void Motor_Start()
{
	digitalWrite(Start_Relay,HIGH);
	delay(10000);	//10 Sec delay
	digitalWrite(Start_Relay,LOW);
	delay(1000);
}

void Motor_Stop()
{
	digitalWrite(Stop_Relay, HIGH);
	delay(10000);	//10 Sec delay
	digitalWrite(Stop_Relay, LOW);
	delay(1000);
}

void update_display()
{
	int elapsed_time;
	
	lcd.clear();
	
	//display_upper_tank()
	lcd.setCursor(0,0);
	lcd.print("UPPER TANK = L1");
	//Draw correct or wrong symbol
	if(UU_Flag)
		lcd.write(byte(0));
	else
		lcd.write(byte(1));
	lcd.print(" L2");
	if(UL_Flag)
		lcd.write(byte(0));
	else
		lcd.write(byte(1));
		
	//diplay_lower_tank()
	lcd.setCursor(0,1);
	lcd.print("LOWER TANK = L1");
	//Draw correct or wrong symbol
	if(LU_Flag)
		lcd.write(byte(0));
	else
		lcd.write(byte(1));
	lcd.print(" L2");
	if(LL_Flag)
		lcd.write(byte(0));
	else
		lcd.write(byte(1));
		
	//display_timer();
	lcd.setCursor(0,2);
	if(LP_Flag && Motor_Flag && Timer_Needed)
		{
			lcd.print("MOTOR OFF IN ");
			elapsed_time = OFF_TIME - timer;
			lcd.print(elapsed_time);
			lcd.print(" Min");
		}
	else if(LP_Flag == false && Motor_Flag && Timer_Needed)
		{
			lcd.print("MOTOR ON IN ");
			elapsed_time = ON_TIME - timer;
			lcd.print(elapsed_time);
			lcd.print(" Min");
		}
	else if(LP_Flag == false && Motor_Flag == false && Timer_Needed)
		{
			lcd.print("ON = ");
			lcd.print(ON_TIME);
			lcd.print(" M/");
			lcd.print("OFF = ");
			lcd.print(OFF_TIME);
			lcd.print(" M");
		}
	else
		lcd.print("TIMER OFF...!");
	
	//display_motor_status();
	lcd.setCursor(0,3);
	lcd.print("MOTOR ");
	if(LP_Flag)
		lcd.print("ON/");
	else
		lcd.print("OFF/");
	lcd.print("POWER ");
	if(Power_Check)
		lcd.print("OK");
	else
		lcd.print("FAIL");
}

void boot_screen()
{
	//Update Display
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("--------------------");
	
	lcd.setCursor(0,1);
	lcd.print("       EAGLE        ");
	
	lcd.setCursor(0,2);
	lcd.print("  CONTROL SYSTEMS   ");
	
	//MOTOR STATUS
	lcd.setCursor(0,3);
	lcd.print("--------------------");
	
	for(count = 0; count <= 4; count++)
	{
		digitalWrite(LED,HIGH);
		delay(1000);  //DELAY OF 1 SECOND
		digitalWrite(LED,LOW);
		delay(1000);  //DELAY OF 1 SECOND
	}
	count = 0;  //Reset count for reuse
	
	lcd.clear();
	
	delay(1000);  //1 SEC DELAY
	
	lcd.setCursor(0,0);
	lcd.print("--------------------");
	
	lcd.setCursor(0,1);
	lcd.print("   WIRELESS WATER   ");
	
	lcd.setCursor(0,2);
	lcd.print("  LEVEL CONTROLLER  ");
	
	//MOTOR STATUS
	lcd.setCursor(0,3);
	lcd.print("--------------------");
	
	delay(8000);  //1 SEC DELAY
	
	lcd.clear();
	
	delay(1000);  //1 SEC DELAY
}
