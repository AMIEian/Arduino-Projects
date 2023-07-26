void node18_Init()
{
  node18.begin(19, Serial2);
  node18.preTransmission(preTrans);
  node18.postTransmission(postTrans);
}

void node18_Tasks()
{
  uint8_t j, result;
  uint16_t data[24];
  int DBNum = startingDB + 19;

  Serial.println("Reading Data from Display");
  digitalWrite(com_ok, HIGH);
  
  result = node18.readHoldingRegisters(0, 24);
  
  if (result == node18.ku8MBSuccess)
  {
    Serial.println("Node - 19");
    for (j = 0; j < 24; j++)
    {
      data[j] = node18.getResponseBuffer(j);
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
      
      if(node_18_old != temp[15])
        {
          Serial.println("New Data Received. Writting Data to Display");
          
          node18.setTransmitBuffer(0, temp[0]);
          node18.setTransmitBuffer(1, temp[1]);
          node18.setTransmitBuffer(2, temp[2]);
          
          node18.setTransmitBuffer(3, temp[3]);
          node18.setTransmitBuffer(4, temp[4]);
          node18.setTransmitBuffer(5, temp[5]);
        
          node18.setTransmitBuffer(6, temp[6]);
          node18.setTransmitBuffer(7, temp[7]);
          node18.setTransmitBuffer(8, temp[8]);
        
          node18.setTransmitBuffer(9, temp[9]);
          node18.setTransmitBuffer(10, temp[10]);
          node18.setTransmitBuffer(11, temp[11]);
        
          node18.setTransmitBuffer(12, temp[12]);
          node18.setTransmitBuffer(13, temp[13]);
          node18.setTransmitBuffer(14, temp[14]);
        
          result = node18.writeMultipleRegisters(0, 15);

          delay(200);

          node_18_old = temp[15];
          
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
