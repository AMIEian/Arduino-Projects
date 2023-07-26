
//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------

bool Connect()
{
    //Client.SetConnectionType(0x03);
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
  SerialBT.print("Job time (ms) : ");
  SerialBT.println(Elapsed);
  if(Elapsed > lastTime)
    {
      SerialBT.println(Elapsed);
      lastTime = Elapsed;
    }
}
