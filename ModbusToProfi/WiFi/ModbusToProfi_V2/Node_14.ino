void node14_Init()
{
  node14.begin(15, Serial2);
  node14.preTransmission(preTrans);
  node14.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node14_Read()
{
  uint8_t j;
  
  result = node14.readHoldingRegisters(0, 24);
  
  if (result == node14.ku8MBSuccess)
  {
    SerialBT.println("Node - 1");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node14.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node14_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node14.setTransmitBuffer(0, plcData[0]);
  node14.setTransmitBuffer(1, plcData[1]);
  node14.setTransmitBuffer(2, plcData[2]);
  
  node14.setTransmitBuffer(3, plcData[3]);
  node14.setTransmitBuffer(4, plcData[4]);
  node14.setTransmitBuffer(5, plcData[5]);

  node14.setTransmitBuffer(6, plcData[6]);
  node14.setTransmitBuffer(7, plcData[7]);
  node14.setTransmitBuffer(8, plcData[8]);

  node14.setTransmitBuffer(9, plcData[9]);
  node14.setTransmitBuffer(10, plcData[10]);
  node14.setTransmitBuffer(11, plcData[11]);

  node14.setTransmitBuffer(12, plcData[12]);
  node14.setTransmitBuffer(13, plcData[13]);
  node14.setTransmitBuffer(14, plcData[14]);

  result = node14.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
