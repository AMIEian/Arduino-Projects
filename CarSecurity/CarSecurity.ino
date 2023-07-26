#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "Bounce2.h"
//#include "TinyGPS.h"
#include "SIM900.h"
#include "sms.h"
#include "call.h"

//TinyGPS gps;
SMSGSM sms;
CallGSM in_call;
SoftwareSerial ss(10,11);	// RX, TX
Bounce PanicBouncer = Bounce(); 
Bounce IgnitionBouncer = Bounce();

int numdata;
boolean started=false;
char smsbuffer[160];
char n[20];

char sms_position;
char phone_number[15]; // array for the phone number string
//char sms_text[25];
char primary_number [15] = "+917588093105";
char secondary_number [15];

//Controlling pins
const int Relay = 13;
//Accelerometer Connections
const int X_Motion = A0;
const int Y_Motion = A1;
const int Z_Motion = A3;
//Ignition Detection 
const int Ignition = 6;
const int Panic = 5;

//Controlling variables
boolean LOCK = false;
boolean Vib_SMS_Needed = false;
int Vib_Sensitivity = 20;
int Gforce_Percent = 66;
float MaxG = 1.99;
float flat = 0.0, flon = 0.0;
unsigned long age = 0.0;
String Latitude = "NA";
String Longitude = "NA";
boolean Reply_Primary = false;

