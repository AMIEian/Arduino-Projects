// Import required libraries
#include <SoftwareSerial.h>
#include <Nextion.h>
#include "Settimino.h"

// Uncomment next line to perform small and fast data access
#define DO_IT_SMALL

SoftwareSerial swSer(2, 3);

const int WiFi_Error_LED = A0;   //RED
const int Arduino_Error_LED = A1;  //BLUE
const int PLC_Error_LED = A2;   //GREEN

//Ethernet Settings
byte mac[] = { 0x00, 0x50, 0x56, 0xC0, 0x00, 0x06 };
String gWay = "192.168.0.1";
String sMask = "255.255.255.0";
String plc_ip = "192.168.0.70";
int dbNum = 1;
String local_ip = "192.168.0.72";

IPAddress Local; // Local Address
IPAddress PLC;   // PLC Address
IPAddress Gateway;
IPAddress Subnet;

String inputString = "";         // a String to hold incoming data

int16_t counts[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

boolean dataSent = false;

int8_t invert = 0;

byte Buffer[2];

// S7Client will create an EthernetClient as TCP Client
S7Client Client(_S7WIRED);

void(* resetFunc) (void) = 0;

//----------------------------------------------------------------------
// Setup : Init WiFi and Serial port
//----------------------------------------------------------------------

int temp[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int tempOld[11] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};

void setup() 
  {
    //wdt_disable();
    
    pinMode(WiFi_Error_LED, OUTPUT);
    pinMode(Arduino_Error_LED, OUTPUT);
    pinMode(PLC_Error_LED, OUTPUT);
    
    digitalWrite(WiFi_Error_LED, LOW);
    digitalWrite(Arduino_Error_LED, LOW);
    digitalWrite(PLC_Error_LED, LOW);

    for(int i = 0; i <= 1; i++)
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
    
    Serial.begin(115200);
    swSer.begin(9600);
    
    // reserve 200 bytes for the inputString:
    inputString.reserve(200);

    initModule();
    
    //Get WiFi settings from Arduino EEPROM
    //bool settingsReceived = false;
    //while(settingsReceived == false);
      //settingsReceived = getWiFiSettings();
      
    //Create IP Addresses from received data
    Local = stringToIP(local_ip); // Local Address
    Gateway = stringToIP(gWay);
    Subnet = stringToIP(sMask);
    PLC = stringToIP(plc_ip);
    
    Ethernet.begin(mac, Local, Gateway, Subnet);
  }

//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------

bool Connect()
  {
      int Result = Client.ConnectTo(PLC, 0, 0); //PLC IP, Slot 0, Rack 0
      /*
      Serial.print(F("Connecting to "));
      Serial.println(PLC);  
      */
      if (Result==0) 
        {
          Serial.print(F("Connected ! PDU Length = "));
          Serial.println(Client.GetPDULength());
          digitalWrite(WiFi_Error_LED, LOW);
        }
      else
        {
          digitalWrite(WiFi_Error_LED, HIGH);
          //resetFunc();
        }
      
      return Result == 0;
  }

//----------------------------------------------------------------------
// Dumps a buffer (a very rough routine)
//----------------------------------------------------------------------
/*
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
*/
//----------------------------------------------------------------------
// Prints the Error number
//----------------------------------------------------------------------
void CheckError(int ErrNo)
  {
    Serial.print(F("Error No. 0x"));
    Serial.println(ErrNo, HEX);
    
    // Checks if it's a Severe Error => we need to disconnect
    if (ErrNo & 0x00FF)
      {
        //Serial.println(F("SEVERE ERROR, disconnecting."));
        digitalWrite(PLC_Error_LED, HIGH);
        Client.Disconnect(); 
        //resetFunc();
      }
  }

//----------------------------------------------------------------------
// Time related routines
//----------------------------------------------------------------------
/*
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
*/
void loop() 
  {
    int Result;
    int8_t dataSize = sizeof(counts);
    
    // Connection
    while (!Client.Connected)
      {
        if (!Connect())
          delay(5);
      }

    Result=Client.WriteArea(S7AreaDB, dbNum, 0, dataSize, counts); 
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
        bool newData = false;
        while(newData == false)
          newData = getNewData();
        dataSent = false;
      }
      
    delay(150);
    
    //READ DO VALUES FROM PLC AND SEND THEM TO ARDUINO
    void *Target;
    int8_t ReadSize=22;
    int8_t RddbNum = dbNum + 1;
    Target = NULL;
    
    // Connection
    while (!Client.Connected)
      {
        if (!Connect())
          delay(5);
      }
    
    Result=Client.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    if (Result==0)
      {
        sendNewValues();
      }
    else
      CheckError(Result);
      
    delay(150);
  }

