//#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <SIM900.h>
#include <sms.h>
#include <call.h>

//GSM Controlling Variables
SMSGSM sms;
CallGSM in_call;

String masterNumber = "9819895913";
String authenticNumbers[7]; 
char phone_number[15];
int8_t totalAdmins = 0;
int8_t totalUsers = 0;
bool addAdmin = false;
bool msgFrmAdmin = false;
bool msgFrmUser = false;

void setup() 
{
  initUnit();
}

void loop() 
{
  Check_New_SMS();
  Check_New_Call();
  LogicController();
  delay(1000);
}
