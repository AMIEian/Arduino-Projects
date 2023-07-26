/*
 * GSMTests.ino
 *
 * Created: 16-Mar-2016 2:05:30 PM
 * Author: AMIEian
 */ 

#include "SIM900.h"
#include <SoftwareSerial.h>
//If not used, is better to exclude the HTTP library,
//for RAM saving.
//If your sketch reboots itself proprably you have finished,
//your memory available.
//#include "inetGSM.h"

//If you want to use the Arduino functions to manage SMS, uncomment the lines below.
#include "sms.h"
#include "SIM900.h"

SMSGSM sms;
SIMCOM900 modem;

//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to send and receive SMS.

int numdata;
boolean started=false;
char smsbuffer[160];
char n[20];

//debug begin
char sms_position;
char phone_number[20] = "9819895913"; // array for the phone number string
char sms_text[100] = "Hi";
char authentic_no [20] = "+917588093105";

String Phone_No;
String SMS_Data;
String Serial_Data;

char inSerial[40];
int i=0;


//debug end


void setup()
{
    //Serial connection.
    Serial.begin(9600);
    Serial.println("GSM Shield testing.");
    //Start configuration of shield with baud rate.
    //For http uses is recommended to use 4800 or slower.
    if (gsm.begin(9600)) 
    {
        Serial.println("\nstatus=READY");
        Serial.println("NO NEW SMS,WAITTING");
        started=true;
    } 
    else 
        Serial.println("\nstatus=IDLE");

    if(started) 
    {
        //Enable this two lines if you want to send an SMS.
        //if (sms.SendSMS("3471234567", "Arduino SMS"))
        //Serial.println("\nSMS sent OK");

       //if NO SPACE ,you need delete SMS  from position 1 to position 20
       //please enable this four lines
       for(i=1;i<=20;i++)
       {
           sms.DeleteSMS(i);
       }
        i = 0;
        Serial.println("Ready to get calls...!");
    }

}

void loop()
{
    if(started) 
    {
        //Check call from valid number
        serialswread();
    }
}

void serialswread()
{
    //i = 0;
    gsm.SimpleRead();
}