void CalibrationMode()
  {
    bool calMode = true;
    while(calMode == true)
      {
        while (swSer.available() > 0) 
          {
            Serial.write(swSer.read());
          }
        while (Serial.available() > 0) 
          {
            cli();
            swSer.write(Serial.read());
            sei();
          }
      }
  }

void initModule()
{
  cli();
  swSer.print('0');
  sei();
  for(int i = 0; i < 11; i++)
    {
      cli();
      swSer.print(',');
      swSer.print(temp[i]);
      sei();
      //Save sent value into old values array
      tempOld[i] = temp[i];
    }
  cli();
  swSer.print(',');
  swSer.println();
  sei();
}

void sendNewValues()
  {
    pbyte buf;
    buf = pbyte(&PDU.DATA[0]);
    int8_t index = 0;
    int te;
    int8_t x = 0;
    //Fill Data into temp array
    while(x < 22)
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
    int8_t i;
    for(i = 0; i < 11; i++)
      {
        if(temp[i] != tempOld[i])
          {
            sendValues = true;
            break;
          }
      }
    if(sendValues == true)
      {
        cli();
        swSer.print('0');
        sei();
        for(i = 0; i < 11; i++)
          {
            cli();
            swSer.print(',');
            swSer.print(temp[i]);
            sei();
            //Save sent value into old values array
            tempOld[i] = temp[i];
          }
        cli();
        swSer.print(',');
        swSer.println();
        sei();
      }
  }

bool getNewData() 
  {
    boolean stringComplete = false;
    inputString = "";
    int waitCount = 0;

    cli();
    swSer.print('#');
    sei();
    digitalWrite(Arduino_Error_LED, HIGH);
    
    while(swSer.available() <= 0)
      {
        delay(1);
        waitCount++;
        if(waitCount > 5000)
          {
            waitCount = 0;
            cli();
            swSer.print('#');
            sei();
          }
      }

    waitCount = 0;
    
    while(stringComplete == false)
      {    
        while(swSer.available()) 
          {
            // get the new byte:
            char inChar = (char)swSer.read();
            inputString += inChar;
            if (inChar == '\n' || inChar == '\r') 
              stringComplete = true;
          }
        delay(1);
        waitCount++;
        if(waitCount > 1000)
          return false;
      }
    
    String valueStr = "";
    int value = 0;
    int8_t countIndex = 0;
    
    for(int8_t i = 1; i < inputString.length(); i++)
      {
        if(inputString.charAt(i) == ' ' || inputString.charAt(i) == '\n')
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

    counts[20] = invert++;
    stringComplete = false;
    inputString = "";
    digitalWrite(Arduino_Error_LED, LOW);
    return true;
  }

bool getWiFiSettings()
  {
    boolean stringComplete = false;
    inputString = "";
    int waitCount = 0;

    cli();
    swSer.println('@');
    sei();
    digitalWrite(Arduino_Error_LED, HIGH);
    
    while(swSer.available() <= 0)
      {
        delay(1);
        waitCount++;
        if(waitCount > 5000)
          {
            waitCount = 0;
            cli();
            swSer.println('@');
            sei();
          }
      }
      
    while(stringComplete == false)
      {
        while(swSer.available()) 
          {
            // get the new byte:
            char inChar = (char)swSer.read();
            inputString += inChar;
            if (inChar == '\n') 
                stringComplete = true;
          }
          delay(1);
          waitCount++;
          if(waitCount > 1000)
            return false;
      }
    
    gWay = inputString.substring(inputString.lastIndexOf('^')+1, inputString.lastIndexOf('&'));
    sMask = inputString.substring(inputString.lastIndexOf('&')+1, inputString.lastIndexOf('*'));
    plc_ip = inputString.substring(inputString.lastIndexOf('*')+1, inputString.lastIndexOf('('));
    dbNum = inputString.substring(inputString.lastIndexOf('(')+1, inputString.lastIndexOf(')')).toInt();
    local_ip = inputString.substring(inputString.lastIndexOf('-')+1, inputString.lastIndexOf('+'));
    
    stringComplete = false;
    inputString = "";
    digitalWrite(Arduino_Error_LED, LOW);
    return true;
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
