void node0_Init()
{
  node0.begin(1, Serial2);
  node0.preTransmission(preTrans);
  node0.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node0_Read()
{
  uint8_t j;
  
  result = node0.readHoldingRegisters(0, 24);
  
  if (result == node0.ku8MBSuccess)
  {
    SerialBT.println("Node - 1");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node0.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node0_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node0.setTransmitBuffer(0, plcData[0]);
  node0.setTransmitBuffer(1, plcData[1]);
  node0.setTransmitBuffer(2, plcData[2]);
  
  node0.setTransmitBuffer(3, plcData[3]);
  node0.setTransmitBuffer(4, plcData[4]);
  node0.setTransmitBuffer(5, plcData[5]);

  node0.setTransmitBuffer(6, plcData[6]);
  node0.setTransmitBuffer(7, plcData[7]);
  node0.setTransmitBuffer(8, plcData[8]);

  node0.setTransmitBuffer(9, plcData[9]);
  node0.setTransmitBuffer(10, plcData[10]);
  node0.setTransmitBuffer(11, plcData[11]);

  node0.setTransmitBuffer(12, plcData[12]);
  node0.setTransmitBuffer(13, plcData[13]);
  node0.setTransmitBuffer(14, plcData[14]);

  result = node0.writeMultipleRegisters(0, 15);

  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
