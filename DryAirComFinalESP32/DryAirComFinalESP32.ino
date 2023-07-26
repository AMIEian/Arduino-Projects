#include "Settimino.h"
#include "BluetoothSerial.h"

#define swSer Serial2

BluetoothSerial SerialBT;

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

String inputString = "";         // a String to hold incoming data

int16_t counts[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

boolean dataSent = false;

byte Buffer[1024];

unsigned long Elapsed; // To calc the execution time

unsigned long lastTime = 0; 

int invert = 0;

int conFail = 0;

void(* resetFunc) (void) = 0;

//----------------------------------------------------------------------
// Setup : Init WiFi and Serial port
//----------------------------------------------------------------------

int temp[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int tempOld[11] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};

#define RXD2 16
#define TXD2 17

const int WizReset = 4;

void setup() 
  {
    //wdt_disable();
    pinMode(WizReset, OUTPUT);
    digitalWrite(WizReset, LOW);
    Serial.begin(115200);
    swSer.begin(9600, SERIAL_8N1, RXD2, TXD2);
    SerialBT.begin("ESP32test"); //Bluetooth device name
    
    swSer.println("^0"); //digitalWrite(WiFi_Error_LED, LOW);
    swSer.println("~0"); //digitalWrite(PLC_Error_LED, LOW);
    
    for(int i = 0; i <= 2; i++)
      {
        swSer.println("^1"); //digitalWrite(WiFi_Error_LED, HIGH);
        swSer.println("~1"); //digitalWrite(PLC_Error_LED, HIGH);
        delay(1000);
        swSer.println("^0"); //digitalWrite(WiFi_Error_LED, LOW);
        swSer.println("~0"); //digitalWrite(PLC_Error_LED, LOW);
        delay(1000);
      }

    delay(1000);
    
    // reserve 200 bytes for the inputString:
    inputString.reserve(200);

    initModule();
    
    //Get WiFi settings from Arduino EEPROM
    bool settingsReceived = false;
    //while(settingsReceived == false)
    settingsReceived = getWiFiSettings();
    
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

//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------

bool Connect()
  {
      int Result = -1;
      if(WiFiConnected)
        Result = ClientWiFi.ConnectTo(PLC, 0, 0); //PLC IP, Slot 0, Rack 0
      else if(EthConnected)
        Result = ClientEth.ConnectTo(PLC, 0, 0); //PLC IP, Slot 0, Rack 0
        
      SerialBT.print("Connecting to ");
      SerialBT.println(PLC);  
      
      if (Result==0) 
        {
          SerialBT.print("Connected ! PDU Length = ");
          
          if(WiFiConnected)
            SerialBT.println(ClientWiFi.GetPDULength());
          else if(EthConnected)
            SerialBT.println(ClientEth.GetPDULength());
          
          swSer.println("^0"); //digitalWrite(WiFi_Error_LED, LOW);
          conFail = 0;
        }
      else
        {
          SerialBT.print("Connection error 0x");
          SerialBT.println(Result, HEX);
          swSer.println("^1"); //digitalWrite(WiFi_Error_LED, HIGH);
          if(WiFiConnected)
            {
              ClientWiFi.Disconnect();
              delay(500);
              connectToWiFi();
            }
          else if(EthConnected)
            {
              ClientEth.Disconnect();
              //delay(500);
              //connectToEth();
            }
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
  
    SerialBT.print("[ Dumping ");SerialBT.print(Length);
    SerialBT.println(" bytes ]===========================");
    for (i = 0; i < Length; i++)
      {
        cnt++;
        if (buf[i] < 10)
          SerialBT.print("0");
        SerialBT.print(buf[i]);
        SerialBT.print(" ");
        if (cnt == 16)
          {
            cnt = 0;
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
    SerialBT.print("Error No. 0x");
    SerialBT.println(ErrNo, HEX);
    
    // Checks if it's a Severe Error => we need to disconnect
    if (ErrNo & 0x00FF)
      {
        swSer.println("~1"); //digitalWrite(PLC_Error_LED, HIGH);
        SerialBT.println("SEVERE ERROR, disconnecting.");
        if(WiFiConnected)
            {
              ClientWiFi.Disconnect();
            }
          else if(EthConnected)
            {
              ClientEth.Disconnect();
            }
      }
    else
      {
        ESP.restart();
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
    SerialBT.print("Job time (ms) : ");
    SerialBT.println(Elapsed);
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
        swSer.println("~0"); //digitalWrite(PLC_Error_LED, LOW);
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
      
    delay(1000);
    
    //READ DO VALUES FROM PLC AND SEND THEM TO ARDUINO
    void *Target;
    int ReadSize=22;
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
        Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
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
        Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
      }
      
    if (Result==0)
      {
        sendNewValues();
        //Dump(Target, ReadSize);
      }
    else
      CheckError(Result);
      
    delay(1000);

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

void CalibrationMode()
  {
    bool calMode = true;
    swSer.println("^1"); //digitalWrite(WiFi_Error_LED, HIGH);
    swSer.println("~1"); //digitalWrite(PLC_Error_LED, HIGH);
    //Serial.print("Calibration Mode...!");
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

bool connectToWiFi()
{
  char idbuff[25];
  id.toCharArray(idbuff, 25);
  char passbuff[25];
  pass.toCharArray(passbuff, 25);
  int waitCount = 0;
  
  SerialBT.println("Connecting WiFi...");
  swSer.println("^1"); //digitalWrite(WiFi_Error_LED, HIGH);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(idbuff, passbuff);  
  WiFi.config(Local, Gateway, Subnet);
  bool onOff = true;
  while(WiFi.status() != WL_CONNECTED && waitCount < 20) 
    {
      if(onOff == true)
        {
          onOff = false;
          swSer.println("^1"); //digitalWrite(WiFi_Error_LED, HIGH);
        }
      else
        {
          onOff = true;
          swSer.println("^0"); //digitalWrite(WiFi_Error_LED, LOW);
        }
      SerialBT.print(".");
      delay(500);
      waitCount++;
    }
  if(waitCount < 20)
    {
      //Connected to WiFi within 10 secs.
      swSer.println("^0"); //digitalWrite(WiFi_Error_LED, LOW);
      return true;
    }
  else
    {
      //Connected to WiFi within 10 secs.
      swSer.println("^1"); //digitalWrite(WiFi_Error_LED, HIGH);
      return false;
    }
}

bool connectToEth()
{  
  swSer.println("^1"); //digitalWrite(WiFi_Error_LED, HIGH);
  ethReset();
  Ethernet.init(5);
  Ethernet.begin(mac, Local, Gateway);
  SerialBT.println(Ethernet.localIP());
  delay(1000);
  swSer.println("^0"); //digitalWrite(WiFi_Error_LED, LOW);
  return true;
}

void ethReset()
{
  digitalWrite(WizReset, LOW);
  delay(1000);
  digitalWrite(WizReset, HIGH);
  delay(1000);
}

void sendNewValues()
  {
    pbyte buf;
    char DOValue;
    buf = pbyte(&PDU.DATA[0]);
    int index = 0;
    int te;
    int x = 0;
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
    int i;
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
    //digitalWrite(Arduino_Error_LED, HIGH);
    
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
    int countIndex = 0;
    
    for(int i = 1; i < inputString.length(); i++)
      {
        if(inputString.charAt(i) == ' ' || inputString.charAt(i) == '\n')
          {
            value = valueStr.toInt();
            counts[countIndex] = value;
            SerialBT.print(counts[countIndex]);
            SerialBT.print(" ");
            countIndex++;
            valueStr = "";
            value = 0;  
          }
        else
          valueStr = valueStr + inputString.charAt(i);
      }
    SerialBT.println();
    counts[20] = invert++;
    stringComplete = false;
    inputString = "";
    //digitalWrite(Arduino_Error_LED, LOW);
    return true;
  }

bool getWiFiSettings()
  {
    boolean stringComplete = false;
    inputString = "";
    int waitCount = 0;
    int attempt = 0;
    cli();
    swSer.println('@');
    //Serial.println('@');
    sei();
    //digitalWrite(Arduino_Error_LED, HIGH);
    
    while(swSer.available() <= 0)
      {
        delay(1);
        waitCount++;
        if(waitCount > 5000)
          {
            waitCount = 0;
            cli();
            swSer.println('@');
            attempt++;
            if(attempt == 2)
              return false;
            //Serial.println('@');
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
    
    id = inputString.substring(inputString.lastIndexOf('$')+1, inputString.lastIndexOf('%'));
    pass = inputString.substring(inputString.lastIndexOf('%')+1, inputString.lastIndexOf('^'));
    gWay = inputString.substring(inputString.lastIndexOf('^')+1, inputString.lastIndexOf('&'));
    sMask = inputString.substring(inputString.lastIndexOf('&')+1, inputString.lastIndexOf('*'));
    plc_ip = inputString.substring(inputString.lastIndexOf('*')+1, inputString.lastIndexOf('('));
    dbNum = inputString.substring(inputString.lastIndexOf('(')+1, inputString.lastIndexOf(')')).toInt();
    local_ip = inputString.substring(inputString.lastIndexOf('-')+1, inputString.lastIndexOf('+'));
    
    stringComplete = false;
    inputString = "";
    //digitalWrite(Arduino_Error_LED, LOW);
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
