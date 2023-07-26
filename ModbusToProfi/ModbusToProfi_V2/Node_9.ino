void node9_Init()
{
  node9.begin(10, Serial2);
  node9.preTransmission(preTrans);
  node9.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node9_Read()
{
  uint8_t j;
  
  result = node9.readHoldingRegisters(0, 24);
  
  if (result == node9.ku8MBSuccess)
  {
    SerialBT.println("Node - 10");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node9.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node9_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node9.setTransmitBuffer(0, plcData[0]);
  node9.setTransmitBuffer(1, plcData[1]);
  node9.setTransmitBuffer(2, plcData[2]);
  
  node9.setTransmitBuffer(3, plcData[3]);
  node9.setTransmitBuffer(4, plcData[4]);
  node9.setTransmitBuffer(5, plcData[5]);

  node9.setTransmitBuffer(6, plcData[6]);
  node9.setTransmitBuffer(7, plcData[7]);
  node9.setTransmitBuffer(8, plcData[8]);

  node9.setTransmitBuffer(9, plcData[9]);
  node9.setTransmitBuffer(10, plcData[10]);
  node9.setTransmitBuffer(11, plcData[11]);

  node9.setTransmitBuffer(12, plcData[12]);
  node9.setTransmitBuffer(13, plcData[13]);
  node9.setTransmitBuffer(14, plcData[14]);

  result = node9.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
