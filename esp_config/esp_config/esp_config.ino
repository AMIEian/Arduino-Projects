
#include <Adafruit_ESP8266.h>
#include <SoftwareSerial.h>

//Arduino Pin Definitions
#define ESP_RX   10
#define ESP_TX   11
#define ESP_RST  4
#define LED 13
//Software Serial Object
SoftwareSerial softser(ESP_RX, ESP_TX);
//ESP8266 WiFi Object
Adafruit_ESP8266 wifi(&softser, &Serial, ESP_RST);

boolean comm = false;
char buffer[50];
int blink_time;

void setup() 
{ 
  pinMode(LED, OUTPUT);
  softser.begin(115200); // Soft serial connection to ESP8266
  Serial.begin(57600); while(!Serial); // UART serial debug

  Serial.println(F("Configuring ESP8266..."));

  wifi.println(F("AT"));
  if(wifi.readLine(buffer, sizeof(buffer))) 
    {
      Serial.println(buffer);
      if(wifi.find())
        comm = true; // Discard the 'OK' that follows 
      else
        Serial.println(F("Unable to execute AT...!"));   
    } 
  else
    {
      Serial.println(F("Unable to Communicate with ESP8266...!"));
    }
  if(comm)
    {
      //Change Mode
      wifi.println(F("AT+CWMODE=1"));
      if(wifi.readLine(buffer, sizeof(buffer))) 
        {
          Serial.println(buffer);
          if(wifi.find())
            comm = true; // Discard the 'OK' that follows 
          else
            {
              comm = false;
              Serial.println(F("Unable to Change Mode...!")); 
            }
        } 
      else
        {
          comm = false;
          Serial.println(F("Unable to Communicate with ESP8266...!"));
        }

      //Change Baud Rate
      wifi.println(F("AT+UART_DEF=9600,8,1,0,0"));
      if(wifi.readLine(buffer, sizeof(buffer))) 
        {
          Serial.println(buffer);
          if(wifi.find())
            comm = true; // Discard the 'OK' that follows 
          else
            {
              comm = false;
              Serial.println(F("Unable to Change Mode...!")); 
            }
        } 
      else
        {
          comm = false;
          Serial.println(F("Unable to Communicate with ESP8266...!"));
        }
    }
}

void loop() 
{
  if(comm)
    blink_time = 100;
  else
    blink_time = 1000;
  while(1)
    {
      digitalWrite(LED,HIGH);
      delay(blink_time);
      digitalWrite(LED,LOW);
      delay(blink_time);
    }
}

