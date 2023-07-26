
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
        digitalWrite(com_err, LOW);
        Serial.print("Connected ! PDU Length = ");Serial.println(Client.GetPDULength());
      }
    else
      {
        digitalWrite(com_err, HIGH);
        Serial.print("Connection error 0x");
        Serial.println(Result, HEX);  
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
 
  Serial.print("[ Dumping ");Serial.print(Length);
  Serial.println(" bytes ]===========================");
  for (i=0; i<Length; i++)
  {
    cnt++;
    if (buf[i]<10)
      Serial.print("0");
    Serial.print(buf[i]);
    Serial.print(" ");
    if (cnt==16)
    {
      cnt=0;
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
  digitalWrite(com_err, HIGH);
  Serial.print("Error No. 0x");
  Serial.println(ErrNo, HEX);
  
  // Checks if it's a Severe Error => we need to disconnect
  if (ErrNo & 0x00FF)
  {
    Serial.println("SEVERE ERROR, disconnecting.");
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
  Serial.print("Job time (ms) : ");
  Serial.println(Elapsed);
  if(Elapsed > lastTime)
    {
      Serial.println(Elapsed);
      lastTime = Elapsed;
    }
}
