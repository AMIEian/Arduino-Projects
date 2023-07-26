#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "SIM900.h"
#include "sms.h"
#include "call.h"

//PIN Alias
const int LED = 13;
const int Security_LED = 11;
const int PIR_LED = 11;
const int VT_Interrupt = 0;   //Pin number 2

//Controlling Variables
boolean Security_Flag = false;
boolean Alert_Flag = false;
static unsigned long last_interrupt_time = 0;
//int interrupt_count = 0;

//GSM Controlling Variables
SMSGSM sms;
CallGSM in_call;

int numdata;
boolean started = false;
char smsbuffer[60];
//char n[15];

char sms_position;
char phone_number[15]; // array for the incoming phone number string
char primary_number [15] = "+918208225379";
char secondary_number [15] = "+917972084065";
boolean sms_received = false;
boolean call_received = false;
boolean Reply_Primary = false;

void setup()
  {
    pinMode(LED, OUTPUT);
    pinMode(Security_LED,OUTPUT);
    pinMode(PIR_LED,OUTPUT);
    
    digitalWrite(Security_LED,LOW);
    digitalWrite(PIR_LED,LOW);
    char i;
  
    //LED blinking
    for(i = 0; i <= 30; i++)
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
      
    //----------------GET SECURITY FLAG FROM EEPROM--------------------//  
    if(EEPROM.read(50) == 1)
      {
        Security_Flag = true;
        digitalWrite(Security_LED,HIGH);
      }
    else
      {
        Security_Flag = false;
        digitalWrite(Security_LED,LOW);
      }
  
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
      
    attachInterrupt(VT_Interrupt,rxr_interrupt,RISING);
    
    Serial.println("Ready to get inputs...!");
  }

void loop()
  {
    
    // If no interrupt came within 20 seconds then reset security flag and interrupt counter
    unsigned long current_time = millis();
    if (current_time - last_interrupt_time > 20000) 
      {
         //interrupt_count = 0;
         Alert_Flag = false;
      }
    
    if(Alert_Flag)
      {
         if(Security_Flag)
           {
              Serial.println("Security Triggered...!");
              digitalWrite(PIR_LED,HIGH);
              delay(1000);
              digitalWrite(PIR_LED,LOW);
              Give_Alert();
           } 
         else
           Alert_Flag = false;
      }
      
    delay(1000);
    Check_New_Call();
    Check_New_SMS();
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
        EEPROM.write(count+2,'#');  //End of phone number
        Message = F("PRI SET TO = ");
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
        EEPROM.write(count+21,'#'); //End of phone number
        Message = F("SEC SET TO = ");
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
        Security_On();
      }
    else if(command == "STO")
      {
        sms_received = true;
        Security_Off();
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
            if(Security_Flag)
              Security_Off();  //Turn on Security
            else
              Security_On(); //Turn off Security
          }
        else if(gsm.checkCall(secondary_number, 'N'))
          {
            Reply_Primary = false;
            call_received = true;
            if(Security_Flag)
              Security_Off();  //Turn on Security
            else
              Security_On(); //Turn off Security
          }
        else
          gsm.cutCall();  //Cut the call
      }
  }

void Security_On()
  {
    String Message;
    
    delay(2000);
    gsm.cutCall();
    Security_Flag = true;
    Serial.println("Security is turned on...!");
    if(call_received)
      {
        delay(15000);
        if(Reply_Primary)
          gsm.call(primary_number, 25000);
        else
          gsm.call(secondary_number, 25000);
        call_received = false;
        Reply_Primary = false;
      }
    if(sms_received)
      {
        Message = F("SECURITY STARTED...!");
        Message.toCharArray(smsbuffer,60,0);
        if(Reply_Primary)
          sms.SendSMS(primary_number, smsbuffer);
        else
          sms.SendSMS(secondary_number, smsbuffer);
        sms_received = false;
        Reply_Primary = false;
      }
    EEPROM.write(50,1);
    digitalWrite(Security_LED,HIGH);
  }

void Security_Off()
  {
    String Message;
    delay(8000);
    gsm.cutCall();
    Security_Flag = false;
    Serial.println("Security is turned off...!");
    delay(5000);
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
        Message = F("SECURITY STOPED...!");
        Message.toCharArray(smsbuffer,60,0);
        if(Reply_Primary)
          sms.SendSMS(primary_number, smsbuffer);
        else
          sms.SendSMS(secondary_number, smsbuffer);
        sms_received = false;
        Reply_Primary = false;
      }
    EEPROM.write(50,0);
    digitalWrite(Security_LED,LOW);
  }

void Give_Alert()
  {
    gsm.call(primary_number, 25000);
    delay(1000);
    gsm.call(secondary_number, 25000);
    delay(1000);
  }

void rxr_interrupt()
{
    unsigned long interrupt_time = millis();
    // If interrupts come faster than 1 sec, assume it's a bounce and ignore
    if (interrupt_time - last_interrupt_time > 1000) 
      {
        last_interrupt_time = interrupt_time;
        Alert_Flag = true;
        /*
        if(interrupt_count == 0)
          {
            interrupt_count++;
          }
        else
          {
            Alert_Flag = true;
          }
        */  
      }
}  
