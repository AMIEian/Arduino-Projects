/*
 * MobileRemote.ino
 *
 * Created: 21-Jun-2016 2:51:16 PM
 * Author: AMIEian
 */ 
#include <Arduino.h>
#include <Wire.h>
#include <sim800.h>
#include <WiFi.h>
#include <gprs.h>

#define UART_TX                     27
#define UART_RX                     26
#define SIMCARD_RST                 5
#define SIMCARD_PWKEY               4
#define SIM800_POWER_ON             23

#define UART_BANUD_RATE             9600

#define I2C_SDA                     21
#define I2C_SCL                     22

#define IP5306_ADDR                 0X75
#define IP5306_REG_SYS_CTL0         0x00

HardwareSerial hwSerial(1);
GPRS gprs(hwSerial, SIMCARD_PWKEY, SIMCARD_RST, SIM800_POWER_ON);

//PIN Alias
const int Comm_LED = 13;
const int Motor_LED = 2;
const int Phase_LED = 15;
const int Phase_1 = 32;
const int Phase_2 = 33;
const int Phase_3 = 25;
const int Start_Relay = 14;
const int Stop_Relay = 12;

//Controlling Variables
boolean Motor_Flag = false;
boolean LP_Flag = false;
boolean Power_Flag = false;
char phasechecktime = 0;

int numdata;
boolean started = false;
char smsbuffer[60];
//char n[15];

char sms_position;
char phone_number[15]; // array for the incoming phone number string
char primary_number [15] = "8208225379";
char secondary_number [15] = "9403585436";
boolean sms_received = false;
boolean call_received = false;
boolean Reply_Primary = false;

bool setPowerBoostKeepOn(int en)
{
    Wire.beginTransmission(IP5306_ADDR);
    Wire.write(IP5306_REG_SYS_CTL0);
    if (en)
        Wire.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
    else
        Wire.write(0x35); // 0x37 is default reg value
    return Wire.endTransmission() == 0;
}

void wifi_test()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("no networks found");
        return;
    }
    Serial.println("Found " + String(n) + " networks");
}

void setup()
	{
		pinMode(Phase_1, INPUT_PULLUP);
		pinMode(Phase_2, INPUT_PULLUP);
		pinMode(Phase_3, INPUT_PULLUP);
		pinMode(Start_Relay, OUTPUT);
		pinMode(Stop_Relay, OUTPUT);
		pinMode(Comm_LED, OUTPUT);
		pinMode(Motor_LED,OUTPUT);
    pinMode(Phase_LED,OUTPUT);	
		
		//Turn off both relays
		digitalWrite(Start_Relay, LOW);
		digitalWrite(Stop_Relay, LOW);
    digitalWrite(Comm_LED,LOW);
		digitalWrite(Motor_LED,LOW);
    digitalWrite(Phase_LED, LOW);
                
    Power_Flag = Power_Check();
                
		char i;
  
		//Serial connection.
		Serial.begin(9600);
  
		//----------------GET PRIMARY AND SECONDARY NUMBERS FROM EEPROM--------------------//
		//Reading Primary number from EEPROM
		if(EEPROM.read(0) == 'P')
			{
				for(i = 0; i < 15; i++)
					primary_number[i] = NULL;
				i = 2;
				while(EEPROM.read(i) != '#')
					{
						primary_number[i-2] = EEPROM.read(i);
						Serial.print(primary_number[i-2]);
						i++;
					}
				Serial.println();
			}
		//Reading Secondary number from EEPROM
		if(EEPROM.read(1) == 'S')
			{
				for(i = 0; i < 15; i++)
					secondary_number[i] = NULL;
				i = 21;
				while(EEPROM.read(i) != '#')
					{
						secondary_number[i-21] = EEPROM.read(i);
						Serial.print(secondary_number[i-21]);
						i++;
					}
				Serial.println();
			}
		//Read motor flag and on/off time from eeprom
		if(EEPROM.read(50) == 1)
      Motor_Flag = false;
			//Motor_Flag = true;
		else
			Motor_Flag = false;
  
		Wire.begin(I2C_SDA, I2C_SCL);
    bool   isOk = setPowerBoostKeepOn(1);
    String info = "IP5306 KeepOn " + String((isOk ? "PASS" : "FAIL"));
    Serial.println(info);

    hwSerial.begin(UART_BANUD_RATE, SERIAL_8N1, UART_RX, UART_TX);
    Serial.println("Setup Complete!");

    wifi_test();

    if (gprs.preInit()) {
        Serial.println("SIM800 Begin PASS");
    } else {
        Serial.println("SIM800 Begin FAIL");
    }

    Serial.println("Test motor ...");
    int i = 3;
    while (i--) {
        hwSerial.print("AT+SPWM=0,1000,1\r\n");
        delay(2000);
        hwSerial.print("AT+SPWM=0,0,0\r\n");
    }

    delay(200);

    if (0 != gprs.init()) {
        Serial.println("Not checked out to SIM card...");
        while (1);
    }

    // Switch audio channels
    hwSerial.print("AT+CHFA=1\r\n");
    delay(2);
    hwSerial.print("AT+CRSL=100\r\n");
    delay(2);
    hwSerial.print("AT+CLVL=100\r\n");
    delay(2);
    hwSerial.print("AT+CLIP=1\r\n");
    delay(2);
    Serial.println("Init success");
	}

