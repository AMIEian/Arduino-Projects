#include <Wire.h>
#include <EEPROM.h>
#include "RTClib.h"
#include "LedControl.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
#define Serial SerialUSB
#endif

RTC_DS1307 rtc;
LedControl lc=LedControl(12,11,10,1);

const int Buzzer = 2;
const int AlarmBuzzer = 3;
const int MinHrsData = 4;
const int MinHrsShift = 6;
const int MinHrsStore = 5;
const int SecData = 7;
const int SecShift = 9;
const int SecStore = 8;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

int secs, mins, hrs;
int dd, mm, yy;
boolean Update_Time = false;
boolean TwentyFourHrs = false;
boolean Update_Alarm = false;
boolean Alarm_On = false;

boolean AlarmBeeps = false;
int beepmillis;
int beeptime = 100;

int alarm_hrs = 0;
int alarm_mins = 0;
int daily_repeat = 0;
int beeps = 0;

int currentmillis = 0;
int preveousmillis = 0;
int displaymillis = 0;
int display_update = 1000;

DateTime CurrentTime;

char DisplayCodes[11] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00};
int TimeArray[6];

void setup()
{    
    pinMode(Buzzer,OUTPUT);
    pinMode(AlarmBuzzer,OUTPUT);
    pinMode(SecData,OUTPUT);
    pinMode(SecShift,OUTPUT);
    pinMode(SecStore,OUTPUT);
    pinMode(MinHrsData,OUTPUT);
    pinMode(MinHrsShift,OUTPUT);
    pinMode(MinHrsStore,OUTPUT);
    
    digitalWrite(Buzzer,LOW);
    digitalWrite(AlarmBuzzer,LOW);
    digitalWrite(SecData,LOW);
    digitalWrite(SecShift,LOW);
    digitalWrite(SecStore,LOW);
    digitalWrite(MinHrsData,LOW);
    digitalWrite(MinHrsShift,LOW);
    digitalWrite(MinHrsStore,LOW);
    
    delay(1000);
    
    Serial.begin(9600);
    
    if (! rtc.begin()) {
        Serial.println("RTC NOT FOUND...");
        while (1);
    }
    Serial.println("RTC FOUND...");
    if (! rtc.isrunning()) {
        Serial.println("RTC is NOT running!");
        delay(5000);
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }   
    Serial.println("RTC IS RUNNING ...");
    /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
    */
    lc.shutdown(0,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(0,8);
    /* and clear the display */
    lc.clearDisplay(0);
    
    inputString.reserve(20);
    
    if(EEPROM.read(15) == 1)
      {
        Alarm_On = true;
        EEPROM.get(0,alarm_hrs);
        EEPROM.get(5,alarm_mins);
        EEPROM.get(10,daily_repeat);
      }
    else
      Alarm_On = false;
      
    if(EEPROM.read(20) == 1)
      TwentyFourHrs = true;
    else
      TwentyFourHrs = false;
      
    CurrentTime = rtc.now();
    UpdateTimeArray();
    DisplaySecs();
    DisplayMinsHrs();
    DisplayDate();
}

void loop()
{
    currentmillis = millis();   //Get current millis
    if(AlarmBeeps)
        {
          if(currentmillis - beepmillis >= beeptime)
            {
              beepmillis = currentmillis;
              if(digitalRead(AlarmBuzzer) == HIGH)
                digitalWrite(AlarmBuzzer, LOW);
              else
                digitalWrite(AlarmBuzzer, HIGH);
            }
        }
    if(currentmillis - displaymillis >= display_update)
        {
            displaymillis = currentmillis;
            //One Sec Complete
            CurrentTime = rtc.now();
            UpdateTimeArray();
            DisplaySecs();
            if(secs == 0)
              {
                //One Min Complete
                DisplayMinsHrs();
                if(Alarm_On)
                  {
                    CheckAlarm();
                  }
                else
                  {
                    digitalWrite(AlarmBuzzer,LOW);
                    AlarmBeeps = false;
                  }
              }
            
            if(secs == 50)
               DisplayDay();
            if(secs == 0)
               DisplayDate();
        
            if(secs == 0 && mins == 0)
              beeps = hrs * 2;  //One Hour Complete 
            if(beeps > 0)
              {
                if(beeps % 2 == 0)
                  digitalWrite(Buzzer,HIGH);
                else
                  digitalWrite(Buzzer,LOW);
                beeps--;
              }
        }
    if (stringComplete) 
    {
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
                CurrentTime = rtc.now();
                UpdateTimeArray();
                DisplaySecs();
                DisplayMinsHrs();
                DisplayDate();
            }
        if(Update_Alarm)
            {
                alarm_hrs = inputString.substring(inputString.lastIndexOf('H')+1, inputString.lastIndexOf('M')).toInt();
                alarm_mins = inputString.substring(inputString.lastIndexOf('M')+1, inputString.lastIndexOf('R')).toInt();
                daily_repeat = inputString.substring(inputString.lastIndexOf('R')+1, inputString.lastIndexOf('#')).toInt();
                EEPROM.put(0,alarm_hrs);
                EEPROM.put(5,alarm_mins);
                EEPROM.put(10,daily_repeat);
                Alarm_On = true;
                EEPROM.write(15,1);
                Update_Alarm = false;
            }
        // clear the string:
        inputString = "";
        stringComplete = false;
    }
}

