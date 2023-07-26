void node15_Init()
{
  node15.begin(16, Serial2);
  node15.preTransmission(preTrans);
  node15.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node15_Read()
{
  uint8_t j;
  
  result = node15.readHoldingRegisters(0, 24);
  
  if (result == node15.ku8MBSuccess)
  {
    SerialBT.println("Node - 1");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node15.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node15_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node15.setTransmitBuffer(0, plcData[0]);
  node15.setTransmitBuffer(1, plcData[1]);
  node15.setTransmitBuffer(2, plcData[2]);
  
  node15.setTransmitBuffer(3, plcData[3]);
  node15.setTransmitBuffer(4, plcData[4]);
  node15.setTransmitBuffer(5, plcData[5]);

  node15.setTransmitBuffer(6, plcData[6]);
  node15.setTransmitBuffer(7, plcData[7]);
  node15.setTransmitBuffer(8, plcData[8]);

  node15.setTransmitBuffer(9, plcData[9]);
  node15.setTransmitBuffer(10, plcData[10]);
  node15.setTransmitBuffer(11, plcData[11]);

  node15.setTransmitBuffer(12, plcData[12]);
  node15.setTransmitBuffer(13, plcData[13]);
  node15.setTransmitBuffer(14, plcData[14]);

  result = node15.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
