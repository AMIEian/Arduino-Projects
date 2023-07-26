void node17_Init()
{
  node17.begin(18, Serial2);
  node17.preTransmission(preTrans);
  node17.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node17_Read()
{
  uint8_t j;
  
  result = node17.readHoldingRegisters(0, 24);
  
  if (result == node17.ku8MBSuccess)
  {
    SerialBT.println("Node - 18");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node17.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node17_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node17.setTransmitBuffer(0, plcData[0]);
  node17.setTransmitBuffer(1, plcData[1]);
  node17.setTransmitBuffer(2, plcData[2]);
  
  node17.setTransmitBuffer(3, plcData[3]);
  node17.setTransmitBuffer(4, plcData[4]);
  node17.setTransmitBuffer(5, plcData[5]);

  node17.setTransmitBuffer(6, plcData[6]);
  node17.setTransmitBuffer(7, plcData[7]);
  node17.setTransmitBuffer(8, plcData[8]);

  node17.setTransmitBuffer(9, plcData[9]);
  node17.setTransmitBuffer(10, plcData[10]);
  node17.setTransmitBuffer(11, plcData[11]);

  node17.setTransmitBuffer(12, plcData[12]);
  node17.setTransmitBuffer(13, plcData[13]);
  node17.setTransmitBuffer(14, plcData[14]);

  result = node17.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
