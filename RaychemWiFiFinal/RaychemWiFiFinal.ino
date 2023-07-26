// Import required libraries
#include <ESP8266WiFi.h>
#include "Settimino.h"

// Uncomment next line to perform small and fast data access
#define DO_IT_SMALL

const int WiFi_Error_LED = 2;   //RED
const int Arduino_Error_LED = 4;  //BLUE
const int PLC_Error_LED = 12;   //GREEN

//WiFi Settings
String id = "";
String pass = "";
String gWay = "";
String sMask = "";
String plc_ip = "";
int dbNum = 1;
int targetDbNum = 1;
String local_ip = "";

IPAddress Local; // Local Address
IPAddress PLC;   // PLC Address
IPAddress Gateway;
IPAddress Subnet;

String inputString = "";         // a String to hold incoming data

int16_t counts[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

boolean dataSent = false;

byte Buffer[1024];

// S7Client will create an EthernetClient as TCP Client
S7Client Client(_S7WIFI);

unsigned long Elapsed; // To calc the execution time

unsigned long lastTime = 0; 

bool invert = false;

void(* resetFunc) (void) = 0;

//----------------------------------------------------------------------
// Setup : Init WiFi and Serial port
//----------------------------------------------------------------------

void setup() 
  {
    wdt_disable();
    
    Serial.begin(115200);
    // reserve 200 bytes for the inputString:
    inputString.reserve(200);

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

    delay(1000);
    
    //Get WiFi settings from Arduino EEPROM
    getWiFiSettings();
    
    //Create IP Addresses from received data
    Local = stringToIP(local_ip); // Local Address
    Gateway = stringToIP(gWay);
    Subnet = stringToIP(sMask);
    PLC = stringToIP(plc_ip);
    
    // Connect to WiFi
    char idbuff[25];
    id.toCharArray(idbuff, 25);
    char passbuff[25];
    pass.toCharArray(passbuff, 25);
    
    digitalWrite(WiFi_Error_LED, HIGH);
    WiFi.begin(idbuff, passbuff);  
    WiFi.config(Local, Gateway, Subnet);
    
    while (WiFi.status() != WL_CONNECTED) 
      {
        digitalWrite(WiFi_Error_LED, ~digitalRead (WiFi_Error_LED));
        delay(500);
      }
    digitalWrite(WiFi_Error_LED, LOW);
  }

//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------

bool Connect()
  {
      int Result = Client.ConnectTo(PLC, 0, 0); //PLC IP, Slot 0, Rack 0
      
      //Serial.print("Connecting to ");Serial.println(PLC);  
      if (Result==0) 
        {
          //Serial.print("Connected ! PDU Length = ");Serial.println(Client.GetPDULength());
          digitalWrite(WiFi_Error_LED, LOW);
        }
      else
        {
          //Serial.print("Connection error 0x"); Serial.println(Result, HEX);
          digitalWrite(WiFi_Error_LED, HIGH);
        }
      
      return Result == 0;
  }

//----------------------------------------------------------------------
// Dumps a buffer (a very rough routine)
//----------------------------------------------------------------------
void Dump(void *Buffer, int Length)
  {
    int i, cnt = 0;
    pbyte buf;
    
    if (Buffer != NULL)
      buf = pbyte(Buffer);
    else  
      buf = pbyte(&PDU.DATA[0]);
  
    Serial.print("[ Dumping ");Serial.print(Length);
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
    //Serial.print("Error No. 0x");
    //Serial.println(ErrNo, HEX);
    
    // Checks if it's a Severe Error => we need to disconnect
    if (ErrNo & 0x00FF)
      {
        digitalWrite(PLC_Error_LED, HIGH);
        //Serial.println("SEVERE ERROR, disconnecting.");
        Client.Disconnect(); 
      }
  }

//----------------------------------------------------------------------
// Time related routines
//----------------------------------------------------------------------

void MarkTime()
  {
    Elapsed = millis();
  }
//----------------------------------------------------------------------

void ShowTime()
  {
    // Calcs the time
    Elapsed = millis()-Elapsed;
    Serial.print("Job time (ms) : ");
    Serial.println(Elapsed);
  }

void loop() 
  {
    getNewData();
    
    int dataSize, Result;
    dataSize = sizeof(counts);
    
    // Connection
    while (!Client.Connected)
      {
        if (!Connect())
          delay(5);
      }

    MarkTime();
    Result=Client.WriteArea(S7AreaDB, targetDbNum, 0, dataSize, counts); 
    // We are requesting DB access with DB Number, Start from byte N.0, We need "Size" in bytes finally Put them into our target (Buffer or PDU)
    
    if (Result==0)
      {
        dataSent = true;
        digitalWrite(PLC_Error_LED, LOW);
      }
    else
      CheckError(Result); 
      
    if(dataSent)
      {
        getNewData();
        dataSent = false;
      }
      
    delay(50);
    
    //READ DO VALUES FROM PLC AND SEND THEM TO ARDUINO
    void *Target;
    int ReadSize=1;
    Target = NULL;
    
    // Connection
    while (!Client.Connected)
      {
        if (!Connect())
          delay(5);
      }
      
    MarkTime();
    Result=Client.ReadArea(S7AreaDB, dbNum, 42, ReadSize, Target);  // Put them into PDU
    if (Result==0)
      {
        pbyte buf;
        char DOValue;
        buf = pbyte(&PDU.DATA[0]);
        DOValue = (char)buf[0];
        DOValue = (char)DOValue + '0';
        Serial.print('0');
        Serial.println(DOValue);
      }
    else
      CheckError(Result);
        
    delay(50);
  }

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void getNewData() 
  {
    boolean stringComplete = false;
    inputString = "";
    int waitCount = 0;
    
    Serial.print('#');
    digitalWrite(Arduino_Error_LED, HIGH);
    
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
    
    while(stringComplete == false)
      {
        while (Serial.available()) 
          {
            // get the new byte:
            char inChar = (char)Serial.read();
            inputString += inChar;
            if (inChar == '\n') 
              stringComplete = true;
          }
      }
    
    String valueStr = "";
    int value = 0;
    int countIndex = 0;
    
    if(inputString.charAt(0) == 'R')
      targetDbNum = dbNum + 1;
    else if(inputString.charAt(0) == 'F')
      targetDbNum = dbNum + 2;
    else
      targetDbNum = dbNum;
      
    for(int i = 1; i < inputString.length(); i++)
      {
        if(inputString.charAt(i) == ' ' || inputString.charAt(i) == 'R' || inputString.charAt(i) == 'F' || inputString.charAt(i) == '\n')
          {
            value = valueStr.toInt();
            counts[countIndex] = value;
            countIndex++;
            valueStr = "";
            value = 0;  
          }
        else
          valueStr = valueStr + inputString.charAt(i);
      }

    stringComplete = false;
    inputString = "";
    digitalWrite(Arduino_Error_LED, LOW);
  }

void getWiFiSettings()
  {
    boolean stringComplete = false;
    inputString = "";
    int waitCount = 0;
    
    Serial.println('@');
    digitalWrite(Arduino_Error_LED, HIGH);
    
    while(Serial.available() <= 0)
      {
        delay(1);
        waitCount++;
        if(waitCount > 5000)
          {
            waitCount = 0;
            Serial.println('@');
          }
      }
      
    while(stringComplete == false)
      {
        while (Serial.available()) 
          {
            // get the new byte:
            char inChar = (char)Serial.read();
            // add it to the inputString:
            inputString += inChar;
            // if the incoming character is a newline, set a flag so the main loop can
            // do something about it:
            if (inChar == '\n') 
              {
                stringComplete = true;
              }
          }
      }
    
    id = inputString.substring(inputString.lastIndexOf('$')+1, inputString.lastIndexOf('%'));
    pass = inputString.substring(inputString.lastIndexOf('%')+1, inputString.lastIndexOf('^'));
    gWay = inputString.substring(inputString.lastIndexOf('^')+1, inputString.lastIndexOf('&'));
    sMask = inputString.substring(inputString.lastIndexOf('&')+1, inputString.lastIndexOf('*'));
    plc_ip = inputString.substring(inputString.lastIndexOf('*')+1, inputString.lastIndexOf('('));
    dbNum = inputString.substring(inputString.lastIndexOf('(')+1, inputString.lastIndexOf(')')).toInt();
    local_ip = inputString.substring(inputString.lastIndexOf('-')+1, inputString.lastIndexOf('+'));
    
    stringComplete = false;
    inputString = "";
    digitalWrite(Arduino_Error_LED, LOW);
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
