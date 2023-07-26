//#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <SIM900.h>
#include <sms.h>
#include <call.h>

#define beatLED 13

//GSM Controlling Variables
SMSGSM sms;
CallGSM in_call;

String masterNumber = "9664998039";
String authenticNumbers[7]; 
char phone_number[15];
int8_t totalAdmins = 0;
int8_t totalUsers = 0;
char addAdmin = 0;
char msgFrmAdmin = 0;
char msgFrmUser = 0;

void(* resetFunc) (void) = 0;//declare reset function at address 0

void setup() 
{
  initUnit();
}

void loop() 
{
  digitalWrite(beatLED, LOW);
  Check_New_SMS();
  digitalWrite(beatLED, HIGH);
  delay(1000);
  digitalWrite(beatLED, LOW);
  Check_New_Call();
  digitalWrite(beatLED, HIGH);
  delay(1000);
  digitalWrite(beatLED, LOW);
  LogicController();
  digitalWrite(beatLED, HIGH);
  delay(1000);
}
