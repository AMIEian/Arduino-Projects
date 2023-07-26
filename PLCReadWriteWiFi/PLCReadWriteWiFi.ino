//#include <ESP8266WiFi.h>
#include "Settimino.h"

//const int WiFi_Error_LED = 2;   //RED
//const int Arduino_Error_LED = 4;  //BLUE
//const int PLC_Error_LED = 12;   //GREEN

// Uncomment next line to perform small and fast data access
#define DO_IT_SMALL

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x00, 0x50, 0x56, 0xC0, 0x00, 0x02 };

IPAddress Local(192,168,0,72); // Local Address
IPAddress PLC(192,168,0,70);   // PLC Address

char ssid[] = "Nitin Andro Labs";    // Your network SSID (name)
char pass[] = "Shreya.AMIEian";  // Your network password (if any)

IPAddress Gateway(192, 168, 0, 1);
IPAddress Subnet(255, 255, 255, 0);

int DBNum = 1; // This DB must be present in your PLC
byte Buffer[1024];

// S7Client will create an EthernetClient as TCP Client
S7Client Client(_S7WIFI);

unsigned long Elapsed; // To calc the execution time

unsigned long lastTime = 0; 

bool invert = false;

void(* resetFunc) (void) = 0;

//----------------------------------------------------------------------
// Setup : Init Ethernet and Serial port
//----------------------------------------------------------------------

int temp[11];

void setup() {
    /*
    wdt_disable();
    pinMode(WiFi_Error_LED, OUTPUT);
    pinMode(Arduino_Error_LED, OUTPUT);
    pinMode(PLC_Error_LED, OUTPUT);
    
    digitalWrite(WiFi_Error_LED, LOW);
    digitalWrite(Arduino_Error_LED, LOW);
    digitalWrite(PLC_Error_LED, LOW);

    for(int i = 0; i <= 4; i++)
      {
        digitalWrite(WiFi_Error_LED, HIGH);
        digitalWrite(Arduino_Error_LED, HIGH);
        digitalWrite(PLC_Error_LED, HIGH);
        delay(1000);
        digitalWrite(WiFi_Error_LED, LOW);
        digitalWrite(Arduino_Error_LED, LOW);
        digitalWrite(PLC_Error_LED, LOW);
        delay(1000);
      }
    */
    delay(1000);
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
     while (!Serial) {
      ; // wait for serial port to connect. Needed for Leonardo only
    }
    //ESP8266 Initialization    
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    //digitalWrite(WiFi_Error_LED, HIGH);
    WiFi.begin(ssid, pass);
    WiFi.config(Local, Gateway, Subnet);
    while (WiFi.status() != WL_CONNECTED) 
    {
        //digitalWrite(WiFi_Error_LED, ~digitalRead (WiFi_Error_LED));
        delay(500);
        Serial.print(".");
    }
    //digitalWrite(WiFi_Error_LED, LOW);
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.print("Local IP address : ");
    Serial.println(WiFi.localIP());  
}
//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------

bool Connect()
{
    int Result=Client.ConnectTo(PLC, 
                                  0,  // Rack (see the doc.)
                                  0); // Slot (see the doc.)
    Serial.print("Connecting to ");Serial.println(PLC);  
    if (Result==0) 
    {
      Serial.print("Connected ! PDU Length = ");Serial.println(Client.GetPDULength());
      //digitalWrite(WiFi_Error_LED, LOW);
    }
    else
      {
        Serial.print("Connection error 0x");
        Serial.println(Result, HEX);
        //digitalWrite(WiFi_Error_LED, HIGH);
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

  Serial.print("[ Dumping ");Serial.print(Length);
  Serial.println(" bytes ]===========================");
  for (i=0; i<Length; i++)
  {
    cnt++;
    if (buf[i]<10)
      Serial.print("0");
    Serial.print(buf[i],HEX);
    Serial.print(" ");
    if (cnt==16)
    {
      cnt=0;
      Serial.println();
    }
  }  

  Serial.println();
  
  int index = 0;
  int te;
  int x = 0;
  while(x < 22)
  {
    te = buf[x] << 8;
    x = x + 1;
    te = te + buf[x];
    x = x + 1;
    temp[index] = te;
    index = index + 1;
    Serial.print(te);
    Serial.print(" ");
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
    //digitalWrite(PLC_Error_LED, HIGH);
    Client.Disconnect(); 
  }

  delay(1000);
  //Serial.println("Resetting The Device.");
  //resetFunc();  //call reset
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
  /*
  if(Elapsed > lastTime)
    {
      Serial.println(Elapsed);
      lastTime = Elapsed;
    }
  */
}

//----------------------------------------------------------------------
// Main Loop
//----------------------------------------------------------------------
void loop() 
{
  int Size;
  int Result;
  void *Target;
  
  DBNum = 1;
   
  #ifdef DO_IT_SMALL
    Size=22;
    Target = NULL; // Uses the internal Buffer (PDU.DATA[])
  #else
    Size=1024;
    Target = &Buffer; // Uses a larger buffer
  #endif
  
  // Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(5);
  }
  
  Serial.print("Reading ");Serial.print(Size);Serial.print(" bytes from DB");Serial.println(DBNum);
  // Get the current tick
  MarkTime();
  Result=Client.ReadArea(S7AreaDB, // We are requesting DB access
                         2,    // DB Number
                         0,        // Start from byte N.0
                         Size,     // We need "Size" bytes
                         Target);  // Put them into our target (Buffer or PDU)
  if (Result==0)
  {
    ShowTime();
    Dump(Target, Size);
  }
  else
    CheckError(Result);
    
  delay(50);
  
  int16_t buf[] = {10000, 10002, 10003, 10004, 10005, 10006, 10007, 10008, 10009, 10010, 10011, 10012, 10013, 10014, 10015 , 10016, 17, 18, 19, 20, 21};
  int sz = sizeof(buf);

  if(invert == true)
    {
      buf[0] = 0;
      invert = false;
    }
  else
    {
      buf[0] = 1;
      invert = true;
    }

   // Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(5);
  }
  
  Serial.print("Write ");Serial.print(sz);Serial.println(" bytes from DB1.DBB0"); 
  // Put the current tick
  MarkTime();
  Result=Client.WriteArea(S7AreaDB, // We are requesting DB access
                         1,        // DB Number = 1
                         0,        // Start from byte N.0
                         sz,     // We need "Size" bytes
                         buf);  // Put them into our target (Buffer or PDU)
  if (Result==0)
  {
    //digitalWrite(PLC_Error_LED, LOW);
    ShowTime();
    //Dump(Target, sz);
  }
  else
    CheckError(Result);
  delay(1000); 
}
