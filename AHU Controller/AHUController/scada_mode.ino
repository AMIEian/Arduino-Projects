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
int16_t dbNum = 257;
String local_ip = "192.168.0.155";

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress Local; // Local Address
IPAddress PLC;   // PLC Address
IPAddress Gateway;
IPAddress Subnet;

boolean dataSent = false;

byte Buffer[1024];

unsigned long Elapsed; // To calc the execution time

unsigned long lastTime = 0; 

int16_t invert = 0;

int16_t conFail = 0;

void(* resetFunc) (void) = 0;

int16_t attempt = 0;

void ahuScadaModeInit()
{
  changeAHUStatus(AHU_INIT);
  //INIT SCADA ARRAY
  SCADA_DATA[SCADA_ACK] = 0;
  SCADA_DATA[SCADA_AHU_STATUS] = AHU_OFF;
  SCADA_DATA[SCADA_WARNING] = 0;
  SCADA_DATA[SCADA_WARNING_COUNTS] = 0;
  
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
  changeAHUStatus(AHU_OFF);
}

bool connectToWiFi()
{
  char idbuff[25];
  id.toCharArray(idbuff, 25);
  char passbuff[25];
  pass.toCharArray(passbuff, 25);
  int16_t waitCount = 0;
  
  SerialBT.println("Connecting WiFi...");
  //WiFi.mode(WIFI_STA);
  WiFi.begin(idbuff, passbuff);  
  WiFi.config(Local, Gateway, Subnet);
  bool onOff = true;
  while(WiFi.status() != WL_CONNECTED && waitCount < 20) 
    {
      if(onOff == true)
        {
          onOff = false;
        }
      else
        {
          onOff = true;
        }
      SerialBT.print(".");
      delay(500);
      waitCount++;
    }
  if(waitCount < 20)
    {
      //Connected to WiFi within 10 secs.
      return true;
    }
  else
    {
      //Connected to WiFi within 10 secs.
      return false;
    }
}

bool connectToEth()
{  
  ethReset();
  Ethernet.init(5);
  Ethernet.begin(mac, Local, Gateway);
  SerialBT.println(Ethernet.localIP());
  delay(1000);
  return true;
}

void ethReset()
{
  digitalWrite(WizReset, LOW);
  delay(1000);
  digitalWrite(WizReset, HIGH);
  delay(1000);
}

//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------

