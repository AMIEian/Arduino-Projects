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
          digitalWrite(WiFi_Error_LED, LOW);
          conFail = 0;
        }
      else
        {
          SerialBT.print("Connection error 0x");
          SerialBT.println(Result, HEX);
          digitalWrite(WiFi_Error_LED, HIGH);
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
        //digitalWrite(PLC_Error_LED, HIGH);
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
