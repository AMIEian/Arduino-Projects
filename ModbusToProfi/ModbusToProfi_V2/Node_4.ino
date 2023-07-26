void node4_Init()
{
  node4.begin(5, Serial2);
  node4.preTransmission(preTrans);
  node4.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node4_Read()
{
  uint8_t j;
  
  result = node4.readHoldingRegisters(0, 24);
  
  if (result == node4.ku8MBSuccess)
  {
    SerialBT.println("Node - 5");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node4.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node4_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node4.setTransmitBuffer(0, plcData[0]);
  node4.setTransmitBuffer(1, plcData[1]);
  node4.setTransmitBuffer(2, plcData[2]);
  
  node4.setTransmitBuffer(3, plcData[3]);
  node4.setTransmitBuffer(4, plcData[4]);
  node4.setTransmitBuffer(5, plcData[5]);

  node4.setTransmitBuffer(6, plcData[6]);
  node4.setTransmitBuffer(7, plcData[7]);
  node4.setTransmitBuffer(8, plcData[8]);

  node4.setTransmitBuffer(9, plcData[9]);
  node4.setTransmitBuffer(10, plcData[10]);
  node4.setTransmitBuffer(11, plcData[11]);

  node4.setTransmitBuffer(12, plcData[12]);
  node4.setTransmitBuffer(13, plcData[13]);
  node4.setTransmitBuffer(14, plcData[14]);

  result = node4.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
