/*
Actual IC pin connections
M_Clear = 1

RX = 2
TX = 3

UU = 4
UL = 5
LU = 6

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

SDA = 27
SCL = 28

ARDUINO / Program pin connections -

RX = 0
TX = 1

UU = 2
UL = 3
LU = 4

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

SDA = 18 / A4
SCL = 19 / A5
*/

#include <Wire.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "RTClib.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
#define Serial SerialUSB
#endif

//RTC and Bluetooth objects
RTC_DS1307 rtc;
LiquidCrystal lcd(12, 10, 5, 6, 7, 8);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

//PIN assignment
const int LED = 13;
const int Phase_1 = 11;
const int Phase_2 = 9;
const int Phase_3 = 17;  //A3
const int Start_Relay = 15;  //A1
const int Stop_Relay = 16;  //A2

//Controlling variables
int32_t ON_TIME = 1;
int32_t OFF_TIME = 1;
boolean Motor_Flag = true;
boolean Phase_Flag = false;
boolean Timer_Needed = true;
boolean Update_Time = false;
int Phase_Check_Interval = 15000;
int currentmillis = 0;
int preveousmillis = 0;
int displaymillis = 0;
int display_update = 1000;

byte correct[8] = {
    0x00, 0x01, 0x03, 0x16, 0x1c, 0x08, 0x00, 0x00
};

byte wrong[8] = {
    0x00, 0x1b, 0x0e, 0x04, 0x0e, 0x1b, 0x00, 0x00
};

DateTime now;
DateTime Motor_On_Time;
DateTime Motor_Off_Time;

void setup()
{
    //Configure Input / Output pins
    pinMode(LED, OUTPUT);
    pinMode(Phase_1, INPUT);
    pinMode(Phase_2, INPUT);
    pinMode(Phase_3, INPUT);
    pinMode(Start_Relay, OUTPUT);
    pinMode(Stop_Relay, OUTPUT);
    
    //Turn off both relays
    digitalWrite(Start_Relay, LOW);
    digitalWrite(Stop_Relay, LOW);
    
    //LED blinking
    for(char count = 0; count < 5; count ++)
        {
            digitalWrite(LED, HIGH);
            delay(1000);
            digitalWrite(LED, LOW);
            delay(1000);
        }
        
    lcd.begin(20, 4);
    // create a new character
    //lcd.createChar(0, correct);
    // create a new character
    //lcd.createChar(1, wrong);
    
    Serial.begin(9600);
    
    if (! rtc.begin()) {
        //lcd.clear();
        lcd.print("RTC NOT FOUND...");
        while (1);
    }

    if (! rtc.isrunning()) {
        //lcd.clear();
        lcd.print("RTC is NOT running!");
        delay(5000);
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }   
    inputString.reserve(20);
    
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
    else
        {
            ON_TIME = 10;   //On time in secs
            OFF_TIME = 60;  //Off time in secs
        }
        
        
    ON_TIME = ON_TIME * 60; //On time in secs
    Motor_On_Time = rtc.now() + TimeSpan(OFF_TIME);
    OFF_TIME = OFF_TIME * 60;   //Off time in secs
    Motor_Off_Time = rtc.now() + TimeSpan(OFF_TIME + ON_TIME);
}

void loop()
{
    currentmillis = millis();   //Get current millis
    if(currentmillis - displaymillis >= display_update)
        {
            displaymillis = currentmillis;
            display_screen();
        }
    
    // print the string when a newline arrives:
    if (stringComplete) {
        if(Update_Time)
            {
                int yr, mon, dy, hr, mm, ss;
                yr = inputString.substring(inputString.lastIndexOf('Y')+1, inputString.lastIndexOf('M')).toInt();
                mon = inputString.substring(inputString.lastIndexOf('M')+1, inputString.lastIndexOf('D')).toInt();
                dy = inputString.substring(inputString.lastIndexOf('D')+1, inputString.lastIndexOf('H')).toInt();
                hr = inputString.substring(inputString.lastIndexOf('H')+1, inputString.lastIndexOf('N')).toInt();
                mm = inputString.substring(inputString.lastIndexOf('N')+1, inputString.lastIndexOf('S')).toInt();
                ss = inputString.substring(inputString.lastIndexOf('S')+1, inputString.lastIndexOf('#')).toInt();
                rtc.adjust(DateTime(yr, mon, dy, hr, mm, ss));
                Update_Time = false;
            }
        else
            {
                ON_TIME = inputString.substring(0, inputString.lastIndexOf('$')).toInt();
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
                    
                ON_TIME = ON_TIME * 60; //On time in secs
                OFF_TIME = OFF_TIME * 60;   //Off time in secs
                
                Serial.println(ON_TIME);
                Serial.println(OFF_TIME);
                Serial.println(ON_TIME+OFF_TIME);
                Serial.print("OK");
                
                Motor_On_Time = rtc.now() + TimeSpan(OFF_TIME);
                Motor_Off_Time = rtc.now() + TimeSpan(OFF_TIME + ON_TIME);
            }
        // clear the string:
        inputString = "";
        stringComplete = false;
    }
    
    //Check whether time is equal to or greater than set on time if it is set the motor flag on
    if(compare_DateTime(rtc.now(),Motor_On_Time) >= 0 && compare_DateTime(rtc.now(),Motor_Off_Time) <= 0)
        {
            Motor_Flag = true;
            Phase_Check_Interval = 0;
        }
    else
        {
            if(Motor_Flag)
                {
                    //Turn off motor. Set new on and off time
                    //Motor_Stop();
                    digitalWrite(Start_Relay,LOW);
                    Phase_Flag = false;
                    Motor_Flag = false;
                    Motor_On_Time = rtc.now() + TimeSpan(OFF_TIME);
                    Motor_Off_Time = rtc.now() + TimeSpan(OFF_TIME + ON_TIME);
                }
        }   
    
    //If motor flag is on check phases and turn on the motor
    if(Motor_Flag)
        {
            if(currentmillis - preveousmillis >= Phase_Check_Interval)
                {
                    Phase_Check_Interval = 15000;
                    preveousmillis = currentmillis;
                    if(Phase_Check())
                        {
                            if(Phase_Flag == false)
                                {
                                    //Motor_Start();
                                    digitalWrite(Start_Relay,HIGH);
                                    Phase_Flag = true;
                                }
                        }
                    else
                        {
                            if(Phase_Flag == true)
                                {
                                    //Motor_Stop();
                                    digitalWrite(Start_Relay,LOW);
                                    Phase_Flag = false;
                                }
                        }
                }
        }
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
            
            stringComplete = true;
        }
        if(inChar == 'U')
            Update_Time = true;
    }
}

