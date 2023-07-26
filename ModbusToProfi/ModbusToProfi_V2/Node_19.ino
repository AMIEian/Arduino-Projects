void node19_Init()
{
  node19.begin(20, Serial2);
  node19.preTransmission(preTrans);
  node19.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node19_Read()
{
  uint8_t j;
  
  result = node19.readHoldingRegisters(0, 24);
  
  if (result == node19.ku8MBSuccess)
  {
    SerialBT.println("Node - 20");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node19.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node19_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node19.setTransmitBuffer(0, plcData[0]);
  node19.setTransmitBuffer(1, plcData[1]);
  node19.setTransmitBuffer(2, plcData[2]);
  
  node19.setTransmitBuffer(3, plcData[3]);
  node19.setTransmitBuffer(4, plcData[4]);
  node19.setTransmitBuffer(5, plcData[5]);

  node19.setTransmitBuffer(6, plcData[6]);
  node19.setTransmitBuffer(7, plcData[7]);
  node19.setTransmitBuffer(8, plcData[8]);

  node19.setTransmitBuffer(9, plcData[9]);
  node19.setTransmitBuffer(10, plcData[10]);
  node19.setTransmitBuffer(11, plcData[11]);

  node19.setTransmitBuffer(12, plcData[12]);
  node19.setTransmitBuffer(13, plcData[13]);
  node19.setTransmitBuffer(14, plcData[14]);

  result = node19.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
