void node8_Init()
{
  node8.begin(9, Serial2);
  node8.preTransmission(preTrans);
  node8.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node8_Read()
{
  uint8_t j;
  
  result = node8.readHoldingRegisters(0, 24);
  
  if (result == node8.ku8MBSuccess)
  {
    SerialBT.println("Node - 1");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node8.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node8_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node8.setTransmitBuffer(0, plcData[0]);
  node8.setTransmitBuffer(1, plcData[1]);
  node8.setTransmitBuffer(2, plcData[2]);
  
  node8.setTransmitBuffer(3, plcData[3]);
  node8.setTransmitBuffer(4, plcData[4]);
  node8.setTransmitBuffer(5, plcData[5]);

  node8.setTransmitBuffer(6, plcData[6]);
  node8.setTransmitBuffer(7, plcData[7]);
  node8.setTransmitBuffer(8, plcData[8]);

  node8.setTransmitBuffer(9, plcData[9]);
  node8.setTransmitBuffer(10, plcData[10]);
  node8.setTransmitBuffer(11, plcData[11]);

  node8.setTransmitBuffer(12, plcData[12]);
  node8.setTransmitBuffer(13, plcData[13]);
  node8.setTransmitBuffer(14, plcData[14]);

  result = node8.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
