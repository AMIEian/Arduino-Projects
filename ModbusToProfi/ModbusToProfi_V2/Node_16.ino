void node16_Init()
{
  node16.begin(17, Serial2);
  node16.preTransmission(preTrans);
  node16.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node16_Read()
{
  uint8_t j;
  
  result = node16.readHoldingRegisters(0, 24);
  
  if (result == node16.ku8MBSuccess)
  {
    SerialBT.println("Node - 17");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node16.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node16_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node16.setTransmitBuffer(0, plcData[0]);
  node16.setTransmitBuffer(1, plcData[1]);
  node16.setTransmitBuffer(2, plcData[2]);
  
  node16.setTransmitBuffer(3, plcData[3]);
  node16.setTransmitBuffer(4, plcData[4]);
  node16.setTransmitBuffer(5, plcData[5]);

  node16.setTransmitBuffer(6, plcData[6]);
  node16.setTransmitBuffer(7, plcData[7]);
  node16.setTransmitBuffer(8, plcData[8]);

  node16.setTransmitBuffer(9, plcData[9]);
  node16.setTransmitBuffer(10, plcData[10]);
  node16.setTransmitBuffer(11, plcData[11]);

  node16.setTransmitBuffer(12, plcData[12]);
  node16.setTransmitBuffer(13, plcData[13]);
  node16.setTransmitBuffer(14, plcData[14]);

  result = node16.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
