/**
 * reuseConnection.ino
 *
 *  Created on: 22.11.2015
 *
 */


#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;

HTTPClient http;

const char ssid[] = "Eagle Control Systems";
const char pass[] = "AMIEian.Shreya";

const char server[] = "http://192.168.0.105:80"; // domain name: example.com, maker.ifttt.com, etc
const char resource[] = "/upload.php?";
const char dev_id[] = "id=8";
char url[1024];

char buf[10];
double param = 13;

unsigned long Elapsed; // To calc the execution time

void setup() {
    USE_SERIAL.begin(115200);
    
    wifiMulti.addAP(ssid, pass);

    // allow reuse (if server supports it)
    http.setReuse(true);
}

void loop() {
    createURL();
    USE_SERIAL.println(url);
    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {
        MarkTime();
        http.begin(url);
        //http.begin("http://192.168.0.105/upload.php?id=8&a=997&b=511&c=291");

        int httpCode = http.GET();
        ShowTime();
        if(httpCode > 0) {
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                http.writeToStream(&USE_SERIAL);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        USE_SERIAL.println();
        USE_SERIAL.println();
        http.end();
    }

    delay(1000);
}

void createURL()
{
  memset(url, 0, sizeof url);
  strcat(url,server);
  strcat(url,resource);
  strcat(url,dev_id);
  strcat(url,"&a=");
  strcat(url,itoa(random(1024),buf,10));
  strcat(url,"&b=");
  strcat(url,itoa(random(1024),buf,10));
  strcat(url,"&c=");
  strcat(url,itoa(random(1024),buf,10));
}

//----------------------------------------------------------------------
// Profiling routines
//----------------------------------------------------------------------
void MarkTime()
{
  Elapsed=millis();
}
//----------------------------------------------------------------------
void ShowTime()
{
  // Calcs the time
  Elapsed=millis()-Elapsed;
  Serial.print("Job time (ms) : ");
  Serial.println(Elapsed);   
}
