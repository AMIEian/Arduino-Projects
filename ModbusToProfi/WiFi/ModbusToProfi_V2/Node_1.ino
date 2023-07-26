void node1_Init()
{
  node1.begin(2, Serial2);
  node1.preTransmission(preTrans);
  node1.postTransmission(postTrans);
}
//Read Modbus data and send to PLC
void node1_Read()
{
  uint8_t j;
  
  result = node1.readHoldingRegisters(0, 24);
  
  if (result == node1.ku8MBSuccess)
  {
    SerialBT.println("Node - 1");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node1.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node1_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node1.setTransmitBuffer(0, plcData[0]);
  node1.setTransmitBuffer(1, plcData[1]);
  node1.setTransmitBuffer(2, plcData[2]);
  
  node1.setTransmitBuffer(3, plcData[3]);
  node1.setTransmitBuffer(4, plcData[4]);
  node1.setTransmitBuffer(5, plcData[5]);

  node1.setTransmitBuffer(6, plcData[6]);
  node1.setTransmitBuffer(7, plcData[7]);
  node1.setTransmitBuffer(8, plcData[8]);

  node1.setTransmitBuffer(9, plcData[9]);
  node1.setTransmitBuffer(10, plcData[10]);
  node1.setTransmitBuffer(11, plcData[11]);

  node1.setTransmitBuffer(12, plcData[12]);
  node1.setTransmitBuffer(13, plcData[13]);
  node1.setTransmitBuffer(14, plcData[14]);

  result = node1.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
