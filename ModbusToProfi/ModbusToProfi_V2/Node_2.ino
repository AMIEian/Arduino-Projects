void node2_Init()
{
  node2.begin(3, Serial2);
  node2.preTransmission(preTrans);
  node2.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node2_Read()
{
  uint8_t j;
  
  result = node2.readHoldingRegisters(0, 24);
  
  if (result == node2.ku8MBSuccess)
  {
    SerialBT.println("Node - 3");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node2.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node2_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node2.setTransmitBuffer(0, plcData[0]);
  node2.setTransmitBuffer(1, plcData[1]);
  node2.setTransmitBuffer(2, plcData[2]);
  
  node2.setTransmitBuffer(3, plcData[3]);
  node2.setTransmitBuffer(4, plcData[4]);
  node2.setTransmitBuffer(5, plcData[5]);

  node2.setTransmitBuffer(6, plcData[6]);
  node2.setTransmitBuffer(7, plcData[7]);
  node2.setTransmitBuffer(8, plcData[8]);

  node2.setTransmitBuffer(9, plcData[9]);
  node2.setTransmitBuffer(10, plcData[10]);
  node2.setTransmitBuffer(11, plcData[11]);

  node2.setTransmitBuffer(12, plcData[12]);
  node2.setTransmitBuffer(13, plcData[13]);
  node2.setTransmitBuffer(14, plcData[14]);

  result = node2.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
