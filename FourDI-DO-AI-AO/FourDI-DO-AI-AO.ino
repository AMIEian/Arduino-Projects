#include<WiFi.h>
#include <Preferences.h>
#include "Settimino.h"
#include "BluetoothSerial.h"

#define WiFi_Error_LED 12   //RED
#define Com_LED 27   //GREEN

// Uncomment next line to perform small and fast data access
#define DO_IT_SMALL

// S7Client will create an EthernetClient as TCP Client
S7Client ClientWiFi(_S7WIFI);
S7Client ClientEth(_S7WIRED);

bool WiFiConnected = false;
bool EthConnected = false;

//WiFi Settings
String id = "Nitin Andro";  // Labs";;
String pass = "Shreya.AMIEian";
String gWay = "192.168.0.1";
String sMask = "255.255.255.0";
String plc_ip = "192.168.0.70";
int dbNum = 1;
String local_ip = "192.168.0.72";

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress Local; // Local Address
IPAddress PLC;   // PLC Address
IPAddress Gateway;
IPAddress Subnet;

String pcCommand = "";         // a String to hold incoming data

boolean dataSent = false;

byte Buffer[1024];

unsigned long Elapsed; // To calc the execution time

unsigned long lastTime = 0; 

int invert = 0;

int conFail = 0;

#define MAX_COUNTS 10000

//Indexes for various readings in counts array
#define AI_INDEX 0
#define DI_INDEX 4

//Indexes for various readings in input_counts array
#define AO_INDEX 0
#define DO_INDEX 4

//Indexes for various readings in fullVals array
#define AI_FULLVALS_INDEX 0
#define DAC_FULLVALS_INDEX 4

int16_t zeroVals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                           //AI = A to D                AO = E to H
int16_t fullVals[8] = {10000, 10000, 10000, 10000, 3800, 3800, 3800, 3800};
int16_t counts[5] = {10000, 5000, 2500, 1250, 3};
int16_t inputCounts[5] = {0, 0, 0, 0, 0};
int16_t analogCounts[4] = {0, 0, 0, 0};

Preferences prefs;

BluetoothSerial SerialBT;

void setup() 
  {
    //wdt_disable();
    
    pinMode(WiFi_Error_LED, OUTPUT);
    pinMode(Com_LED, OUTPUT);
    
    digitalWrite(WiFi_Error_LED, LOW);
    digitalWrite(Com_LED, LOW);

    initModule();
    
    for(int i = 0; i <= 5; i++)
      {
        digitalWrite(WiFi_Error_LED, HIGH);
        digitalWrite(Com_LED, HIGH);
        delay(1000);
        digitalWrite(WiFi_Error_LED, LOW);
        digitalWrite(Com_LED, LOW);
        delay(1000);
      }

    delay(1000);
    
    Serial.begin(115200);

    SerialBT.begin("Polygon"); //Bluetooth device name
    
    // reserve 200 bytes for the inputString:
    pcCommand.reserve(200);
    
    prefs.begin("calibrations"); // use "calibrations" namespace
    loadCalFromEEPROM();
    
    //Get WiFi settings from Arduino EEPROM
    getWiFiSettings();
    
    //Create IP Addresses from received data
    Local = stringToIP(local_ip); // Local Address
    Gateway = stringToIP(gWay);
    Subnet = stringToIP(sMask);
    PLC = stringToIP(plc_ip);
    WiFi.macAddress(mac);
    // Connect to WiFi
    if(connectToWiFi() == true)
      WiFiConnected = true;
    else if(connectToEth() == true)
      EthConnected = true;
  }

