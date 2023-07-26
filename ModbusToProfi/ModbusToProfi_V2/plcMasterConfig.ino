void AssembleConfigArray()
{
  pbyte buf;
  buf = pbyte(&PDU.DATA[0]);
  int x = 0;
  while(x < 6)
    {
      configData[x] = buf[x];
      SerialBT.print(configData[x]);
      SerialBT.print(" ");
      x = x + 1;
    }
  SerialBT.println("--------------------");
}

void getConfig()
{
  Serial.println("Reading Configurations From PLC");
  digitalWrite(com_ok, HIGH);
  //Get new data from PLC
  void *Target;
  Target = NULL;
  // Connection
  while (!Client.Connected)
    {
      if (!Connect())
        delay(5);
    }
  result=Client.ReadArea(S7AreaDB, // We are requesting DB access
                         configDB,        // DB Number = 1
                         0,        // Start from byte N.0
                         configDBSize,     // We need "Size" bytes
                         Target);  // Put them into our target (Buffer or PDU)
  
  //result = 0;
  if (result==0)
    {
      AssembleConfigArray();
      digitalWrite(com_ok, LOW);
    }
  else
    CheckError(result);
}

void setConfig()
{
  digitalWrite(com_ok, HIGH);
  SerialBT.println("Setting config to PLC");
    
  //Send Data to PLC
  //Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(5);
  }
  result=Client.WriteArea(S7AreaDB, // We are requesting DB access
                         configDB,        // DB Number = 1
                         0,        // Start from byte N.0
                         configDBSize,     // We need "Size" bytes
                         configData);  // Put them into our target (Buffer or PDU)

  digitalWrite(com_ok, LOW);
  SerialBT.println("Data Sent.");
}

void sendDisplayDataToPLC()
{
  digitalWrite(com_ok, HIGH);
  SerialBT.println("Sending Data to PLC");
    
  //Send Data to PLC
  //Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(5);
  }
  result=Client.WriteArea(S7AreaDB, // We are requesting DB access
                         writeDB,        // DB Number = 1
                         0,        // Start from byte N.0
                         writeDBSize,     // We need "Size" bytes
                         displayData);  // Put them into our target (Buffer or PDU)

  digitalWrite(com_ok, LOW);
  SerialBT.println("Data Sent.");
}

void getDisplayDataFromPLC()
{
  SerialBT.println("Reading Data From PLC");
  digitalWrite(com_ok, HIGH);
  //Get new data from PLC
  void *Target;
  Target = NULL;
  // Connection
  while (!Client.Connected)
    {
      if (!Connect())
        delay(5);
    }
  result=Client.ReadArea(S7AreaDB, // We are requesting DB access
                         readDB,        // DB Number = 1
                         0,        // Start from byte N.0
                         readDBSize,     // We need "Size" bytes
                         Target);  // Put them into our target (Buffer or PDU)
  
  //result = 0;
  if (result==0)    
    AssembleArray();
  else
    CheckError(result);
}

void updateReadErrDB()
{
  digitalWrite(com_ok, HIGH);
  SerialBT.println("Sending Data to PLC");
    
  //Send Data to PLC
  //Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(5);
  }
  result=Client.WriteArea(S7AreaDB, // We are requesting DB access
                         readErrDB,        // DB Number = 1
                         0,        // Start from byte N.0
                         readErrDBSize,     // We need "Size" bytes
                         readErrData);  // Put them into our target (Buffer or PDU)

  digitalWrite(com_ok, LOW);
  SerialBT.println("Data Sent.");
}

void updateWriteErrDB()
{
  digitalWrite(com_ok, HIGH);
  SerialBT.println("Sending Data to PLC");
    
  //Send Data to PLC
  //Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(5);
  }
  result=Client.WriteArea(S7AreaDB, // We are requesting DB access
                         writeErrDB,        // DB Number = 1
                         0,        // Start from byte N.0
                         writeErrDBSize,     // We need "Size" bytes
                         writeErrData);  // Put them into our target (Buffer or PDU)

  digitalWrite(com_ok, LOW);
  SerialBT.println("Data Sent.");
}

void AssembleArray()
{
  pbyte buf;
  buf = pbyte(&PDU.DATA[0]);
  int index = 0;
  int te;
  int x = 0;
  //Fill Data into plcData array
  while(x < 32)
    {
      te = buf[x] << 8;
      x = x + 1;
      te = te + buf[x];
      x = x + 1;
      plcData[index] = te;
      index = index + 1;
    }
}
