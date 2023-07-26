void node10_Init()
{
  node10.begin(11, Serial2);
  node10.preTransmission(preTrans);
  node10.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node10_Read()
{
  uint8_t j;
  
  result = node10.readHoldingRegisters(0, 24);
  
  if (result == node10.ku8MBSuccess)
  {
    SerialBT.println("Node - 1");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node10.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node10_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node10.setTransmitBuffer(0, plcData[0]);
  node10.setTransmitBuffer(1, plcData[1]);
  node10.setTransmitBuffer(2, plcData[2]);
  
  node10.setTransmitBuffer(3, plcData[3]);
  node10.setTransmitBuffer(4, plcData[4]);
  node10.setTransmitBuffer(5, plcData[5]);

  node10.setTransmitBuffer(6, plcData[6]);
  node10.setTransmitBuffer(7, plcData[7]);
  node10.setTransmitBuffer(8, plcData[8]);

  node10.setTransmitBuffer(9, plcData[9]);
  node10.setTransmitBuffer(10, plcData[10]);
  node10.setTransmitBuffer(11, plcData[11]);

  node10.setTransmitBuffer(12, plcData[12]);
  node10.setTransmitBuffer(13, plcData[13]);
  node10.setTransmitBuffer(14, plcData[14]);

  result = node10.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