void UpdateTimeArray()
{
    secs = CurrentTime.second();
    mins = CurrentTime.minute();
    hrs = CurrentTime.hour();
 
    TimeArray[0] = secs % 10;
    TimeArray[1] = secs / 10;
    TimeArray[2] = mins % 10;
    TimeArray[3] = mins / 10;
    if(TwentyFourHrs == false)
      {
        if(hrs > 12)
          hrs = hrs - 12;
        if(hrs == 0)
          hrs = 12;
      }
    TimeArray[4] = hrs % 10;
    TimeArray[5] = hrs / 10;
    if(TwentyFourHrs == false && TimeArray[5] == 0)
      TimeArray[5] = 10;
}

void DisplaySecs()
{
    unsigned short Mask, bits, Flag, digit, number;
    for(digit = 0; digit < 2; digit++)
       {
          number = DisplayCodes[TimeArray[digit]];
          Mask = 0x01;
          for (bits=0; bits<8; bits++)
             {
                Flag = number & Mask;
                if(Flag == 0) 
                    digitalWrite(SecData,LOW);
                    //digitalWrite(SecData,HIGH);
                if(Flag > 0)
                    digitalWrite(SecData,HIGH);  
                    //digitalWrite(SecData,LOW); 
                delayMicroseconds(1);
                digitalWrite(SecShift,HIGH);
                delayMicroseconds(1);
                digitalWrite(SecShift,LOW);
                Mask = Mask << 1;
             }
       }
    delayMicroseconds(1);   
    digitalWrite(SecStore,HIGH);
    delayMicroseconds(1);
    digitalWrite(SecStore,LOW);
}

void DisplayMinsHrs()
{
    unsigned short Mask, bits, Flag, digit, number;
    for(digit = 2; digit < 6; digit++)
       {
          number = DisplayCodes[TimeArray[digit]];
          Mask = 0x01;
          for (bits=0; bits<8; bits++)
             {
                Flag = number & Mask;
                if(Flag == 0) 
                    digitalWrite(MinHrsData,LOW);
                if(Flag > 0)
                    digitalWrite(MinHrsData,HIGH);   
                delayMicroseconds(1);
                digitalWrite(MinHrsShift,HIGH);
                delayMicroseconds(1);
                digitalWrite(MinHrsShift,LOW);
                Mask = Mask << 1;
             }
       }
    delayMicroseconds(1);   
    digitalWrite(MinHrsStore,HIGH);
    delayMicroseconds(1);
    digitalWrite(MinHrsStore,LOW);
}

