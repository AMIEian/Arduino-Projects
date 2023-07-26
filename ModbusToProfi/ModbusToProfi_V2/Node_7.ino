void node7_Init()
{
  node7.begin(8, Serial2);
  node7.preTransmission(preTrans);
  node7.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node7_Read()
{
  uint8_t j;
  
  result = node7.readHoldingRegisters(0, 24);
  
  if (result == node7.ku8MBSuccess)
  {
    SerialBT.println("Node - 8");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node7.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node7_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node7.setTransmitBuffer(0, plcData[0]);
  node7.setTransmitBuffer(1, plcData[1]);
  node7.setTransmitBuffer(2, plcData[2]);
  
  node7.setTransmitBuffer(3, plcData[3]);
  node7.setTransmitBuffer(4, plcData[4]);
  node7.setTransmitBuffer(5, plcData[5]);

  node7.setTransmitBuffer(6, plcData[6]);
  node7.setTransmitBuffer(7, plcData[7]);
  node7.setTransmitBuffer(8, plcData[8]);

  node7.setTransmitBuffer(9, plcData[9]);
  node7.setTransmitBuffer(10, plcData[10]);
  node7.setTransmitBuffer(11, plcData[11]);

  node7.setTransmitBuffer(12, plcData[12]);
  node7.setTransmitBuffer(13, plcData[13]);
  node7.setTransmitBuffer(14, plcData[14]);

  result = node7.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
