/*----------------------------------------------------------------------
 Read Status Demo

 Created 12 Dec 2016
 by Davide Nardella
 
------------------------------------------------------------------------
This demo shows how to read the PLC status.
The model extended is needed.

- During the loop, try to disconnect the ethernet cable.
  The system will report the error and will reconnect automatically
  when you re-plug the cable.
- For safety, this demo *doesn't write* data into the PLC, try
  yourself to change ReadArea with WriteArea.
- This demo uses ConnectTo() with Rack=0 and Slot=2 (S7300)
  - If you want to connect to S71200/S71500 change them to Rack=0, Slot=0.
  - If you want to connect to S7400 see your hardware configuration.
  - If you want to work with a LOGO 0BA7 or S7200 please refer to the
    documentation and change 
    Client.ConnectTo(<IP>, <Rack>, <Slot>);
    with the couple
    Client.SetConnectionParams(<IP>, <LocalTSAP>, <Remote TSAP>);
    Client.Connect();

NodeMCU 1.0 ESP-12E ESP8266 supported
    
----------------------------------------------------------------------*/
// Wifi  -> #define S7WIFI
// Cable -> #define S7WIRED
#define S7WIFI

#include <SPI.h>
#include <Ethernet.h>
#ifdef S7WIFI
  #include <ESP8266WiFi.h> 
#endif
#include "Settimino.h"

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0F, 0x08, 0xE11 };
  
IPAddress Local(192,168,0,90); // Local Address
IPAddress PLC(192,168,0,12);   // PLC Address

// Following constants are needed if you are connecting via WIFI
// The ssid is the name of my WIFI network (the password obviously is wrong)
char ssid[] = "SKYNET-AIR";    // Your network SSID (name)
char pass[] = "yourpassword";  // Your network password (if any)
IPAddress Gateway(192, 168, 0, 1);
IPAddress Subnet(255, 255, 255, 0);

#ifdef S7WIFI
// S7Client will create a WiFiClient as TCP Client
S7Client Client(_S7WIFI);
#else
// S7Client will create an EthernetClient as TCP Client
S7Client Client(_S7WIRED);
#endif

unsigned long Elapsed; // To calc the execution time
int LastStatus = -1;   // To force the first print

//----------------------------------------------------------------------
// Setup : Init Ethernet and Serial port
//----------------------------------------------------------------------
void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
     while (!Serial) {
      ; // wait for serial port to connect. Needed for Leonardo only
    }
#ifdef S7WIFI
//--------------------------------------------- ESP8266 Initialization    
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    WiFi.config(Local, Gateway, Subnet);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.print("Local IP address : ");
    Serial.println(WiFi.localIP());
#else
//--------------------------------Wired Ethernet Shield Initialization    
    // Start the Ethernet Library
    Ethernet.begin(mac, Local);
    // Setup Time, someone said me to leave 2000 because some 
    // rubbish compatible boards are a bit deaf.
    delay(2000); 
    Serial.println("");
    Serial.println("Cable connected");  
    Serial.print("Local IP address : ");
    Serial.println(Ethernet.localIP());
#endif   
}
//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------
bool Connect()
{
    int Result=Client.ConnectTo(PLC, 
                                  0,  // Rack (see the doc.)
                                  2); // Slot (see the doc.)
    Serial.print("Connecting to ");Serial.println(PLC);  
    if (Result==0) 
    {
      Serial.print("Connected ! PDU Length = ");Serial.println(Client.GetPDULength());
    }
    else
      Serial.println("Connection error");
    return Result==0;
}
//----------------------------------------------------------------------
// Prints the Error number
//----------------------------------------------------------------------
void CheckError(int ErrNo)
{
  Serial.print("Error No. 0x");
  Serial.println(ErrNo, HEX);
  
  // Checks if it's a Severe Error => we need to disconnect
  if (ErrNo & 0x00FF)
  {
    Serial.println("SEVERE ERROR, disconnecting.");
    Client.Disconnect(); 
  }
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
//----------------------------------------------------------------------
// Main Loop
//----------------------------------------------------------------------
void loop() 
{
  int Result, Status;
  
  // Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(500);
  }
  
  MarkTime();
  Result = Client.GetPlcStatus(&Status);
  if (Result==0)
  {
    if (Status!=LastStatus)
    {
      LastStatus=Status;
      Serial.print("CPU Status change detected, now is ");
      switch (Status)
      {
        case S7CpuStatusUnknown:
          Serial.println("UNKNOWN");
          break;
        case S7CpuStatusRun:
          Serial.println("RUNNING");
          break;
        case S7CpuStatusStop:
          Serial.println("STOPPED");
          break;
      }
    }
  }
  else
    CheckError(Result);
    
  delay(500);  
}



