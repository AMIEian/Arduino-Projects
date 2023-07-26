void node20_Init()
{
  node20.begin(21, Serial2);
  node20.preTransmission(preTrans);
  node20.postTransmission(postTrans);
}

//Read Modbus data and send to PLC
void node20_Read()
{
  uint8_t j;
  
  result = node20.readHoldingRegisters(0, 24);
  
  if (result == node20.ku8MBSuccess)
  {
    SerialBT.println("Node - 1");
    for (j = 0; j < 24; j++)
    {
      displayData[j] = node20.getResponseBuffer(j);
      SerialBT.print(displayData[j]);
      SerialBT.print(" ");
    }
  }
  SerialBT.println();
  SerialBT.println("-------------------------------------");
}

//Get data from PLC and write to modbus
void node20_Write()
{
  SerialBT.println("Writting Data to Display");
  
  node20.setTransmitBuffer(0, plcData[0]);
  node20.setTransmitBuffer(1, plcData[1]);
  node20.setTransmitBuffer(2, plcData[2]);
  
  node20.setTransmitBuffer(3, plcData[3]);
  node20.setTransmitBuffer(4, plcData[4]);
  node20.setTransmitBuffer(5, plcData[5]);

  node20.setTransmitBuffer(6, plcData[6]);
  node20.setTransmitBuffer(7, plcData[7]);
  node20.setTransmitBuffer(8, plcData[8]);

  node20.setTransmitBuffer(9, plcData[9]);
  node20.setTransmitBuffer(10, plcData[10]);
  node20.setTransmitBuffer(11, plcData[11]);

  node20.setTransmitBuffer(12, plcData[12]);
  node20.setTransmitBuffer(13, plcData[13]);
  node20.setTransmitBuffer(14, plcData[14]);

  result = node20.writeMultipleRegisters(0, 15);
  
  SerialBT.println("Writting Display Complete");
  SerialBT.println("-------------------------------------");
  SerialBT.println();
  SerialBT.println();
}