void DisplayDate()
{
    int digit;
    //CurrentTime = rtc.now();
    dd = CurrentTime.day();
    mm = CurrentTime.month();
    yy = CurrentTime.year();
    yy = yy % 1000;
    
    lc.clearDisplay(0);
    //Display Date
    digit = dd / 10;
    lc.setDigit(0,7,digit,false);
    digit = dd % 10;
    lc.setDigit(0,6,digit,false);
    lc.setRow(0,5,0x01);
    
    //Display Month
    digit = mm / 10;
    lc.setDigit(0,4,digit,false);
    digit = mm % 10;
    lc.setDigit(0,3,digit,false);
    lc.setRow(0,2,0x01);
    
    //Display Year
    digit = yy / 10;
    lc.setDigit(0,1,digit,false);
    digit = yy % 10;
    lc.setDigit(0,0,digit,false);
}

void DisplayDay()
{
    lc.clearDisplay(0);
    int dayofweek = CurrentTime.dayOfTheWeek();
    switch(dayofweek)
      {
        case 1:
          lc.setRow(0,5,0x66);
          lc.setRow(0,4,0x72);
          lc.setRow(0,3,0x1D);
          lc.setRow(0,2,0x15);
          //Serial.println("Monday");
        break;
        case 2:
          lc.setRow(0,5,0x70);
          lc.setRow(0,4,0x46);
          lc.setRow(0,3,0x3e);
          lc.setRow(0,2,0x4f);
          //Serial.println("Tuesday");
        break;
        case 3:
          lc.setRow(0,5,0x1e);
          lc.setRow(0,4,0x3c);
          lc.setRow(0,3,0x4f);
          lc.setRow(0,2,0x7e);
          //Serial.println("Wednesday");
        break;
        case 4:
          lc.setRow(0,5,0x70);
          lc.setRow(0,4,0x46);
          lc.setRow(0,3,0x37);
          lc.setRow(0,2,0x3e);
          //Serial.println("Thursday");
        break;
        case 5:
          lc.setRow(0,4,0x47);
          lc.setRow(0,3,0x05);
          lc.setRow(0,2,0x04);
          //Serial.println("Friday");
        break;
        case 6:
          lc.setRow(0,5,0x5b);
          lc.setRow(0,4,0x77);
          lc.setRow(0,3,0x70);
          lc.setRow(0,2,0x46);
          //Serial.println("Saturday");
        break;
        case 0:
          lc.setRow(0,4,0x5b);
          lc.setRow(0,3,0x1c);
          lc.setRow(0,2,0x15);
          //Serial.println("Sunday");
        break;
      }
}

void CheckAlarm()
{
    int HH, MM;
    CurrentTime = rtc.now();
    HH = CurrentTime.hour();
    MM = CurrentTime.minute();
    
    if(HH == alarm_hrs && MM == alarm_mins)
      {
        AlarmBeeps = true;
        if(daily_repeat == 0)
          {
            Alarm_On = false;
            EEPROM.write(15,0);
          }
      }
    else
      {
        digitalWrite(AlarmBuzzer,LOW);
        AlarmBeeps = false;
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
        if(inChar == 'A')
            Update_Alarm = true;
        if(inChar == 'T')
          {
            TwentyFourHrs = true;
            EEPROM.write(20,1);
            CurrentTime = rtc.now();
            UpdateTimeArray();
            DisplaySecs();
            DisplayMinsHrs();
            DisplayDate();
          }
        if(inChar == 'F')
          {
            TwentyFourHrs = false;
            EEPROM.write(20,0);
            CurrentTime = rtc.now();
            UpdateTimeArray();
            DisplaySecs();
            DisplayMinsHrs();
            DisplayDate();
          }
    }
}
