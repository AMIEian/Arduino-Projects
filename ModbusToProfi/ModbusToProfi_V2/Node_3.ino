void node3_Init()
{
  node3.begin(4, Serial2);
  node3.preTransmission(preTrans);
  node3.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node3_Read()
{
  uint8_t j;
  
  result = node3.readHoldingRegisters(0, 24);
  
  if (result == node3.ku8MBSuccess)
  {
    SerialBT.println("Node - 4");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node3.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node3_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node3.setTransmitBuffer(0, plcData[0]);
  node3.setTransmitBuffer(1, plcData[1]);
  node3.setTransmitBuffer(2, plcData[2]);
  
  node3.setTransmitBuffer(3, plcData[3]);
  node3.setTransmitBuffer(4, plcData[4]);
  node3.setTransmitBuffer(5, plcData[5]);

  node3.setTransmitBuffer(6, plcData[6]);
  node3.setTransmitBuffer(7, plcData[7]);
  node3.setTransmitBuffer(8, plcData[8]);

  node3.setTransmitBuffer(9, plcData[9]);
  node3.setTransmitBuffer(10, plcData[10]);
  node3.setTransmitBuffer(11, plcData[11]);

  node3.setTransmitBuffer(12, plcData[12]);
  node3.setTransmitBuffer(13, plcData[13]);
  node3.setTransmitBuffer(14, plcData[14]);

  result = node3.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