int compare_DateTime(DateTime time_1, DateTime time_2)
{
    if(time_1.year() == time_2.year())
        {
            if(time_1.month() == time_2.month())
                {
                    if(time_1.day() == time_2.day())
                        {
                            if(time_1.hour() == time_2.hour())
                                {
                                    if(time_1.minute() == time_2.minute())
                                        {
                                            if(time_1.second() == time_2.second())
                                                return 0;
                                            else
                                                return (time_1.second() - time_2.second());
                                        }
                                    else
                                        return (time_1.minute() - time_2.minute());
                                }
                            else
                                return (time_1.hour() - time_2.hour());
                        }
                    else
                        return (time_1.day() - time_2.day());
                }
            else
                return (time_1.month() - time_2.month());
        }
    else
        return (time_1.year() - time_2.year());
}

boolean Phase_Check()
{
    if(digitalRead(Phase_1) == HIGH && digitalRead(Phase_2) == HIGH && digitalRead(Phase_3) == HIGH)
        return true;
    else
        return true;
}

void Motor_Start()
{
    digitalWrite(Start_Relay,HIGH);
    delay(10000);   //10 Sec delay
    digitalWrite(Start_Relay,LOW);
    delay(1000);
}

void Motor_Stop()
{
    digitalWrite(Stop_Relay, HIGH);
    delay(10000);   //10 Sec delay
    digitalWrite(Stop_Relay, LOW);
    delay(1000);
}

void display_screen()
{
    lcd.clear();
    int hh, mm, ss;
    String temp;
    
    lcd.setCursor(0, 0);
    lcd.print("    EAGLE TIMER     ");
    
    lcd.setCursor(0, 1);
    temp.concat("TIME =      ");
    
    if(rtc.now().hour() < 10)
      temp.concat("0");
    temp.concat(rtc.now().hour());
    temp.concat(":");
    if(rtc.now().minute() < 10)
      temp.concat("0");
    temp.concat(rtc.now().minute());
    temp.concat(":");
    if(rtc.now().second() < 10)
      temp.concat("0");
    temp.concat(rtc.now().second());
    lcd.print(temp);
    
    lcd.setCursor(0, 2);
    temp = "";
    if(Motor_Flag)
        {
            temp.concat("OFF TIME =  ");
            if(Motor_Off_Time.hour() < 10)
              temp.concat("0");
            temp.concat(Motor_Off_Time.hour());
            temp.concat(":");
            if(Motor_Off_Time.minute() < 10)
              temp.concat("0");
            temp.concat(Motor_Off_Time.minute());
            temp.concat(":");
            if(Motor_Off_Time.second() < 10)
              temp.concat("0");
            temp.concat(Motor_Off_Time.second());
        }
    else
        {
            temp.concat("ON TIME =   ");
            if(Motor_On_Time.hour() < 10)
              temp.concat("0");
            temp.concat(Motor_On_Time.hour());
            temp.concat(":");
            if(Motor_On_Time.minute() < 10)
              temp.concat("0");
            temp.concat(Motor_On_Time.minute());
            temp.concat(":");
            if(Motor_On_Time.second() < 10)
              temp.concat("0");
            temp.concat(Motor_On_Time.second());
        }
    lcd.print(temp);
    /*
    lcd.setCursor(0, 2);
    temp = "";
    temp.concat("PHASES = 1.");
    if(digitalRead(Phase_1) == HIGH)
        temp.concat("K");
    else
        temp.concat("X");
    temp.concat(" 2.");
    if(digitalRead(Phase_2) == HIGH)
        temp.concat("K");
    else
        temp.concat("X");
    temp.concat(" 3.");
    if(digitalRead(Phase_3) == HIGH)
        temp.concat("K");
    else
        temp.concat("X");
    lcd.print(temp);
    */
    lcd.setCursor(0, 3);
    temp = "";
    temp.concat("MOTOR STATUS =   ");
    if(Motor_Flag)
        temp.concat(" ON");
    else
        temp.concat("OFF");
    lcd.print(temp);
}
