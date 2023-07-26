#include<WiFi.h>
#include <ModbusMaster.h>
#include "Settimino.h"

#define RXD2 16
#define TXD2 17
#define MAX485_DE 2

#define DO_IT_SMALL

const int com_ok = 12;
const int com_err = 27;

byte mac[] = { 0x00, 0x50, 0x56, 0xC0, 0x00, 0x06 };

//IPAddress Local(192,168,0,90); // Local Address
IPAddress PLC(192,168,0,70);   // PLC Address
IPAddress ip(192, 168, 0, 150);
IPAddress myDns(192, 168, 0, 1);

int startingDB = 0;
//IPAddress gWay(192, 168, 0, 1);

byte Buffer[10];

// S7Client will create an EthernetClient as TCP Client
S7Client Client(_S7WIRED);

// instantiate ModbusMaster object
ModbusMaster node0, node1, node2, node3, node4, node5, node6, node7, node8, node9, node10, node11, node12, node13, node14, node15, node16, node17, node18, node19, node20;

unsigned long Elapsed; // To calc the execution time

unsigned long lastTime = 0; 

bool invert = false;

const int RESET_P = 2;

void(* resetFunc) (void) = 0;

int temp[16]; //= {200, 25, 40, 150, 0, 0, 450, 22, 0, 0, 83, 28, 0, 0, 92};

int node_0_old = 0, node_1_old = 0, node_2_old = 0, node_3_old = 0, node_4_old = 0, node_5_old = 0, node_6_old = 0, node_7_old = 0, node_8_old = 0, node_9_old = 0, node_10_old = 0, node_11_old = 0, node_12_old = 0, node_13_old = 0, node_14_old = 0, node_15_old = 0, node_16_old = 0, node_17_old = 0, node_18_old = 0, node_19_old = 0, node_20_old = 0;

int nodeCounts = 20;

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
  pinMode(com_err, OUTPUT);
  pinMode(com_ok, OUTPUT);
  digitalWrite(com_err, LOW);
  digitalWrite(com_ok, LOW);
  //WiFi.macAddress(mac);
  WiFi.mode(WIFI_OFF);
  btStop();
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_DE, 0);
  Serial.begin(115200);

  // Start the Ethernet Library
  connectToEth();
  
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  if(nodeCounts > 0)
    node0_Init();
  if(nodeCounts > 1)
    node1_Init();
  if(nodeCounts > 2)
    node2_Init();
  if(nodeCounts > 3)
    node3_Init();
  if(nodeCounts > 4)
    node4_Init();
  if(nodeCounts > 5)
    node5_Init();
  if(nodeCounts > 6)
    node6_Init();
  if(nodeCounts > 7)
    node7_Init();
  if(nodeCounts > 8)
    node8_Init();
  if(nodeCounts > 9)
    node9_Init();
  if(nodeCounts > 10)
    node10_Init();
  if(nodeCounts > 11)
    node11_Init();
  if(nodeCounts > 12)
    node12_Init();
  if(nodeCounts > 13)
    node13_Init();
  if(nodeCounts > 14)
    node14_Init();
  if(nodeCounts > 15)
    node15_Init();
  if(nodeCounts > 16)
    node16_Init();
  if(nodeCounts > 17)
    node17_Init();
  if(nodeCounts > 18)
    node18_Init();
  if(nodeCounts > 19)
    node19_Init();
  if(nodeCounts > 20)
    node20_Init();
}

void loop()
{
  if(nodeCounts > 0)
    node0_Tasks();
  if(nodeCounts > 1)
    node1_Tasks();
  if(nodeCounts > 2)
    node2_Tasks();
  if(nodeCounts > 3)
    node3_Tasks();
  if(nodeCounts > 4)
    node4_Tasks();
  if(nodeCounts > 5)
    node5_Tasks();
  if(nodeCounts > 6)
    node6_Tasks();
  if(nodeCounts > 7)
    node7_Tasks();
  if(nodeCounts > 8)
    node8_Tasks();
  if(nodeCounts > 9)
    node9_Tasks();
  if(nodeCounts > 10)
    node10_Tasks();
  if(nodeCounts > 11)
    node11_Tasks();
  if(nodeCounts > 12)
    node12_Tasks();
  if(nodeCounts > 13)
    node13_Tasks();
  if(nodeCounts > 14)
    node14_Tasks();
  if(nodeCounts > 15)
    node15_Tasks();
  if(nodeCounts > 16)
    node16_Tasks();
  if(nodeCounts > 17)
    node17_Tasks();
  if(nodeCounts > 18)
    node18_Tasks();
  if(nodeCounts > 19)
    node19_Tasks();
  if(nodeCounts > 20)
    node20_Tasks();
}

void AssembleArray()
{
  pbyte buf;
  buf = pbyte(&PDU.DATA[0]);
  int index = 0;
  int te;
  int x = 0;
  //Serial.println();
  //Fill Data into temp array
  while(x < 32)
    {
      te = buf[x] << 8;
      x = x + 1;
      te = te + buf[x];
      x = x + 1;
      temp[index] = te;
      //Serial.print(temp[index]);
      //Serial.print(" ");
      index = index + 1;
    }
  //Serial.println();
}