void loop()
	{
		/*
		if(Motor_Flag)
			{
				//If motor flag is on check for phases every 15 sec
				phasechecktime++;
				if(phasechecktime >= 30)
					{
						phasechecktime = 0;
						if(Power_Check())
							{
								if( LP_Flag == false )
									{
										Motor_Start();
										LP_Flag = true;
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
									}
							}
					}
			}
    */
    delay(1000);
		Check_New_Call();
		Check_New_SMS();
    Power_Flag = Power_Check();
	}

void Check_New_SMS()
	{
		//-------------------------CHECK FOR NEW SMS-------------------------//
		sms_position=sms.IsSMSPresent(SMS_UNREAD);
		if (sms_position)
			{
				//Read new SMS
				sms.GetSMS(sms_position, phone_number, smsbuffer, 60);
				//Check whether SMS is received from authentic number
				if(Authentic_SMS())
					SMS_Parser();
				//Delete the SMS to free up memory
				sms.DeleteSMS(sms_position);
			}
	}

boolean Authentic_SMS()
	{
		String Phone_No = phone_number;
		String Primary_No = primary_number;
		String Secondary_No = secondary_number;
		
		//Check for master numbers. (Eagle number)
		if(Phone_No.indexOf("9819895913", 0) > -1)
			{
				Reply_Primary = true;
				return true;
			}
		
		//Check for primary or secondary numbers
		if(Phone_No.indexOf(Primary_No, 0) > -1)
			{
				Reply_Primary = true;
				return true;
			}
		else if(Phone_No.indexOf(Secondary_No, 0) > -1)
			{
				Reply_Primary = false;
				return true;
			}
		else
			return false;	
	}

void SMS_Parser()
	{
		String SMS_Data = smsbuffer;	
		String temp;
		String command;
		char length = 0;
		char count;
		String Message;
	
		//Get command string from SMS
		command = SMS_Data.substring(0,3);
		Serial.println(command);
	
		if(command == "PRI")
			{
				for(count = 0; count < 15; count++)
					primary_number[count] = NULL;
				SMS_Data.substring(SMS_Data.lastIndexOf('=')+1).toCharArray(primary_number,15,0);
				temp = primary_number;
				length = temp.length();
				count = 0;
				EEPROM.write(0,'P');
				for(count = 0; count < length; count++)
					EEPROM.write(count+2,temp.charAt(count));
				EEPROM.write(count+2,'#');	//End of phone number
				Message = F("PRI SET TO =");
				Message.concat(primary_number);
				Message.toCharArray(smsbuffer,60,0);
			
				if(Reply_Primary)
					sms.SendSMS(primary_number,smsbuffer);
				else
					sms.SendSMS(secondary_number,smsbuffer);
			}
		else if(command == "SEC")
			{		
				for(count = 0; count < 15; count++)
					secondary_number[count] = NULL;
				SMS_Data.substring(SMS_Data.lastIndexOf('=')+1).toCharArray(secondary_number,15,0);
				temp = secondary_number;
				length = temp.length();
				count = 0;
				EEPROM.write(1,'S');
				for(count = 0; count < length; count++)
					EEPROM.write(count+21,temp.charAt(count));
				EEPROM.write(count+21,'#');	//End of phone number
				Message = F("SEC SET TO =");
				Message.concat(secondary_number);
				Message.toCharArray(smsbuffer,60,0);
			
				if(Reply_Primary)
					sms.SendSMS(primary_number,smsbuffer);
				else
					sms.SendSMS(secondary_number,smsbuffer);
			}
		else if(command == "STA")
			{			
				sms_received = true;
				Motor_On();
			}
		else if(command == "STO")
			{
				sms_received = true;
				Motor_Off();
			}
			
		else if(command == "GET")
			{
				//CREAT SMS WITH ALL NEEDED INFORMATION
				Message = F("MOTOR IS ");
				if(Motor_Flag)
					Message.concat(F("ON"));
				else
					Message.concat(F("OFF"));
			
				if(Power_Check())
					Message.concat(F(", POWER OK"));
				else
					Message.concat(F(", POWER FAIL"));
				
			        
				Message.concat(F(", PRI="));
				Message.concat(primary_number);
				Message.concat(F(", SEC="));
				Message.concat(secondary_number);
			
				Message.toCharArray(smsbuffer,60,0);
			
				if(Reply_Primary)
					sms.SendSMS(primary_number,smsbuffer);
				else
					sms.SendSMS(secondary_number,smsbuffer);
			}	
		else
			{
				Message = F("INVALID KEYWORD ...!");
				Message.toCharArray(smsbuffer,60,0);
			
				if(Reply_Primary)
					sms.SendSMS(primary_number,smsbuffer);
				else
					sms.SendSMS(secondary_number,smsbuffer);
			}
	}