void setup()
{
	//Configure relay as output pin and keep the relay off until no security issues
	pinMode(Relay,OUTPUT);
	digitalWrite(Relay,LOW);
	
	//Configure Accelerometer pins as input
	pinMode(X_Motion,INPUT);
	pinMode(Y_Motion,INPUT);
	pinMode(Z_Motion,INPUT);
	
	//Configure Ignition detection as input
	pinMode(Ignition, INPUT);
	//After setting up the button, setup the Bounce instance:
	IgnitionBouncer.attach(Ignition);
	IgnitionBouncer.interval(5);
	
	//Configure panic as input
	pinMode(Panic, INPUT);
	// After setting up the button, setup the Bounce instance :
	PanicBouncer.attach(Panic);
	PanicBouncer.interval(5); // interval in ms
	
	//Software Serial Communication for GPS
	ss.begin(9600);
	
	//Serial connection.
	Serial.begin(9600);
	
	//----------------GET PRIMARY AND SECONDARY NUMBERS FROM EEPROM--------------------//
	//Reading Primary and Secondary numbers from EEPROM
	char i;
	//char data = '+';
	if(EEPROM.read(0) == 'P')
		{
			//Serial.println("Primary Number - ");
			for(i = 0; i < 20; i++)
				primary_number[i] = NULL;
			i = 2;
			while(EEPROM.read(i) != '#')
				{
					primary_number[i-2] = EEPROM.read(i);
					Serial.print(primary_number[i-2]);
					i++;
				}	
			//Serial.println(" ");
		}
	if(EEPROM.read(1) == 'S')
		{
			//Serial.println("Secondary Number - ");
			for(i = 0; i < 20; i++)
				secondary_number[i] = NULL;
			i = 21;
			while(EEPROM.read(i) != '#')
				{
					secondary_number[i-21] = EEPROM.read(i);
					Serial.print(secondary_number[i-21]);
					i++;
				}
			//Serial.println(" ");	
		}
	
	//---------------------------GET LOCK STATUS FROM EEPROM-----------------------------//
	if(EEPROM.read(50) == 1)
		LOCK = true;
	else
		LOCK = false;
	
	//------------------------GET RELAY STATUS FROM EEPROM----------------------------//
	if(EEPROM.read(51) == 1)
		digitalWrite(Relay,HIGH);
	else
		digitalWrite(Relay,LOW);
		
	//---------------------GET VIBRATION SMS STATUS------------------------------//
	if(EEPROM.read(52) == 1)
		Vib_SMS_Needed = true;
	else
		Vib_SMS_Needed = false;
	
	//-------------------GET VIBRATION SENSITIVITY VALUE---------------------------//
	Vib_Sensitivity = (int)EEPROM.read(53);
		
	//-------------------GET G-FORCE PERCENT VALUE---------------------------------//
	Gforce_Percent = (int)EEPROM.read(54);
	
	//------------------CALCULATE G-FORCE VALUE FROM G-FORCE PERCENT----------------//
	MaxG = (Gforce_Percent/100)*3;
	
	Serial.println(F("GSM testing."));
	//Start configuration of shield with baud rate.
	if (gsm.begin(9600))
		{
			Serial.println(F("READY"));
			started=true;
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
	//CHECK IGNITION DETECTOR TO CHECK WHETHER CAR IS RUNNING OR NOT
	if(Vehicle_Running())
		{
			//If CAR IS RUNNING AND UNLOCKED ONLY GO FOR ACCIDENT DETECTION AND PANIC INPUT
			if(LOCK == false)
				{
					if(Accident_Detector())
						Critical_Alert(true);
					if(Panic_Detector())
						Critical_Alert(false);
				}
			else
				{
					//IF CAR IS RUNNING AND LOCKED SEND IGNITION ALERT AND WAIT FOR LOCK SMS
					Ignition_Alert();
					Check_New_SMS();
				}
		}
	else
		{
			//IF VEHICLE IS NOT RUNNING AND UNLOCKED WAIT FOR PANIC, SMS AND CALLS
			if(LOCK == false)
				{
					if(Panic_Detector())
						Critical_Alert(false);
					Check_New_SMS();
					Check_New_Call();
				}
			else
				{
					//IF VEHICLE IS NOT RUNNING AND LOCKED THEN CHECK FOR MOVEMENT, PANIC INPUT, NEW SMS AND NEW CALL
					if(Motion_Detector())
						Movement_Alert();
					if(Panic_Detector())
						Critical_Alert(false);
					Check_New_SMS();
					Check_New_Call();
				}
		}
}

void Check_New_SMS()
{
	//-------------------------CHECK FOR NEW SMS-------------------------//
	sms_position=sms.IsSMSPresent(SMS_UNREAD);
	if (sms_position)
		{
			//Read new SMS
			sms.GetSMS(sms_position, phone_number, smsbuffer, 160);
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
	Serial.println(F("-------------------------------------"));
	Serial.println(phone_number);
	Serial.println(primary_number);
	Serial.println(secondary_number);
	Serial.println(F("-------------------------------------"));
	/*
	//Check for master numbers. (Eagle numbers)
	if(Phone_No.indexOf("7588093105", 0) > -1 || Phone_No.indexOf("9819895913, 0") > -1)
		return true;
	*/
	//Check for primary or secondary numbers
	if(Phone_No.indexOf(Primary_No, 0) > -1)
		{
			Serial.println(F("SMS Received from primary..."));
			Reply_Primary = true;
			return true;
		}
	else if(Phone_No.indexOf(Secondary_No, 0) > -1)
		{
			Serial.println(F("SMS Received from secondary..."));
			Reply_Primary = false;
			return true;
		}
	else
		return false;	
}

void SMS_Parser()
{
	String SMS_Data = smsbuffer;	
	//Serial.println(SMS_Data);
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
			for(count = 0; count < 20; count++)
				primary_number[count] = NULL;
			SMS_Data.substring(SMS_Data.lastIndexOf('=')+1).toCharArray(primary_number,20,0);
			temp = primary_number;
			length = temp.length();
			count = 0;
			EEPROM.write(0,'P');
			for(count = 0; count < length; count++)
				EEPROM.write(count+2,temp.charAt(count));
			EEPROM.write(count+2,'#');	//End of phone number
			Message = F("PRIMARY NUMBER SET TO =");
			Message.concat(primary_number);
			Message.toCharArray(smsbuffer,160,0);
			
			if(Reply_Primary)
				sms.SendSMS(primary_number,smsbuffer);
			else
				sms.SendSMS(secondary_number,smsbuffer);
		}
	else if(command == "SEC")
		{		
			for(count = 0; count < 20; count++)
				secondary_number[count] = NULL;
			SMS_Data.substring(SMS_Data.lastIndexOf('=')+1).toCharArray(secondary_number,20,0);
			temp = secondary_number;
			length = temp.length();
			count = 0;
			EEPROM.write(1,'S');
			for(count = 0; count < length; count++)
				EEPROM.write(count+21,temp.charAt(count));
			EEPROM.write(count+21,'#');	//End of phone number
			Message = F("SECONDARY NUMBER SET TO =");
			Message.concat(secondary_number);
			Message.toCharArray(smsbuffer,160,0);
			
			if(Reply_Primary)
				sms.SendSMS(primary_number,smsbuffer);
			else
				sms.SendSMS(secondary_number,smsbuffer);
		}
	else if(command == "LOC")
		{			
			LOCK = true;
			EEPROM.write(50, 1);
			Message = F("VEHICLE IS LOCKED");
			Message.toCharArray(smsbuffer,160,0);
			
			if(Reply_Primary)
				sms.SendSMS(primary_number,smsbuffer);
			else
				sms.SendSMS(secondary_number,smsbuffer);
		}
	else if(command == "UNL")			
		{
			LOCK = false;
			EEPROM.write(50, 0);
			Message = F("VEHICLE IS UNLOCKED");
			Message.toCharArray(smsbuffer,160,0);
			
			if(Reply_Primary)
				sms.SendSMS(primary_number,smsbuffer);
			else
				sms.SendSMS(secondary_number,smsbuffer);
		}
	else if(command == "OFF")
		{
			//OFF the vehicle
			digitalWrite(Relay,HIGH);
			EEPROM.write(51,1);
			Message = F("VEHICLE IS TURNED OFF...");
			Message.toCharArray(smsbuffer,160,0);
			
			if(Reply_Primary)
				sms.SendSMS(primary_number,smsbuffer);
			else
				sms.SendSMS(secondary_number,smsbuffer);
		}
	else if(command == "SMS")
		{
			command = SMS_Data.substring(SMS_Data.lastIndexOf('=')+1);
			if(command == "Y")
				{
					Vib_SMS_Needed = true;
					EEPROM.write(52,1);	
					Message = F("SMS ALERT IS ON...");
					Message.toCharArray(smsbuffer,160,0);
					
					if(Reply_Primary)
						sms.SendSMS(primary_number,smsbuffer);
					else
						sms.SendSMS(secondary_number,smsbuffer);
				}
			else
				{
					Vib_SMS_Needed = false;
					EEPROM.write(52,0);
					Message = F("SMS ALERT IS OFF...");
					Message.toCharArray(smsbuffer,160,0);
					
					if(Reply_Primary)
						sms.SendSMS(primary_number,smsbuffer);
					else
						sms.SendSMS(secondary_number,smsbuffer);
				}	
		}
	else if(command == "VIB")
		{
			command = SMS_Data.substring(SMS_Data.lastIndexOf('=')+1);
			Vib_Sensitivity = command.toInt();
			EEPROM.write(53,(uint8_t)Vib_Sensitivity);
			Message = F("VIBRATION SENSITIVITY SET =");
			Message.concat(Vib_Sensitivity);
			Message.toCharArray(smsbuffer,160,0);
			
			if(Reply_Primary)
				sms.SendSMS(primary_number,smsbuffer);
			else
				sms.SendSMS(secondary_number,smsbuffer);
		}	
	else if(command == "ACC")
		{
			command = SMS_Data.substring(SMS_Data.lastIndexOf('=')+1);
			Gforce_Percent = command.toInt();
			MaxG = (Gforce_Percent/100)*3;
			EEPROM.write(54,(uint8_t)Gforce_Percent);
			Message = F("ACCIDENT G-FORCE PERCENTAGE =");
			Message.concat(Gforce_Percent);
			Message.toCharArray(smsbuffer,160,0);
			
			if(Reply_Primary)
				sms.SendSMS(primary_number,smsbuffer);
			else
				sms.SendSMS(secondary_number,smsbuffer);
		}
	else if(command == "GET")
		{
			//CREAT SMS WITH ALL NEEDED INFORMATION
			Message = F("STATUS=");
			if(LOCK)
				Message.concat("LOCKED");
			else
				Message.concat("UNLOCKED");
			Message.concat(", PRI=");
			Message.concat(primary_number);
			Message.concat(", SEC=");
			Message.concat(secondary_number);
			Message.concat(", SMS=");
			if(Vib_SMS_Needed)
				Message.concat("ON");
			else
				Message.concat("OFF");
			Message.concat(", VIB=");
			Message.concat(Vib_Sensitivity);
			Message.concat(", ACC=");
			Message.concat(Gforce_Percent);
			Message.toCharArray(smsbuffer,160,0);
			
			if(Reply_Primary)
				sms.SendSMS(primary_number,smsbuffer);
			else
				sms.SendSMS(secondary_number,smsbuffer);
		}
	else if(command == "MAP")
		{
			//Update_GPSData();
			Message = F("MAP= ");
			String Link = "http://maps.google.com/maps?q=" + Latitude +"," + Longitude + "&ll=" + Latitude + "," + Longitude + "&z=17";
			Message = Message + Link;
			Message.toCharArray(smsbuffer,160,0);
			
			if(Reply_Primary)
				sms.SendSMS(primary_number,smsbuffer);
			else
				sms.SendSMS(secondary_number,smsbuffer);
		}
	else		
		Serial.println(F("Invalid SMS"));
}

void Check_New_Call()
{
	//--------------------CHECK FOR INCOMING CALL-----------------------------//
	if(in_call.CallStatus() == CALL_INCOM_VOICE)
		{
			if(gsm.checkCall(primary_number))
				{
					//Serial.println("Primary Calling...");
			
					if(LOCK)
						{
							LOCK = false;	//Unlock the car
							delay(15000);	//15 Sec delay
							EEPROM.write(50,0);
						}
					else
						{
							LOCK = true;	//Lock the car
							delay(5000);	//5 Sec delay
							EEPROM.write(50,1);
						}
					//Give confirmation miss call
					gsm.call(primary_number,25000);
				}
			else if(gsm.checkCall(secondary_number))
				{
					//Serial.println("Secondary calling...");
			
					if(LOCK)
						{
							LOCK = false;	//Unlock the car
							delay(15000);	//15 Sec delay
							EEPROM.write(50, 0);
						}
					else
						{
							LOCK = true;	//Lock the car
							delay(5000);	//5 sec delay
							EEPROM.write(50, 1);
						}
					//Give confirmation miss call
					gsm.call(secondary_number,25000);
				}
			else
				gsm.cutCall();	//Cut the call
		}
}


void Movement_Alert()
{
	String Message;
	Message = F("Movement Detected...!");
	Message.toCharArray(smsbuffer,160,0);
	//Give callback and SMS
	if(Vib_SMS_Needed)
		sms.SendSMS(primary_number, smsbuffer);
	delay(5000);
	gsm.call(primary_number, 25000);
	delay(5000);
	if(Vib_SMS_Needed)
		sms.SendSMS(secondary_number, smsbuffer);
	delay(5000);
	gsm.call(secondary_number, 25000);
	delay(5000);
}

void Ignition_Alert()
{
	String Message;
	Message = F("Ignition Detected...!");
	Message.toCharArray(smsbuffer,160,0);
	//Give callback and SMS
	if(Vib_SMS_Needed)
		sms.SendSMS(primary_number, smsbuffer);
	delay(5000);
	gsm.call(primary_number, 25000);
	delay(5000);
	if(Vib_SMS_Needed)
		sms.SendSMS(secondary_number, smsbuffer);
	delay(5000);
	gsm.call(secondary_number, 25000);
	delay(5000);
}

void Critical_Alert(boolean Accident)
{
	String Message;
	//Update_GPSData();
	//Accident Occurred. Inform owner with message, call and send a link to gmap.
	if(Accident)
		Message = F("ACCIDENT OCCURRED. ");
	else
		Message = F("DRIVER IS IN PANIC. ");
	if(flat == 0.0)
		{
			Message.toCharArray(smsbuffer,160,0);
			sms.SendSMS(primary_number,smsbuffer);
			delay(1000);
			sms.SendSMS(secondary_number,smsbuffer);
			delay(1000);
			//Make call
			gsm.call(primary_number, 25000);
			delay(5000);
			gsm.call(secondary_number, 25000);
			delay(5000);
		}
	else
		{
			Message = Message + "CLICK LINK TO GET LOCATION- ";
			String Link = "http://maps.google.com/maps?q=" + Latitude +"," + Longitude + "&ll=" + Latitude + "," + Longitude + "&z=17";
			Message = Message + Link;
			Message.toCharArray(smsbuffer,160,0);
			sms.SendSMS(primary_number, smsbuffer);
			delay(1000);
			sms.SendSMS(secondary_number, smsbuffer);
			delay(1000);
			//Make call
			gsm.call(primary_number, 25000);
			delay(5000);
			gsm.call(secondary_number, 25000);
			delay(5000);
		}
	//Clear buffer for next message.
	char c;
	for(c = 0; c<160; c++)
		smsbuffer[c] = NULL;
}

boolean Accident_Detector()
{
	int X = 0;
	int Y = 0;
	int Z = 0;
	float zero_G = 512.0;
	float scale = 102.3; 
	float x_G = 0;
	float y_G = 0;
	float z_G = 0;
	
	//Get analog readings
	X = analogRead(X_Motion);
	delay(1);
	Y = analogRead(Y_Motion);
	delay(1);
	Z = analogRead(Z_Motion);
	
	//Check for accident
	x_G = ((float)X - zero_G)/scale;
	y_G = ((float)Y - zero_G)/scale;
	z_G = ((float)Z - zero_G)/scale;
	
	//If reading is negative, convert it to positive
	if(x_G < 0)
		x_G = x_G * -1;
	if(y_G < 0)
		y_G = y_G * -1;
	if(z_G < 0)
		z_G = z_G * -1;

	if(x_G > MaxG || y_G > MaxG || z_G > MaxG)
		return true;
	else
		return false;
}

boolean Panic_Detector()
{
	PanicBouncer.update();
	if(PanicBouncer.read())
		return true;
	else
		return false;
}

boolean Motion_Detector()
{
	int X_Diff = 0;
	int Y_Diff = 0;
	int Z_Diff = 0;
	int X = 0;
	int Y = 0;
	int Z = 0;
	char readings = 0;
	char counter = 0;
	boolean looper = true;
	
	while(looper && counter < 5)
		{
			//This loop will take 20 readings for 1 sec and gets accumulated result of each step
			for(readings = 0; readings < 20; readings++)
				{
					//Get analog readings
					X = analogRead(X_Motion);
					delay(1);
					Y = analogRead(Y_Motion);
					delay(1);
					Z = analogRead(Z_Motion);
					delay(50);
					X_Diff = X_Diff + (analogRead(X_Motion) - X);
					delay(1);
					Y_Diff = Y_Diff + (analogRead(Y_Motion) - Y);
					delay(1);
					Z_Diff = Z_Diff + (analogRead(Z_Motion) - Z);
					//Check for negative difference if it is convert it to positive.
					if(X_Diff < 0)
						X_Diff = X_Diff * -1;
					if(Y_Diff < 0)
						Y_Diff = Y_Diff * -1;
					if(Z_Diff < 0)
						Z_Diff = Z_Diff * -1;
				}
			if(X_Diff >= Vib_Sensitivity || Y_Diff >= Vib_Sensitivity || Z_Diff >= Vib_Sensitivity)
				{
					looper = true;
					delay(50);	//Delay for next reading
					counter++;
					X_Diff = 0;
					Y_Diff = 0;
					Z_Diff = 0;
				}
			else
				looper = false;
		}
	return looper;
}

boolean Vehicle_Running()
{
	IgnitionBouncer.update();
	if(IgnitionBouncer.read())
		return true;
	else
		return false;
}
/*
void Update_GPSData()
{
	boolean newData = false;
	//unsigned long chars;
	//unsigned short sentences, failed;
	char sz[32];

	// For one second we parse GPS data and report some key values
	for (unsigned long start = millis(); millis() - start < 1000;)
	{
		while (ss.available())
		{
			char c = ss.read();
			// Serial.write(c); // uncomment this line if you want to see the GPS data flowing
			if (gps.encode(c)) // Did a new valid sentence come in?
			newData = true;
		}
	}

	if (newData)
	{
		gps.f_get_position(&flat, &flon, &age);
		
		//Serial.print("LAT=");
		//Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
		dtostrf(flat,10,5,sz);
		Latitude = sz;
		
		//Serial.print(" LON=");
		//Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
		dtostrf(flon,10,5,sz);
		Longitude = sz;
		/*
		Serial.print(" SAT=");
		Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
		Serial.print(" PREC=");
		Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
		
	}
	/*
	gps.stats(&chars, &sentences, &failed);
	Serial.print(" CHARS=");
	Serial.print(chars);
	Serial.print(" SENTENCES=");
	Serial.print(sentences);
	Serial.print(" CSUM ERR=");
	Serial.println(failed);
	if (chars == 0)
		Serial.println("** No characters received from GPS: check wiring **");
	
}
*/
