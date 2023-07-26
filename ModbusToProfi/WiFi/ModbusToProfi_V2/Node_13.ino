void node13_Init()
{
  node13.begin(14, Serial2);
  node13.preTransmission(preTrans);
  node13.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node13_Read()
{
  uint8_t j;
  
  result = node13.readHoldingRegisters(0, 24);
  
  if (result == node13.ku8MBSuccess)
  {
    SerialBT.println("Node - 1");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node13.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node13_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node13.setTransmitBuffer(0, plcData[0]);
  node13.setTransmitBuffer(1, plcData[1]);
  node13.setTransmitBuffer(2, plcData[2]);
  
  node13.setTransmitBuffer(3, plcData[3]);
  node13.setTransmitBuffer(4, plcData[4]);
  node13.setTransmitBuffer(5, plcData[5]);

  node13.setTransmitBuffer(6, plcData[6]);
  node13.setTransmitBuffer(7, plcData[7]);
  node13.setTransmitBuffer(8, plcData[8]);

  node13.setTransmitBuffer(9, plcData[9]);
  node13.setTransmitBuffer(10, plcData[10]);
  node13.setTransmitBuffer(11, plcData[11]);

  node13.setTransmitBuffer(12, plcData[12]);
  node13.setTransmitBuffer(13, plcData[13]);
  node13.setTransmitBuffer(14, plcData[14]);

  result = node13.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