void Check_New_Call()
	{
		//--------------------CHECK FOR INCOMING CALL-----------------------------//
		if(in_call.CallStatus() == CALL_INCOM_VOICE)
			{
				if(gsm.checkCall(primary_number, 'N'))
					{
						Reply_Primary = true;
						call_received = true;
						if(Motor_Flag)
							Motor_Off();	//Lock the car
						else
							Motor_On();	//Unlock the
					}
				else if(gsm.checkCall(secondary_number, 'N'))
					{
						Reply_Primary = false;
						call_received = true;
						if(Motor_Flag)
							Motor_Off();	//Lock the car
						else
							Motor_On();	//Unlock the
					}
				else
					gsm.cutCall();	//Cut the call
			}
	}

void Motor_On()
	{
		String Message;
		if(Power_Check())
			{
				delay(5000);
				gsm.cutCall();
				Motor_Start();
				Motor_Flag = true;
				LP_Flag = true;
				if(call_received)
					{
						delay(30000);
						if(Reply_Primary)
							gsm.call(primary_number, 25000);
						else
							gsm.call(secondary_number, 25000);
						call_received = false;
						Reply_Primary = false;
					}
				if(sms_received)
					{
						Message = F("MOTOR STARTED...!");
						Message.toCharArray(smsbuffer,60,0);
						if(Reply_Primary)
							sms.SendSMS(primary_number, smsbuffer);
						else
							sms.SendSMS(secondary_number, smsbuffer);
						sms_received = false;
						Reply_Primary = false;
					}
				EEPROM.write(50,1);
			}
		else
			{
				Serial.println(F("POWER FAIL"));
				if(call_received)
					{
						delay(20000);
						gsm.cutCall();
						call_received = false;
						Reply_Primary = false;
					}
				if(sms_received)
					{
						Message = F("POWER NOT AVAILABLE ...!");
						Message.toCharArray(smsbuffer,60,0);
						if(Reply_Primary)
							sms.SendSMS(primary_number, smsbuffer);
						else
							sms.SendSMS(secondary_number, smsbuffer);
						sms_received = false;
						Reply_Primary = false;
					}
			}
	}

void Motor_Off()
	{
		String Message;
		delay(15000);
		gsm.cutCall();
		Motor_Stop();
		Motor_Flag = false;
		LP_Flag = false;
		delay(10000);
		if(call_received)
			{
				if(Reply_Primary)
					gsm.call(primary_number, 25000);
				else
					gsm.call(secondary_number, 25000);
				call_received = false;
				Reply_Primary = false;
			}
		if(sms_received)
			{
				Message = F("MOTOR STOPED...!");
				Message.toCharArray(smsbuffer,60,0);
				if(Reply_Primary)
					sms.SendSMS(primary_number, smsbuffer);
				else
					sms.SendSMS(secondary_number, smsbuffer);
				sms_received = false;
				Reply_Primary = false;
			}
		EEPROM.write(50,0);
	}

void Motor_Start()
	{
		digitalWrite(Start_Relay,HIGH);
		delay(8000);	//10 Sec delay
		digitalWrite(Start_Relay,LOW);
		delay(1000);
		Serial.println(F("MOTOR ON"));
		digitalWrite(Motor_LED,HIGH);	
	}

void Motor_Stop()
	{
		digitalWrite(Stop_Relay, HIGH);
		delay(8000);	//10 Sec delay
		digitalWrite(Stop_Relay, LOW);
		delay(1000);
		Serial.println(F("MOTOR OFF"));
		digitalWrite(Motor_LED,LOW);
	}

boolean Power_Check()
	{
		//-------------------POWER CHECK-----------------------//
		if(analogRead(Phase_1) >= 900 || analogRead(Phase_2) >= 900 || analogRead(Phase_3) >= 900)
      {
        digitalWrite(Phase_LED, LOW);
        return false; 
      }
		else
			{
        digitalWrite(Phase_LED, HIGH);
        return true; 
      }
	}
