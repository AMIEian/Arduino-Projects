#include <SPI.h>
#include <Ethernet.h>
#include "Settimino.h"

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0F, 0x08, 0xE1 };

IPAddress Local(192,168,0,71); // Local Address
IPAddress PLC(192,168,0,70);   // PLC Address

byte Buffer[1024];  //Big Data Buffer for printing

int ReadDBNum = 1; // This DB must be present in your PLC
int ReadStartPointer = 0;  //Starting location for data to get read
int ReadDataSize = 50;  //Number of bytes we wany to read from PLC Database
void *ReadDataBuffer = NULL;  //Use default library buffer (PDU.DATA[])

int WriteDBNum = 1; // This DB must be present in your PLC
int WriteStartPointer = 0;  //Starting location for data to get write
byte WriteDataBuffer[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 , 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50};
int WriteDataSize = sizeof(WriteDataBuffer);  //Number of bytes we wany to write in PLC Database

// S7Client will create an EthernetClient as TCP Client
S7Client Client(_S7WIRED);

unsigned long Elapsed; // To calc the execution time

//Gains and Offsets for calibration
//We have 16 + 4 = 20 Offsets and Gain Values. One for each channel.
//By default all gains are 1 and offsets are 0
float offsets[20] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float gains[20] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

//----------------------------------------------------------------------
// Setup : Init Ethernet and Serial port
//----------------------------------------------------------------------
void setup() 
  {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
     while (!Serial) 
      {
        ; // wait for serial port to connect.
      }
    // Start the Ethernet Library
    Ethernet.begin(mac, Local);
    delay(2000); 
    Serial.println("");
    Serial.println("Cable connected");  
    Serial.print("Local IP address : ");
    Serial.println(Ethernet.localIP());  
  }
//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------

bool Connect()
  {
    int Result = Client.ConnectTo(PLC, 0, 0); // Rack = 0, Slot = 0
    Serial.print("Connecting to ");
    Serial.println(PLC);  
    if (Result == 0) 
      {
        Serial.print("Connected ! PDU Length = ");Serial.println(Client.GetPDULength());
      }
    else
      Serial.println("Connection error");
    return Result==0;
  }

//----------------------------------------------------------------------
// Dumps a buffer
//----------------------------------------------------------------------
void Dump(void *Buffer, int Length)
  {
    int i, cnt = 0;
    pbyte buf;
  
    if (Buffer != NULL)
      buf = pbyte(Buffer);
    else  
      buf = pbyte(&PDU.DATA[0]);

    Serial.print("[ Dumping ");
    Serial.print(Length);
    Serial.println(" bytes ]===========================");
    for (i = 0; i < Length; i++)
      {
        cnt++;
        if (buf[i] < 10)
          Serial.print("0");
        Serial.print(buf[i]);
        Serial.print(" ");
        if (cnt == 16)
          {
            cnt = 0;
            Serial.println();
          }
      }  
    Serial.println("===============================================");
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
// Time Calculator Routines
//----------------------------------------------------------------------
void MarkTime()
  {
    Elapsed = millis();
  }
//----------------------------------------------------------------------

void ShowTime()
  {
    // Calcs the time
    Elapsed = millis() - Elapsed;
    Serial.print("Job time (ms) : ");
    Serial.println(Elapsed);   
  }

//----------------------------------------------------------------------
// Main Loop
//----------------------------------------------------------------------
void loop() 
  {
    int Result;
    // Connection
    while (!Client.Connected)
      {
        if (!Connect())
        delay(500);
      }
  
    Serial.print("Reading ");
    Serial.print(ReadDataSize);
    Serial.print(" bytes from DB");
    Serial.println(ReadDBNum);
    
    // Get the current tick
    MarkTime();
    Result = Client.ReadArea(S7AreaDB, ReadDBNum, ReadStartPointer, ReadDataSize, ReadDataBuffer);
    if (Result == 0)
      {
        ShowTime();
        Dump(ReadDataBuffer, ReadDataSize);
      }
    else
      CheckError(Result);
    
    delay(10000);  
  
    Serial.print("Writing ");
    Serial.print(WriteDataSize);
    Serial.print(" bytes to DB"); 
    Serial.println(WriteDBNum);
    
    // Put the current tick
    MarkTime();
    Result = Client.WriteArea(S7AreaDB, WriteDBNum, WriteStartPointer, WriteDataSize, WriteDataBuffer);
    if (Result == 0)
      {
        ShowTime();
        Dump(WriteDataBuffer, WriteDataSize);
      }
    else
      CheckError(Result);
      
    delay(500); 
  }
