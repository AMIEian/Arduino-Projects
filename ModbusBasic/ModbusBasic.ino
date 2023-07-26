/*

  Basic.pde - example using ModbusMaster library

  Library:: ModbusMaster
  Author:: Doc Walker <4-20ma@wvfans.net>

  Copyright:: 2009-2016 Doc Walker

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

*/

#include <ModbusMaster.h>
//#include <SoftwareSerial.h>

#define RXD2 16
#define TXD2 17
#define MAX485_DE 2

//SoftwareSerial mySerial(10, 11); // RX, TX

// instantiate ModbusMaster object
ModbusMaster node, node2;

uint16_t i = 0;

void preTrans()
{
  digitalWrite(MAX485_DE, 1);
}

void postTrans()
{
  digitalWrite(MAX485_DE, 0);
}

void setup()
{
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_DE, 0);
  // use Serial (port 0); initialize Modbus communication baud rate
  Serial.begin(9600);
  //mySerial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  //mySerial.println("Hello World...!");
  // communicate with Modbus slave ID 2 over Serial (port 0)
  node.begin(1, Serial2);
  //node2.begin(2, Serial2);
  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTrans);
  node.postTransmission(postTrans);

  //node2.preTransmission(preTrans);
  //node2.postTransmission(postTrans);
}

int x = 0, y = 100;

void loop()
{
  //static uint32_t i;
  uint8_t j, result;
  uint16_t data[24];
  /*
  result = node2.readHoldingRegisters(0, 24);
  //interrupts();
  // do something with data if read is successful
  if (result == node2.ku8MBSuccess)
  {
    for (j = 0; j < 24; j++)
    {
      data[j] = node2.getResponseBuffer(j);
      Serial.print(data[j]);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  node2.setTransmitBuffer(0, 200);
  node2.setTransmitBuffer(1, 25);
  node2.setTransmitBuffer(2, 40);
  
  node2.setTransmitBuffer(3, 150);
  node2.setTransmitBuffer(4, 0);
  node2.setTransmitBuffer(5, 0);

  node2.setTransmitBuffer(6, 450);
  node2.setTransmitBuffer(7, 22);
  node2.setTransmitBuffer(8, 0);

  node2.setTransmitBuffer(9, 0);
  node2.setTransmitBuffer(10, 83);
  node2.setTransmitBuffer(11, 28);

  node2.setTransmitBuffer(12, 0);
  node2.setTransmitBuffer(13, 0);
  node2.setTransmitBuffer(14, 92);

  result = node2.writeMultipleRegisters(0, 15);
  
  delay(100);
  */
  result = node.readHoldingRegisters(0, 24);
  //interrupts();
  // do something with data if read is successful
  if (result == node.ku8MBSuccess)
  {
    for (j = 0; j < 24; j++)
    {
      data[j] = node.getResponseBuffer(j);
      Serial.print(data[j]);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  node.setTransmitBuffer(0, 200);
  node.setTransmitBuffer(1, 25);
  node.setTransmitBuffer(2, 40);
  
  node.setTransmitBuffer(3, 150);
  node.setTransmitBuffer(4, 0);
  node.setTransmitBuffer(5, 0);

  node.setTransmitBuffer(6, 450);
  node.setTransmitBuffer(7, 22);
  node.setTransmitBuffer(8, 0);

  node.setTransmitBuffer(9, 0);
  node.setTransmitBuffer(10, 83);
  node.setTransmitBuffer(11, 28);

  node.setTransmitBuffer(12, 0);
  node.setTransmitBuffer(13, 0);
  node.setTransmitBuffer(14, 92);

  result = node.writeMultipleRegisters(0, 15);
  
  delay(100);
}
