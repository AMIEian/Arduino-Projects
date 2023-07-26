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

HardwareSerial gsm(1);
GPRS gprs(gsm, SIMCARD_PWKEY, SIMCARD_RST, SIM800_POWER_ON);

char smsText[200];
char ph_no[15];

void setup()
{
    Serial.begin(115200);
    initModem();
    for(int i = 1; i <= 10; i++)
      DeleteSMS(i);
    //Serial.println("Sending SMS...");
    //SendSMS("9819895913", "Hello World...!");
}

void loop()
{ 
    /*
    String number = checkRinging();
    if(number != "")
      {
        Serial.print("New Incoming Call From ...");
        Serial.println(number);
      }
    //modemLoop();
    */
    int res = IsSMSPresent(0);
    if(res > 0)
      {
        char re = GetSMS(res, ph_no, smsText, 159);
        if(re == 1)
          {
            Serial.println(ph_no);
            Serial.println(smsText);
          }
      }
    delay(5000);
}
