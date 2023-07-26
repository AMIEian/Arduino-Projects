void node20_Init()
{
  node20.begin(21, Serial2);
  node20.preTransmission(preTrans);
  node20.postTransmission(postTrans);
}

void node20_Tasks()
{
  uint8_t j, result;
  uint16_t data[24];
  int DBNum = startingDB + 21;

  Serial.println("Reading Data from Display");
  digitalWrite(com_ok, HIGH);
  
  result = node20.readHoldingRegisters(0, 24);
  
  if (result == node20.ku8MBSuccess)
  {
    Serial.println("Node - 21");
    for (j = 0; j < 24; j++)
    {
      data[j] = node20.getResponseBuffer(j);
      Serial.print(data[j]);
      Serial.print(" ");
    }
    Serial.println("-------------------------------------");
    Serial.println();
    
    Serial.println("Sending Data to PLC");
    
    //Send Data to PLC
    //Connection
    while (!Client.Connected)
    {
      if (!Connect())
        delay(5);
    }
    result=Client.WriteArea(S7AreaDB, // We are requesting DB access
                           DBNum,        // DB Number = 1
                           0,        // Start from byte N.0
                           48,     // We need "Size" bytes
                           data);  // Put them into our target (Buffer or PDU)
  }

  digitalWrite(com_ok, LOW);
  delay(100);
  
  Serial.println("Data Sent. Reading Data From PLC");
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
                         DBNum,        // DB Number = 1
                         48,        // Start from byte N.0
                         32,     // We need "Size" bytes
                         Target);  // Put them into our target (Buffer or PDU)
  if (result==0)
    {
      //Dump(Target, 30);      
      AssembleArray();
      
      if(node_20_old != temp[15])
        {
          Serial.println("New Data Received. Writting Data to Display");
          
          node20.setTransmitBuffer(0, temp[0]);
          node20.setTransmitBuffer(1, temp[1]);
          node20.setTransmitBuffer(2, temp[2]);
          
          node20.setTransmitBuffer(3, temp[3]);
          node20.setTransmitBuffer(4, temp[4]);
          node20.setTransmitBuffer(5, temp[5]);
        
          node20.setTransmitBuffer(6, temp[6]);
          node20.setTransmitBuffer(7, temp[7]);
          node20.setTransmitBuffer(8, temp[8]);
        
          node20.setTransmitBuffer(9, temp[9]);
          node20.setTransmitBuffer(10, temp[10]);
          node20.setTransmitBuffer(11, temp[11]);
        
          node20.setTransmitBuffer(12, temp[12]);
          node20.setTransmitBuffer(13, temp[13]);
          node20.setTransmitBuffer(14, temp[14]);
        
          result = node20.writeMultipleRegisters(0, 15);

          delay(200);

          node_20_old = temp[15];
          
          Serial.println("Writting Display Complete");
          Serial.println("-------------------------------------");
          Serial.println();
          Serial.println();
          digitalWrite(com_ok, LOW);
        }
      else
        {
          Serial.println("No New Data Found.");
        }
      digitalWrite(com_ok, LOW);
    }
  else
    CheckError(result);
}
