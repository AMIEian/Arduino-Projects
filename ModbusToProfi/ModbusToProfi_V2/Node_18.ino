void node18_Init()
{
  node18.begin(19, Serial2);
  node18.preTransmission(preTrans);
  node18.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node18_Read()
{
  uint8_t j;
  
  result = node18.readHoldingRegisters(0, 24);
  
  if (result == node18.ku8MBSuccess)
  {
    SerialBT.println("Node - 19");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node18.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node18_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node18.setTransmitBuffer(0, plcData[0]);
  node18.setTransmitBuffer(1, plcData[1]);
  node18.setTransmitBuffer(2, plcData[2]);
  
  node18.setTransmitBuffer(3, plcData[3]);
  node18.setTransmitBuffer(4, plcData[4]);
  node18.setTransmitBuffer(5, plcData[5]);

  node18.setTransmitBuffer(6, plcData[6]);
  node18.setTransmitBuffer(7, plcData[7]);
  node18.setTransmitBuffer(8, plcData[8]);

  node18.setTransmitBuffer(9, plcData[9]);
  node18.setTransmitBuffer(10, plcData[10]);
  node18.setTransmitBuffer(11, plcData[11]);

  node18.setTransmitBuffer(12, plcData[12]);
  node18.setTransmitBuffer(13, plcData[13]);
  node18.setTransmitBuffer(14, plcData[14]);

  result = node18.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