bool Connect()
  {
      int16_t Result = -1;
      if(WiFiConnected)
        Result = ClientWiFi.ConnectTo(PLC, 0, 0); //PLC IP, Slot 0, Rack 0
      else if(EthConnected)
        Result = ClientEth.ConnectTo(PLC, 0, 0); //PLC IP, Slot 0, Rack 0
        
      digitalWrite(WiFi_LED, HIGH);
      SerialBT.print("Connecting to ");
      SerialBT.println(PLC);  
      
      if (Result==0) 
        {
          digitalWrite(WiFi_LED, LOW);
          SerialBT.print("Connected ! PDU Length = ");
          
          if(WiFiConnected)
            SerialBT.println(ClientWiFi.GetPDULength());
          else if(EthConnected)
            SerialBT.println(ClientEth.GetPDULength());
          
          conFail = 0;
        }
      else
        {
          SerialBT.print("Connection error 0x");
          SerialBT.println(Result, HEX);
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
void Dump(void *Buffer, int16_t Length)
  {
    int16_t i, cnt = 0;
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
void CheckError(int16_t ErrNo)
  {
    SerialBT.print("Error No. 0x");
    SerialBT.println(ErrNo, HEX);

    digitalWrite(WiFi_LED, HIGH);
    
    // Checks if it's a Severe Error => we need to disconnect
    if (ErrNo & 0x00FF)
      {
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

bool getWiFiSettings()
  {
    boolean stringComplete = false;
    String inputString = "";
    int16_t waitCount = 0;
    int16_t attempt = 0;
    Serial2.println('@');
    
    while(Serial2.available() <= 0)
      {
        delay(1);
        waitCount++;
        if(waitCount > 5000)
          {
            waitCount = 0;
            Serial2.println('@');
            attempt++;
            if(attempt == 2)
              return false;
          }
      }
      
    while(stringComplete == false)
      {
        while(Serial2.available()) 
          {
            // get the new byte:
            char inChar = (char)Serial2.read();
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
    return true;
  }

IPAddress stringToIP(String strIP)
  {
    byte Parts[4] = {0,0,0,0};
    int16_t Part = 0;
    for ( int16_t i=0; i<strIP.length(); i++ )
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

void getSCADA()
{
  //READ PLC AND GET WHICH TABLE IS UPDATED
  int16_t Result;
  void *Target;
  int16_t ReadSize=4;
  int16_t RddbNum = dbNum;  //1;
  Target = NULL;
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
    
  if (Result==0)
    {
      //Save these values to AHUSetting array
      pbyte buf;
      buf = pbyte(&PDU.DATA[0]);
      int16_t te;
      int16_t x = 0;
      int16_t scadaData[2];
      bool changed = false;
      
      te = buf[x] << 8;
      x = x + 1;
      te = te + buf[x];
      scadaData[0] = te;
      
      x = x + 1;
      
      te = buf[x] << 8;
      x = x + 1;
      te = te + buf[x];
      scadaData[1] = te;
      
      delay(150);
      
      //SerialBT.println(te);
      if(scadaData[1] != schedulerEnabled)
        {
          changed = true;
          schedulerEnabled = scadaData[1];
        }
      if(changed == true)
        {
          SerialBT.println("Scheduler setting changed...");
          myNextion.setComponentValue(SchedulerEnabled, schedulerEnabled);
          delay(50);
          getSchedularValues();
        }
      if(scadaData[0] == 1)
        {
          setSCADA();
          SerialBT.println("Getting new Settings...");
          getUserSettingSCADA();
          getRATempSCADA();
          getRATempPIDSCADA();
          getRAHumSCADA();
          getRAHumPIDSCADA();
          getVFDSCADA();
          getVFDPIDSCADA();
          getAVSSCADA();
          getCO2SCADA();
          getAHUSettingsSCADA();
        }
    }
  else
    CheckError(Result);
}

void getAHUSettingsSCADA()
{
  //READ VALUES FROM PLC AND STORE THEM TO ARRAY
  int16_t Result;
  void *Target;
  int16_t ReadSize=sizeof(AHUSetting);
  int16_t RddbNum = dbNum + 1;  //2;
  Target = NULL;
  // Connection
  SerialBT.print("Read ");SerialBT.print(ReadSize);SerialBT.print(" bytes from DB");SerialBT.println(dbNum);
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
    
  if (Result==0)
    {
      //Save these values to AHUSetting array
      pbyte buf;
      buf = pbyte(&PDU.DATA[0]);
      int16_t index = 0;
      int16_t te;
      int16_t x = 0;
      bool changed = false;
      //Fill Data into temp array
      while(x < sizeof(AHUSetting))
        {
          te = buf[x] << 8;
          x = x + 1;
          te = te + buf[x];
          if(te != AHUSetting[index])
            {
              changed = true;
              AHUSetting[index] = te;
            }
          SerialBT.print("AHUSetting["); SerialBT.print(index); SerialBT.print("] : ");
          SerialBT.println(AHUSetting[index]);
          x = x + 1;
          index = index + 1;
        }
        
      if(changed == true)
        {
          SerialBT.println("AHUSetting Changed...");
          if(AHUSetting[ahuStatus] == AHU_ON)
            {
              if(SCADA_DATA[SCADA_AHU_STATUS] == AHU_OFF)
                {
                  startAHU();
                }
            }
          else
            {
              if(SCADA_DATA[SCADA_AHU_STATUS] == AHU_ON)
                {
                  stopAHU();
                }
            }
          
          prefs.putBool("AHUSetting", true);
          prefs.putBytes("AHUSet", AHUSetting, sizeof(AHUSetting));
          delay(100);
          setAHUSettingsValues();
          /*
          int16_t arrayLength = sizeof(AHUSetting)/sizeof(int16_t);
          for(int16_t i = 0; i < arrayLength; i++)
            AHUSetting[i] = 99;
        
          prefs.getBytes("AHUSet", AHUSetting, sizeof(AHUSetting));
            
          for(int16_t i = 0; i < arrayLength; i++)
            SerialBT.println(AHUSetting[i]);
          */
        }
    }
  else
    CheckError(Result);
    
  delay(150);
}

void getRATempSCADA()
{
  //READ VALUES FROM PLC AND STORE THEM TO ARRAY
  int16_t Result;
  void *Target;
  int16_t ReadSize=sizeof(RATemp);
  int16_t RddbNum = dbNum + 2;  //3;
  Target = NULL;
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
    
  if (Result==0)
    {
      //Save these values to RATemp array
      pbyte buf;
      buf = pbyte(&PDU.DATA[0]);
      int16_t index = 0;
      int16_t te;
      int16_t x = 0;
      bool changed = false;
      //Fill Data into temp array
      while(x < sizeof(RATemp))
        {
          te = buf[x] << 8;
          x = x + 1;
          te = te + buf[x];
          if(te != RATemp[index])
            {
              changed = true;
              RATemp[index] = te;
            }
          SerialBT.print("RATemp["); SerialBT.print(index); SerialBT.print("] : ");
          SerialBT.println(RATemp[index]);
          x = x + 1;
          index = index + 1;
        }

      if(changed == true)
        {
          SerialBT.println("RATemp Setting Changed...");
          prefs.putBool("RAT", true);
          prefs.putBytes("RATemp", RATemp, sizeof(RATemp));
          delay(100);
          setRATempValues();
          /*
          int16_t arrayLength = sizeof(RATemp)/sizeof(int16_t);
          for(int16_t i = 0; i < arrayLength; i++)
            RATemp[i] = 99;
        
          prefs.getBytes("RATemp", RATemp, sizeof(RATemp));
            
          for(int16_t i = 0; i < arrayLength; i++)
            SerialBT.println(RATemp[i]);
          */
        }
    }
  else
    CheckError(Result);
    
  delay(150);
}

void getRATempPIDSCADA()
{
  //READ VALUES FROM PLC AND STORE THEM TO ARRAY
  int16_t Result;
  void *Target;
  int16_t ReadSize=sizeof(RATempPID);
  int16_t RddbNum = dbNum + 3;  //4;
  Target = NULL;
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
    
  if (Result==0)
    {
      //Save these values to RATempPID array
      pbyte buf;
      buf = pbyte(&PDU.DATA[0]);
      int16_t index = 0;
      int16_t te;
      int16_t x = 0;
      bool changed = false;
      //Fill Data into temp array
      while(x < sizeof(RATempPID))
        {
          te = buf[x] << 8;
          x = x + 1;
          te = te + buf[x];
          if(te != RATempPID[index])
            {
              changed = true;
              RATempPID[index] = te;
            }
          SerialBT.print("RATempPID["); SerialBT.print(index); SerialBT.print("] : ");
          SerialBT.println(RATempPID[index]);
          x = x + 1;
          index = index + 1;
        }
        
      if(changed == true)
        {
          SerialBT.println("RATempPID Setting Changed...");
          prefs.putBool("RATP", true);
          prefs.putBytes("RATPID", RATempPID, sizeof(RATempPID));
          delay(100);
          setRATempPIDValues();
          /*
          int16_t arrayLength = sizeof(RATempPID)/sizeof(int16_t);
          for(int16_t i = 0; i < arrayLength; i++)
            RATempPID[i] = 99;
        
          prefs.getBytes("RATPID", RATempPID, sizeof(RATempPID));
            
          for(int16_t i = 0; i < arrayLength; i++)
            SerialBT.println(RATempPID[i]);
          */
        }
    }
  else
    CheckError(Result);
    
  delay(150);
}

void getRAHumSCADA()
{
  //READ VALUES FROM PLC AND STORE THEM TO ARRAY
  int16_t Result;
  void *Target;
  int16_t ReadSize=sizeof(RAHum);
  int16_t RddbNum = dbNum + 4;  //5;
  Target = NULL;
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
    
  if (Result==0)
    {
      //Save these values to RAHum array
      pbyte buf;
      buf = pbyte(&PDU.DATA[0]);
      int16_t index = 0;
      int16_t te;
      int16_t x = 0;
      bool changed = false;
      //Fill Data into temp array
      while(x < sizeof(RAHum))
        {
          te = buf[x] << 8;
          x = x + 1;
          te = te + buf[x];
          if(te != RAHum[index])
            {
              changed = true;
              RAHum[index] = te;
            }
          SerialBT.print("RAHum["); SerialBT.print(index); SerialBT.print("] : ");
          SerialBT.println(RAHum[index]);
          x = x + 1;
          index = index + 1;
        }

      if(changed == true)
        {
          SerialBT.println("RAHum Setting Changed...");
          prefs.putBool("RAH", true);
          prefs.putBytes("RAHum", RAHum, sizeof(RAHum));
          delay(100);
          setRAHumValues();
          /*
          int16_t arrayLength = sizeof(RAHum)/sizeof(int16_t);
          for(int16_t i = 0; i < arrayLength; i++)
            RAHum[i] = 99;
        
          prefs.getBytes("RAHum", RAHum, sizeof(RAHum));
            
          for(int16_t i = 0; i < arrayLength; i++)
            SerialBT.println(RAHum[i]);
          */
        }
    }
  else
    CheckError(Result);
    
  delay(150);
}

void getRAHumPIDSCADA()
{
  //READ VALUES FROM PLC AND STORE THEM TO ARRAY
  int16_t Result;
  void *Target;
  int16_t ReadSize=sizeof(RAHumPID);
  int16_t RddbNum = dbNum + 5;  //6;
  Target = NULL;
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
    
  if (Result==0)
    {
      //Save these values to RAHumPID array
      pbyte buf;
      buf = pbyte(&PDU.DATA[0]);
      int16_t index = 0;
      int16_t te;
      int16_t x = 0;
      bool changed = false;
      //Fill Data into temp array
      while(x < sizeof(RAHumPID))
        {
          te = buf[x] << 8;
          x = x + 1;
          te = te + buf[x];
          if(te != RAHumPID[index])
            {
              changed = true;
              RAHumPID[index] = te;
            }
          SerialBT.print("RAHumPID["); SerialBT.print(index); SerialBT.print("] : ");
          SerialBT.println(RAHumPID[index]);
          x = x + 1;
          index = index + 1;
        }

      if(changed == true)
        {
          SerialBT.println("RAHumPID Setting Changed...");
          prefs.putBool("RAHP", true);
          prefs.putBytes("RAHPID", RAHumPID, sizeof(RAHumPID));
          delay(100);
          setRAHumPIDValues();
          /*
          int16_t arrayLength = sizeof(RAHumPID)/sizeof(int16_t);
          for(int16_t i = 0; i < arrayLength; i++)
            RAHumPID[i] = 99;
        
          prefs.getBytes("RAHPID", RAHumPID, sizeof(RAHumPID));
            
          for(int16_t i = 0; i < arrayLength; i++)
            SerialBT.println(RAHumPID[i]);
          */
        }
    }
  else
    CheckError(Result);
    
  delay(150);
}

void getVFDSCADA()
{
  //READ VALUES FROM PLC AND STORE THEM TO ARRAY
  int16_t Result;
  void *Target;
  int16_t ReadSize=sizeof(VFD);
  int16_t RddbNum = dbNum + 6;  //7;
  Target = NULL;
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
    
  if (Result==0)
    {
      //Save these values to VFD array
      pbyte buf;
      buf = pbyte(&PDU.DATA[0]);
      int16_t index = 0;
      int16_t te;
      int16_t x = 0;
      bool changed = false;
      //Fill Data into temp array
      while(x < sizeof(VFD))
        {
          te = buf[x] << 8;
          x = x + 1;
          te = te + buf[x];
          if(te != VFD[index])
            {
              changed = true;
              VFD[index] = te;
            }
          SerialBT.print("VFD["); SerialBT.print(index); SerialBT.print("] : ");
          SerialBT.println(VFD[index]);
          x = x + 1;
          index = index + 1;
        }

      if(changed == true)
        {
          SerialBT.println("VFD Setting Changed...");
          prefs.putBool("VFD", true);
          prefs.putBytes("VFDVal", VFD, sizeof(VFD));
          delay(100);
          setVFDValues();
          /*
          int16_t arrayLength = sizeof(VFD)/sizeof(int16_t);
          for(int16_t i = 0; i < arrayLength; i++)
            VFD[i] = 99;
        
          prefs.getBytes("VFDVal", VFD, sizeof(VFD));
            
          for(int16_t i = 0; i < arrayLength; i++)
            SerialBT.println(VFD[i]);
          */
        }
    }
  else
    CheckError(Result);
    
  delay(150);
}

void getVFDPIDSCADA()
{
  //READ VALUES FROM PLC AND STORE THEM TO ARRAY
  int16_t Result;
  void *Target;
  int16_t ReadSize=sizeof(VFDPID);
  int16_t RddbNum = dbNum + 7;  //8;
  Target = NULL;
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
    
  if (Result==0)
    {
      //Save these values to VFDPID array
      pbyte buf;
      buf = pbyte(&PDU.DATA[0]);
      int16_t index = 0;
      int16_t te;
      int16_t x = 0;
      bool changed = false;
      //Fill Data into temp array
      while(x < sizeof(VFDPID))
        {
          te = buf[x] << 8;
          x = x + 1;
          te = te + buf[x];
          if(te != VFDPID[index])
            {
              changed = true;
              VFDPID[index] = te;
            }
          SerialBT.print("VFDPID["); SerialBT.print(index); SerialBT.print("] : ");
          SerialBT.println(VFDPID[index]);
          x = x + 1;
          index = index + 1;
        }

      if(changed == true)
        {
          SerialBT.println("VFDPID Setting Changed...");
          prefs.putBool("VFDP", true);
          prefs.putBytes("VFDPID", VFDPID, sizeof(VFDPID));
          delay(100);
          setVFDPIDValues();
          /*
          int16_t arrayLength = sizeof(VFDPID)/sizeof(int16_t);
          for(int16_t i = 0; i < arrayLength; i++)
            VFDPID[i] = 99;
        
          prefs.getBytes("VFDPID", VFDPID, sizeof(VFDPID));
            
          for(int16_t i = 0; i < arrayLength; i++)
            SerialBT.println(VFDPID[i]);
          */
        }
    }
  else
    CheckError(Result);
    
  delay(150);
}

void getAVSSCADA()
{
  //READ VALUES FROM PLC AND STORE THEM TO ARRAY
  int16_t Result;
  void *Target;
  int16_t ReadSize=sizeof(AVS);
  int16_t RddbNum = dbNum + 8;  //9;
  Target = NULL;
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
    
  if (Result==0)
    {
      //Save these values to AVS array
      pbyte buf;
      buf = pbyte(&PDU.DATA[0]);
      int16_t index = 0;
      int16_t te;
      int16_t x = 0;
      bool changed = false;
      //Fill Data into temp array
      while(x < sizeof(AVS))
        {
          te = buf[x] << 8;
          x = x + 1;
          te = te + buf[x];
          if(te != AVS[index])
            {
              changed = true;
              AVS[index] = te;
            }
          SerialBT.print("AVS["); SerialBT.print(index); SerialBT.print("] : ");
          SerialBT.println(AVS[index]);
          x = x + 1;
          index = index + 1;
        }

      if(changed == true)
        {
          SerialBT.println("AVS Setting Changed...");
          prefs.putBool("AVS", true);
          prefs.putBytes("AVSVal", AVS, sizeof(AVS));
          delay(100);
          setAVSValues();
          /*
          int16_t arrayLength = sizeof(AVS)/sizeof(int16_t);
          for(int16_t i = 0; i < arrayLength; i++)
            AVS[i] = 99;
        
          prefs.getBytes("AVSVal", AVS, sizeof(AVS));
            
          for(int16_t i = 0; i < arrayLength; i++)
            SerialBT.println(AVS[i]);
          */
        }
    }
  else
    CheckError(Result);
    
  delay(150);
}

void getCO2SCADA()
{
  //READ VALUES FROM PLC AND STORE THEM TO ARRAY
  int16_t Result;
  void *Target;
  int16_t ReadSize=sizeof(CO2);
  int16_t RddbNum = dbNum + 9;  //10;
  Target = NULL;
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
    
  if (Result==0)
    {
      //Save these values to CO2 array
      pbyte buf;
      buf = pbyte(&PDU.DATA[0]);
      int16_t index = 0;
      int16_t te;
      int16_t x = 0;
      bool changed = false;
      //Fill Data into temp array
      while(x < sizeof(CO2))
        {
          te = buf[x] << 8;
          x = x + 1;
          te = te + buf[x];
          if(te != CO2[index])
            {
              changed = true;
              CO2[index] = te;
            }
          SerialBT.print("CO2["); SerialBT.print(index); SerialBT.print("] : ");
          SerialBT.println(CO2[index]);
          x = x + 1;
          index = index + 1;
        }

      if(changed == true)
        {
          SerialBT.println("CO2 Setting Changed...");
          prefs.putBool("CO2", true);
          prefs.putBytes("CO2Val", CO2, sizeof(CO2));
          delay(100);
          setCO2Values();
          /*
          int16_t arrayLength = sizeof(CO2)/sizeof(int16_t);
          for(int16_t i = 0; i < arrayLength; i++)
            CO2[i] = 99;
        
          prefs.getBytes("CO2Val", CO2, sizeof(CO2));
            
          for(int16_t i = 0; i < arrayLength; i++)
            SerialBT.println(CO2[i]);
          */
        }
    }
  else
    CheckError(Result);
    
  delay(150);
}

void getUserSettingSCADA()
{
  //READ VALUES FROM PLC AND STORE THEM TO ARRAY
  int16_t Result;
  void *Target;
  int16_t ReadSize=sizeof(UserSetting);
  int16_t RddbNum = dbNum + 10; //11;
  Target = NULL;
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
    
  if (Result==0)
    {
      //Save these values to UserSetting array
      pbyte buf;
      buf = pbyte(&PDU.DATA[0]);
      int16_t index = 0;
      int16_t te;
      int16_t x = 0;
      bool changed = false;
      //Fill Data into temp array
      while(x < sizeof(UserSetting))
        {
          te = buf[x] << 8;
          x = x + 1;
          te = te + buf[x];
          if(te != UserSetting[index])
            {
              changed = true;
              UserSetting[index] = te;
            }
          SerialBT.print("UserSetting["); SerialBT.print(index); SerialBT.print("] : ");
          SerialBT.println(UserSetting[index]);
          x = x + 1;
          index = index + 1;
        }

      if(changed == true)
        {
          SerialBT.println("User Setting Changed...");           
          prefs.putBool("User", true);
          prefs.putBytes("USet", UserSetting, sizeof(UserSetting));
          delay(100);
          setUserSettingsValues();
          /*
          int16_t arrayLength = sizeof(UserSetting)/sizeof(int16_t);
          for(int16_t i = 0; i < arrayLength; i++)
            UserSetting[i] = 99;
        
          prefs.getBytes("USet", UserSetting, sizeof(UserSetting));
            
          for(int16_t i = 0; i < arrayLength; i++)
            SerialBT.println(UserSetting[i]);
          */
        }
    }
  else
    CheckError(Result);
    
  delay(150);
}

void getWarningSCADA()
{
  //READ PLC AND GET WHICH TABLE IS UPDATED
  int16_t Result;
  void *Target;
  int16_t ReadSize=2;
  int16_t RddbNum = dbNum + 13; //14;
  Target = NULL;
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.ReadArea(S7AreaDB, RddbNum, 0, ReadSize, Target);  // Put them into PDU
    }
    
  if (Result==0)
    {
      //Save these values to AHUSetting array
      pbyte buf;
      buf = pbyte(&PDU.DATA[0]);
      int16_t te;
      int16_t x = 0;
      te = buf[x] << 8;
      x = x + 1;
      te = te + buf[x];
      SerialBT.println(te);
      delay(150);
      SCADA_DATA[SCADA_ACK] = te;
    }
  else
    CheckError(Result);
    
  delay(150);
}

void setWarningSCADA()
{
  int16_t dataSize, Result;
  int16_t WrdbNum = dbNum + 13; //14;
  dataSize = sizeof(SCADA_DATA);
  
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.WriteArea(S7AreaDB, WrdbNum, 0, dataSize, SCADA_DATA); 
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.WriteArea(S7AreaDB, WrdbNum, 0, dataSize, SCADA_DATA); 
    }

  // We are requesting DB access with DB Number, Start from byte N.0, We need "Size" in bytes finally Put them into our target (Buffer or PDU)
  if (Result!=0)
    {
      CheckError(Result); 
    }

  delay(150);
}

void setSCADA()
{
  int16_t dataSize, Result;
  int16_t WrdbNum = dbNum;  //1;
  int16_t counts[1] = {0};
  dataSize = sizeof(counts);
  
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.WriteArea(S7AreaDB, WrdbNum, 0, dataSize, counts); 
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.WriteArea(S7AreaDB, WrdbNum, 0, dataSize, counts); 
    }

  // We are requesting DB access with DB Number, Start from byte N.0, We need "Size" in bytes finally Put them into our target (Buffer or PDU)
  if (Result!=0)
    {
      CheckError(Result); 
    }

  delay(150);
}

void setInputsSCADA()
{
  int16_t dataSize, Result;
  int16_t WrdbNum = dbNum + 11; //12;
  dataSize = sizeof(inputs);
  
  if(invert == 1)
    invert = 0;
  else
    invert = 1;

  inputs[20] = invert;

  inputs[19] = outputs[Chilled_Water_AO];

  inputs[18]  = 0;  //Can be used in future
    
  // Connection
  if(WiFiConnected)
    {
      attempt = 0;
      while (!ClientWiFi.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientWiFi.WriteArea(S7AreaDB, WrdbNum, 0, dataSize, inputs); 
    }
  else if(EthConnected)
    {
      attempt = 0;
      while (!ClientEth.Connected)
        {
          if (!Connect())
            {
              delay(5);
              attempt++;
              if(attempt > 20)
                return;
            }
        }
      Result=ClientEth.WriteArea(S7AreaDB, WrdbNum, 0, dataSize, inputs); 
    }

  // We are requesting DB access with DB Number, Start from byte N.0, We need "Size" in bytes finally Put them into our target (Buffer or PDU)
  if (Result!=0)
    {
      CheckError(Result); 
    }

  delay(150);
}
