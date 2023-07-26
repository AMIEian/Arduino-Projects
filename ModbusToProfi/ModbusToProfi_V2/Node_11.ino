void node11_Init()
{
  node11.begin(12, Serial2);
  node11.preTransmission(preTrans);
  node11.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node11_Read()
{
  uint8_t j;
  
  result = node11.readHoldingRegisters(0, 24);
  
  if (result == node11.ku8MBSuccess)
  {
    SerialBT.println("Node - 12");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node11.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node11_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node11.setTransmitBuffer(0, plcData[0]);
  node11.setTransmitBuffer(1, plcData[1]);
  node11.setTransmitBuffer(2, plcData[2]);
  
  node11.setTransmitBuffer(3, plcData[3]);
  node11.setTransmitBuffer(4, plcData[4]);
  node11.setTransmitBuffer(5, plcData[5]);

  node11.setTransmitBuffer(6, plcData[6]);
  node11.setTransmitBuffer(7, plcData[7]);
  node11.setTransmitBuffer(8, plcData[8]);

  node11.setTransmitBuffer(9, plcData[9]);
  node11.setTransmitBuffer(10, plcData[10]);
  node11.setTransmitBuffer(11, plcData[11]);

  node11.setTransmitBuffer(12, plcData[12]);
  node11.setTransmitBuffer(13, plcData[13]);
  node11.setTransmitBuffer(14, plcData[14]);

  result = node11.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
