#include <Nextion.h>
#include <PID_v1.h>
#include <PID_AutoTune_v0.h>
#include <Preferences.h>
#include <Wire.h>
#include "RTClib.h"
#include "Settimino.h"
#include "variables.h"
#include "display_strings.h"
#include "BluetoothSerial.h"

#define nextion Serial
Nextion myNextion(nextion, 9600);

//COMMUNICATION VARIABLES
int16_t inputs[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int16_t outputs[25] = {0, 1, 0, 50, 0, 0, 0, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0, 5, 0, 0, 0};

int16_t inputTemperature;
int16_t inputHumidity;

int screenCounter = 0;

//AHU Settings ARRAYS
int16_t AHUSetting[20];

//ANALOG INPUTS ARRAYS - Return Air Temperature
int16_t RATemp[4];
int16_t RATempPID[8];

//ANALOG INPUTS ARRAYS - Return Air Humidity
int16_t RAHum[4];
int16_t RAHumPID[8];

//ANALOG INPUTS ARRAYS - VFD
int16_t VFD[4];
int16_t VFDPID[8];

//ANALOG INPUTS ARRAYS - Air Velocity Sensor
int16_t AVS[4];

//ANALOG INPUTS ARRAYS - CO2 Sensor
int16_t CO2[4];

//PASSWORDS ARRAYS
long PASSWORD[3];

//User Settings ARRAYS
int16_t UserSetting[3];

//SCADA ARRAY
int16_t SCADA_DATA[4];

//Date Time Array
String AHUDateTime[2];
int16_t AHUDate[3];
int16_t AHUTime[2];

//SCHEDULES ARRAY
int16_t schedulerEnabled;
int16_t schedulerTempDisable = 0;
bool onBySchedular = false;
int16_t SCHEDULES[7][8];

//HOLIDAYS ARRAY
int16_t HOLIDAYS[3];
String SpecialHolidays[48];

//Preferances to store data permanently
Preferences prefs;

BluetoothSerial SerialBT;

long screenLastUpdateTime = 0;

long scheduler_last_check_time = 0;

long filter_1_last_check_time = 0;
long filter_1_checking_interval = 10000;
long filter_2_last_check_time = 0;
long filter_2_checking_interval = 10000;
long hepa_last_check_time = 0;
long hepa_checking_interval = 30000;
long fine_last_check_time = 0;
long fine_checking_interval = 30000;
long co2_last_check_time = 0;
long co2_checking_interval = 30000;

//RTC_Millis rtc;
RTC_DS3231 rtc;

const int PIC_Error_LED = 12;
const int PIC_COM_LED = 14;
const int WiFi_LED = 2;
const int WizReset = 4;

void setup() 
{
  pinMode(PIC_Error_LED, OUTPUT);
  pinMode(PIC_COM_LED, OUTPUT);
  pinMode(WiFi_LED, OUTPUT);
  pinMode(WizReset, OUTPUT);

  digitalWrite(WiFi_LED, LOW);
  digitalWrite(WizReset, LOW);
  
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  myNextion.init(); // send the initialization commands for Page 0
  SerialBT.begin("ESP32test"); //Bluetooth device name
  
  prefs.begin("ahu_settings"); // use "calibrations" namespace
  
  //prefs.clear();
  
  /*
  for(int i = 0; i < 10; i++)
    {
      digitalWrite(WiFi_LED, HIGH);
      delay(1000);
      digitalWrite(WiFi_LED, LOW);
      delay(1000);
    }
 */
 
 delay(3000); 
 initRTC();
 initAHUSettings();
}

void loop() 
{
  getNewData();
  AHUUpdate();
  sendNewValues();
  if(millis() - screenLastUpdateTime >= 1500)
    {
      updateScreen();
      screenLastUpdateTime = millis();
    }
  if(millis() - scheduler_last_check_time >= 15000)
    {
      checkScheduler();
      scheduler_last_check_time = millis();
    }
  if(AHUSetting[ahuMode] == 3)
    {
      getSCADA();
      setInputsSCADA();
    }
  serialEvent();
  //delay(500);
}

void serialEvent() {
  int events = 0;
  String eventsString = "";
  while (Serial.available()) {
    //get the new byte:
    char in = (char)Serial.read();
    eventsString = eventsString + in;
    events++;
  }
  for(int i = 0; i < events; i++)
    checkEvent(eventsString.charAt(i));
}

void checkEvent(char inChar)
{
  if (inChar == 'a') {
    screenCounter = 1;
    SerialBT.println(screenCounter);
    initOldValues();
  }
  else if (inChar == 'b') {
    screenCounter = 2;
    SerialBT.println(screenCounter);
    initOldValues();
  }
  else if (inChar == 'c') {
    screenCounter = 3;
    SerialBT.println(screenCounter);
    initOldValues();
  }
  else if (inChar == 'd') {
    screenCounter = 4;
    SerialBT.println(screenCounter);
    initOldValues();
  }
  else if (inChar == 'z') {
    screenCounter = 0;
    homeRefresh();
    SerialBT.println("Home Screen");
  }
  else if (inChar == 'x') {
    screenCounter = 10;
    SerialBT.println("Settings Screen");
  }
  else if (inChar == '1') {
    SerialBT.println("Start AHU");
    startAHU();
  }
  else if (inChar == '0') {
    SerialBT.println("Stop AHU");
    stopAHU();
  }
  else if (inChar == 'j') {
    SerialBT.println("AHU Mode Changed...!");
    //Turn off AHU if it is running
    
    if(AHUSetting[ahuStatus] == AHU_ON)
      {
        if(AHUSetting[ahuMode] == 1)
          stopAHU();
          
        if(AHUSetting[ahuMode] == 2)
          stopAHU_Manual_Mode();  //Init Auto Mode
      }
      
    amRrelayTurnOff();
      
    //AHU Mode Changed
    getAHUSettingsValues();
    
    if(AHUSetting[ahuMode] == 2)
      ahuManualModeInit();
    if(AHUSetting[ahuMode] == 3)
      ahuScadaModeInit();
  }
  else if (inChar == 'o' || inChar == 'p' || inChar == 'q' || inChar == 'r' || inChar == 's') {
    SerialBT.println("AHU Settings Changed...!");
    getAHUSettingsValues();
  }
  else if (inChar == 'i') {
    SerialBT.println("Password Changed...!");
    getPasswordsValues();
  }
  else if (inChar == 'e') {
    SerialBT.println("User Settings Changed...!");
    getUserSettingsValues();
  }
  else if (inChar == 't') {
    SerialBT.println("AI Settings Changed...!");
    getAISettings();
  }
  else if (inChar == 'u') {
    SerialBT.println("PID Settings Changed...!");
    getPIDSettings();
  }
  else if (inChar == 'n') {
    SerialBT.println("Date Time Settings Changed...!");
    getAHUDateTimeValues();
  }
  else if (inChar == 'f') {
    SerialBT.println("Date Time Set...!");
    setAHUDateTimeValues();
  }
  else if (inChar == 'g') {
    SerialBT.println("Set Current Date Time...!");
    setCurDateTimeValues();
  }
  else if (inChar == 'v') {
    SerialBT.println("Scheduler Settings Changed...!");
    getSchedularValues();
  }
  else if (inChar == 'w') {
    SerialBT.println("Holidays Settings Changed...!");
    getHolidaysValues();
  }
  else if (inChar == 'k') {
    SerialBT.println("Schedular Temporary Disabled...!");
    schedulerTempDisable = 1;
  }
  else if (inChar == 'l') {
    SerialBT.println("Schedular Temporary Enabled...!");
    schedulerTempDisable = 0;
  }
  else if (inChar == '#') {
    SerialBT.println("Calibration Mode...!");
    CalibrationMode();
  }
}

void updateScreen()
{
  switch(screenCounter)
      {
        case 0 :
          updateHomeScreen();
          break;
        case 1 :
          updateIOScreen_1();  //Calibrate 0 Values
          break;
        case 2 :
          updateIOScreen_2();  //Calibrate 0 Values
          break;
        case 3 :
          updateIOScreen_3();  //Calibrate 0 Values
          break;
        case 4 :
          updateIOScreen_4();  //Calibrate 0 Values
          break;
        default :
          break;
      }
}

void initRTC()
{
  //rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  if (!rtc.begin()) {
    SerialBT.println("Couldn't find RTC");
    //while (1);
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  if (rtc.lostPower()) {
    SerialBT.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}
