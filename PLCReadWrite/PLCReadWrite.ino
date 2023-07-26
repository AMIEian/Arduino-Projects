#include "Settimino.h"
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

const int com_ok = 21;
const int com_err = 22;

// Uncomment next line to perform small and fast data access
#define DO_IT_SMALL

byte mac[] = { 0x00, 0x50, 0x56, 0xC0, 0x00, 0x06 };

IPAddress Local(192,168,0,90); // Local Address
IPAddress PLC(192,168,0,70);   // PLC Address
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

int DBNum = 1; // This DB must be present in your PLC
byte Buffer[10];

// S7Client will create an EthernetClient as TCP Client
S7Client Client(_S7WIRED);

unsigned long Elapsed; // To calc the execution time

unsigned long lastTime = 0; 

bool invert = false;

const int RESET_P = 2;

String inputString = "";

int value[1];

void setup() 
{    
    delay(1000);
    pinMode(com_err, OUTPUT);
    pinMode(com_ok, OUTPUT);
    digitalWrite(com_err, LOW);
    digitalWrite(com_ok, LOW);
    // use Serial (port 0); initialize Modbus communication baud rate
    Serial.begin(9600);
    SerialBT.begin("POLYGON");
    // Start the Ethernet Library
    connectToEth();
}

//----------------------------------------------------------------------
// Main Loop
//----------------------------------------------------------------------
void loop() 
{
  int Size, Result;
  void *Target;

  digitalWrite(com_ok, LOW);
  bool newData = false;
  while(newData == false)
    newData = getNewData();
  SerialBT.print("Value = ");
  SerialBT.println(value[0]);
   //Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(5);
  }
  
  //Serial.print("Write ");Serial.print(Size);Serial.println(" bytes from DB1.DBB0"); 
  // Put the current tick
  //MarkTime();
  //value[0] = 100;
  Result=Client.WriteArea(S7AreaDB, // We are requesting DB access
                         DBNum,        // DB Number = 1
                         80,        // Start from byte N.0
                         2,     // We need "Size" bytes
                         value);  // Put them into our target (Buffer or PDU)      
  digitalWrite(com_ok, HIGH);              
  delay(150);

  digitalWrite(com_ok, LOW);
  Target = NULL;
  // Connection
  while (!Client.Connected)
    {
      if (!Connect())
        delay(5);
    }
    
  //MarkTime();
  Result=Client.ReadArea(S7AreaDB, DBNum, 82, 2, Target);  // Put them into PDU
  if (Result==0)
  {
    digitalWrite(com_err, LOW);
    //ShowTime();
    Dump(Target, 2);
    pbyte buf;
    buf = pbyte(&PDU.DATA[0]);
    /*
    Serial.print(buf[0]);
    Serial.println(buf[1]);
    */
    int te;
    te = buf[0] << 8;
    te = te + buf[1];
    Serial.println(te);
    SerialBT.print("DO Value = ");
    SerialBT.println(te);
  }
  else
    CheckError(Result);
  digitalWrite(com_ok, HIGH);
  delay(150); 
}

bool connectToEth()
{  
  SerialBT.println("Connecting Ethernet...");
  //digitalWrite(WiFi_Error_LED, HIGH);
  // initialize the ethernet device
  Ethernet.init(5);
  Ethernet.begin(mac, ip, myDns);
  SerialBT.print("Assigned IP ");
  SerialBT.println(Ethernet.localIP());
  /*
  // start the Ethernet connection:
  SerialBT.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    SerialBT.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      SerialBT.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      SerialBT.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    SerialBT.print("  DHCP assigned IP ");
    SerialBT.println(Ethernet.localIP());
  }
  */
  // give the Ethernet shield a second to initialize:
  delay(1000);
  return true;
}

//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------

bool Connect()
{
    int Result=Client.ConnectTo(PLC, 
                                  0,  // Rack (see the doc.)
                                  0); // Slot (see the doc.)
    SerialBT.print("Connecting to ");SerialBT.println(PLC);  
    if (Result==0) 
      {
        digitalWrite(com_err, LOW);
        SerialBT.print("Connected ! PDU Length = ");SerialBT.println(Client.GetPDULength());
      }
    else
      {
        digitalWrite(com_err, HIGH);
        SerialBT.print("Connection error 0x");
        SerialBT.println(Result, HEX);  
        Client.Disconnect();    
        //resetFunc();  
      }
    return Result==0;
}

//----------------------------------------------------------------------
// Dumps a buffer (a very rough routine)
//----------------------------------------------------------------------
void Dump(void *Buffer, int Length)
{
  int i, cnt=0;
  pbyte buf;
  
  if (Buffer!=NULL)
    buf = pbyte(Buffer);
  else  
    buf = pbyte(&PDU.DATA[0]);
 
  SerialBT.print("[ Dumping ");SerialBT.print(Length);
  SerialBT.println(" bytes ]===========================");
  for (i=0; i<Length; i++)
  {
    cnt++;
    if (buf[i]<10)
      SerialBT.print("0");
    SerialBT.print(buf[i]);
    SerialBT.print(" ");
    if (cnt==16)
    {
      cnt=0;
      SerialBT.println();
    }
  }  
  SerialBT.println("===============================================");
}

//----------------------------------------------------------------------
// Prints the Error number
//----------------------------------------------------------------------
void CheckError(int ErrNo)
{
  digitalWrite(com_err, HIGH);
  SerialBT.print("Error No. 0x");
  SerialBT.println(ErrNo, HEX);
  
  // Checks if it's a Severe Error => we need to disconnect
  if (ErrNo & 0x00FF)
  {
    SerialBT.println("SEVERE ERROR, disconnecting.");
    Client.Disconnect(); 
    //resetFunc();
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
  //Serial.print("Job time (ms) : ");
  //Serial.println(Elapsed);
  /*
  if(Elapsed > lastTime)
    {
      Serial.println(Elapsed);
      lastTime = Elapsed;
    }
  */
}

boolean getNewData()
{
  boolean stringComplete = false;
  inputString = "";
  int waitCount = 0;

  Serial.print('#');
  //digitalWrite(Arduino_Error_LED, HIGH);
  
  while(Serial.available() <= 0)
    {
      delay(1);
      waitCount++;
      if(waitCount > 5000)
        {
          waitCount = 0;
          Serial.print('#');
        }
    }

  waitCount = 0;
  
  while(stringComplete == false)
    {    
      while(Serial.available()) 
        {
          // get the new byte:
          char inChar = (char)Serial.read();
          inputString += inChar;
          if (inChar == '\n' || inChar == '\r') 
            stringComplete = true;
        }
      delay(1);
      waitCount++;
      if(waitCount > 1000)
        return false;
    }
  SerialBT.print("String = ");
  SerialBT.println(inputString);
  value[0] = inputString.toInt();
  inputString = "";
  //digitalWrite(Arduino_Error_LED, LOW);
  return true;
}
