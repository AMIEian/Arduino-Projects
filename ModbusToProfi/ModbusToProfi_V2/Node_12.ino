void node12_Init()
{
  node12.begin(13, Serial2);
  node12.preTransmission(preTrans);
  node12.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node12_Read()
{
  uint8_t j;
  
  result = node12.readHoldingRegisters(0, 24);
  
  if (result == node12.ku8MBSuccess)
  {
    SerialBT.println("Node - 13");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node12.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node12_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node12.setTransmitBuffer(0, plcData[0]);
  node12.setTransmitBuffer(1, plcData[1]);
  node12.setTransmitBuffer(2, plcData[2]);
  
  node12.setTransmitBuffer(3, plcData[3]);
  node12.setTransmitBuffer(4, plcData[4]);
  node12.setTransmitBuffer(5, plcData[5]);

  node12.setTransmitBuffer(6, plcData[6]);
  node12.setTransmitBuffer(7, plcData[7]);
  node12.setTransmitBuffer(8, plcData[8]);

  node12.setTransmitBuffer(9, plcData[9]);
  node12.setTransmitBuffer(10, plcData[10]);
  node12.setTransmitBuffer(11, plcData[11]);

  node12.setTransmitBuffer(12, plcData[12]);
  node12.setTransmitBuffer(13, plcData[13]);
  node12.setTransmitBuffer(14, plcData[14]);

  result = node12.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
