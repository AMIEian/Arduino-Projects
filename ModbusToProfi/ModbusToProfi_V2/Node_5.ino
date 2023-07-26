void node5_Init()
{
  node5.begin(6, Serial2);
  node5.preTransmission(preTrans);
  node5.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node5_Read()
{
  uint8_t j;
  
  result = node5.readHoldingRegisters(0, 24);
  
  if (result == node5.ku8MBSuccess)
  {
    SerialBT.println("Node - 6");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node5.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node5_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node5.setTransmitBuffer(0, plcData[0]);
  node5.setTransmitBuffer(1, plcData[1]);
  node5.setTransmitBuffer(2, plcData[2]);
  
  node5.setTransmitBuffer(3, plcData[3]);
  node5.setTransmitBuffer(4, plcData[4]);
  node5.setTransmitBuffer(5, plcData[5]);

  node5.setTransmitBuffer(6, plcData[6]);
  node5.setTransmitBuffer(7, plcData[7]);
  node5.setTransmitBuffer(8, plcData[8]);

  node5.setTransmitBuffer(9, plcData[9]);
  node5.setTransmitBuffer(10, plcData[10]);
  node5.setTransmitBuffer(11, plcData[11]);

  node5.setTransmitBuffer(12, plcData[12]);
  node5.setTransmitBuffer(13, plcData[13]);
  node5.setTransmitBuffer(14, plcData[14]);

  result = node5.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}