void loop() 
  {
    int dataSize, Result;
    dataSize = sizeof(counts);
    
    // Connection
    if(WiFiConnected)
      {
        while (!ClientWiFi.Connected)
          {
            if (!Connect())
            {
              delay(5);
              serialEvent();
            }
          }
        Result=ClientWiFi.WriteArea(S7AreaDB, dbNum, 0, dataSize, counts); 
      }
    else if(EthConnected)
      {
        while (!ClientEth.Connected)
          {
            if (!Connect())
            {
              delay(5);
              serialEvent();
            }
          }
        Result=ClientEth.WriteArea(S7AreaDB, dbNum, 0, dataSize, counts); 
      }

    // We are requesting DB access with DB Number, Start from byte N.0, We need "Size" in bytes finally Put them into our target (Buffer or PDU)
    if (Result==0)
      {
        dataSent = true;
      }
    else
      CheckError(Result); 
      
    if(dataSent)
      {
        bool newData = false;
        while(newData == false)
          newData = getNewData();
        dataSent = false;
      }
    digitalWrite(Com_LED, HIGH);
    delay(1000);
    digitalWrite(Com_LED, LOW);
    
    //READ DO VALUES FROM PLC AND SEND THEM TO ARDUINO
    void *Target;
    int ReadSize=sizeof(inputCounts);
    int RddbNum = dbNum + 1;
    Target = NULL;
    // Connection
    if(WiFiConnected)
      {
        while (!ClientWiFi.Connected)
          {
            if (!Connect())
            {
              delay(5);
              serialEvent();
            }
          }
        Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 2, ReadSize, Target);  // Put them into PDU
      }
    else if(EthConnected)
      {
        while (!ClientEth.Connected)
          {
            if (!Connect())
            {
              delay(5);
              serialEvent();
            } 
          }
        Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 2, ReadSize, Target);  // Put them into PDU
      }
      
    if (Result==0)
      {
        setNewValues();
        //Dump(Target, ReadSize);
      }
    else
      CheckError(Result);
      
    digitalWrite(Com_LED, HIGH);
    delay(1000);
    digitalWrite(Com_LED, LOW);
    
    serialEvent();
  }

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (inChar == '#') {
      CalibrationMode();
    }
  }
}

void initModule()
{
  AiSetup();
  AoSetup();
  DiSetup();
  DoSetup();
}

void setNewValues()
  {
    pbyte buf;
    char DOValue;
    buf = pbyte(&PDU.DATA[0]);
    int temp[5] = {0, 0, 0, 0, 0};
    int index = 0;
    int te;
    int x = 0;
    //Fill Data into temp array
    while(x < 10)
      {
        te = buf[x] << 8;
        x = x + 1;
        te = te + buf[x];
        x = x + 1;
        temp[index] = te;
        index = index + 1;
      }
    //Check whether there is any new data
    bool sendValues = false;
    int i;
    for(i = 0; i < 5; i++)
      {
        if(temp[i] != inputCounts[i])
          {
            sendValues = true;
            break;
          }
      }
    if(sendValues == true)
      {
        for(i = 0; i < 5; i++)
          {
            //Save sent value into old values array
            inputCounts[i] = temp[i];
          }
        SetDOs();
        SetAOs();
      }
  }

bool getNewData() 
  {
    GetDIs();
    GetAIs();
    return true;
  }

void getWiFiSettings()
  {
    String inputString;
    String calDone = "";
    calDone = prefs.getString("wifiConfigDone");
    if(calDone == "Done")
        {
          inputString = prefs.getString("wifiConfig");
          id = inputString.substring(inputString.lastIndexOf('$')+1, inputString.lastIndexOf('%'));
          pass = inputString.substring(inputString.lastIndexOf('%')+1, inputString.lastIndexOf('^'));
          gWay = inputString.substring(inputString.lastIndexOf('^')+1, inputString.lastIndexOf('&'));
          sMask = inputString.substring(inputString.lastIndexOf('&')+1, inputString.lastIndexOf('*'));
          plc_ip = inputString.substring(inputString.lastIndexOf('*')+1, inputString.lastIndexOf('('));
          dbNum = inputString.substring(inputString.lastIndexOf('(')+1, inputString.lastIndexOf(')')).toInt();
          local_ip = inputString.substring(inputString.lastIndexOf('-')+1, inputString.lastIndexOf('+'));
        }
  }

IPAddress stringToIP(String strIP)
  {
    byte Parts[4] = {0,0,0,0};
    int Part = 0;
    for ( int i=0; i<strIP.length(); i++ )
    {
      char c = strIP.charAt(i);
      if ( c == '.' )
      {
        Part++;
        continue;
      }
      Parts[Part] *= 10;
      Parts[Part] += c - '0';
    }
    IPAddress ip( Parts[0], Parts[1], Parts[2], Parts[3] ); 
    return ip;
  }
