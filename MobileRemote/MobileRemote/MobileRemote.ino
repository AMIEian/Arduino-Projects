/*
 * MobileRemote.ino
 *
 * Created: 21-Jun-2016 2:51:16 PM
 * Author: AMIEian
 */ 

#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <SIM900.h>
#include <sms.h>
#include <call.h>

//PIN Alias
const int LED = 13;
const int Comm_LED = 12;
const int Motor_LED = 11;
const int Phase_LED = 10;
const int Phase_1 = A0;
const int Phase_2 = A1;
const int Phase_3 = A2;
const int Start_Relay = 8;
const int Stop_Relay = 7;

//Controlling Variables
boolean Motor_Flag = false;
boolean LP_Flag = false;
boolean Power_Flag = false;
char phasechecktime = 0;

//GSM Controlling Variables
SMSGSM sms;
CallGSM in_call;

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

void setup()
	{
		pinMode(LED, OUTPUT);
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
  
		//LED blinking
		for(i = 0; i <= 5; i++)
			{
				digitalWrite(LED,HIGH);
				delay(1000);
				digitalWrite(LED,LOW);
				delay(1000);
			}
  
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
  
		Serial.println(F("GSM testing."));
		//Start configuration of shield with baud rate.
		if (gsm.begin(9600))
			{
				Serial.println(F("READY"));
				started=true;
				digitalWrite(Comm_LED,HIGH);
			}
		else
			Serial.println(F("IDLE"));
		//If modem started correctly delete messages from SIM Card
		if(started)
			{
				for(i=1;i<=20;i++)
					sms.DeleteSMS(i);
				i = 0;
			}
	}

void loop()
	{
		if(gsm.SendATCmdWaitResp("AT", 1000, 1000, "OK", 2) != '1')
			digitalWrite(Comm_LED,HIGH);
		else
			digitalWrite(Comm_LED,LOW);
			
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
