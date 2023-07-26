#include<WiFi.h>
#include <ModbusMaster.h>
#include "Settimino.h"
#include "BluetoothSerial.h"

#define RXD2 16
#define TXD2 17
#define MAX485_DE 2

#define DO_IT_SMALL

//INTEGRETOR NUMBER. TO BE SET BY POLYGON ESP32 PROGRAMMER
const int unitNo = 1;

//DB NUMBERS -
int configDB = 501;
int readDB = 502; //DB Where PLC will store the data to send to display 
int writeDB = 503;  //DB Where Integretor will send the data from the display 
int readErrDB = 504;
int writeErrDB = 505;
int diagDB = 506;

//DB BLOCK SIZES IN BYTES
const int configDBSize = 6;
const int readDBSize = 32;  //Size of the DB Where PLC will store the data to send to display 
const int writeDBSize = 48; //Size of the DB Where Integretor will send the data from the display 
const int readErrDBSize = 32;
const int writeErrDBSize = 32;
const int diagDBSize = 6;

const int com_ok = 12;
const int com_err = 27;

byte mac[] = { 0x00, 0x50, 0x56, 0xC0, 0x00, 0x06 };

//IPAddress Local(192,168,0,90); // Local Address
IPAddress PLC(192,168,0,70);   // PLC Address
IPAddress ip(192, 168, 0, 150);
IPAddress myDns(192, 168, 0, 1);

//IPAddress gWay(192, 168, 0, 1);

byte Buffer[10];

// S7Client will create an EthernetClient as TCP Client
S7Client Client(_S7WIRED);

// instantiate ModbusMaster object
ModbusMaster node0, node1, node2, node3, node4, node5, node6, node7, node8, node9, node10, node11, node12, node13, node14, node15, node16, node17, node18, node19, node20;

unsigned long Elapsed; // To calc the execution time

unsigned long lastTime = 0; 

bool resetESP = false;

uint8_t result = 111;

const int WizReset = 4;

void(* resetFunc) (void) = 0;

int node_0_old = 0, node_1_old = 0, node_2_old = 0, node_3_old = 0, node_4_old = 0, node_5_old = 0, node_6_old = 0, node_7_old = 0, node_8_old = 0, node_9_old = 0, node_10_old = 0, node_11_old = 0, node_12_old = 0, node_13_old = 0, node_14_old = 0, node_15_old = 0, node_16_old = 0, node_17_old = 0, node_18_old = 0, node_19_old = 0, node_20_old = 0;

int nodeCounts = 20;

//Table Data related variables 
#define Comm_Flag 0
#define No_of_Dispays 1
#define Read_Flag 2
#define Read_Display_No 3
#define Write_Flag 4
#define Write_Display_No 5

byte configData[6];
byte readErrData[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte writeErrData[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t displayData[24];
int plcData[16]; //= {200, 25, 40, 150, 0, 0, 450, 22, 0, 0, 83, 28, 0, 0, 92};

BluetoothSerial SerialBT;

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
  pinMode(WizReset, OUTPUT);
  
  digitalWrite(com_err, LOW);
  digitalWrite(com_ok, LOW);
  digitalWrite(WizReset, LOW);
  
  //WiFi.macAddress(mac);
  WiFi.mode(WIFI_OFF);
  //btStop();
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_DE, 0);
  Serial.begin(115200);
  String btName = "Poly"+unitNo;
  SerialBT.begin(btName); //Bluetooth device name

  delay(10000);
  
  // Start the Ethernet Library
  connectToEth();

  //Config DBs as per integretor number set above
  configDBNo();

  //Get configuration from PLC
  getConfig();
  nodeCounts = configData[No_of_Dispays];
  
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  initNodes();
}

void loop()
{
  getConfig();
  if(configData[Comm_Flag] == 2)
    resetESP = true;
  configData[Comm_Flag] = 0;
  /*
  configData[Comm_Flag] = 1;
  configData[No_of_Dispays] = 2;
  configData[Read_Flag] = 3;
  configData[Read_Display_No] = 4;
  configData[Write_Flag] = 5;
  configData[Write_Display_No] = 6;
  */
  //Check for read command
  if(configData[Read_Flag] == 1)
    {
      SerialBT.println("Read Command Received...!");
      SerialBT.print("Reading Display No. "); SerialBT.println(configData[Read_Display_No]);
      result = 111;
      //If read command is 1. Get the display number and read it's data and send it to PLC
      readTheDisplay(configData[Read_Display_No]);
      if(result == 0)
      {
        SerialBT.println("Display Read Successful...!");
        sendDisplayDataToPLC();
        configData[Read_Flag] = 0;
        if(readErrData[configData[Read_Display_No]] == 1)
          {
            readErrData[configData[Read_Display_No]] = 0;
            updateReadErrDB();
          }
      }
      else
      {
        SerialBT.println("Display Read Failed...!");
        readErrData[configData[Read_Display_No]] = 1;
        updateReadErrDB();
      }
    }
    
  //Check for write command
  if(configData[Write_Flag] == 1)
    {
      SerialBT.println("Write Command Received...!");
      SerialBT.print("Writing Display No. "); SerialBT.println(configData[Write_Display_No]);
      result = 111;
      //If read command is 1. Get the display number and read it's data and send it to PLC
      getDisplayDataFromPLC();
      writeTheDisplay(configData[Write_Display_No]);
      if(result == 0)
      {
        SerialBT.println("Display Write Successful...!");
        configData[Write_Flag] = 0;
        if(writeErrData[configData[Write_Display_No]] == 1)
          {
            writeErrData[configData[Write_Display_No]] = 0;
            updateWriteErrDB();
          }
      }
      else
      {
        SerialBT.println("Display Write Failed...!");
        writeErrData[configData[Write_Display_No]] = 1;
        updateWriteErrDB();
      }
    }
  setConfig();
  if(resetESP == true)
    {
      delay(1000);
      resetFunc();
    }
}

void configDBNo()
{
  switch(unitNo)
  {
    case 1:
      configDB = 501;
      readDB = 502; //DB Where PLC will store the data to send to display 
      writeDB = 503;  //DB Where Integretor will send the data from the display 
      readErrDB = 504;
      writeErrDB = 505;
      diagDB = 506;
      break;
    case 2:
      configDB = 511;
      readDB = 512; //DB Where PLC will store the data to send to display 
      writeDB = 513;  //DB Where Integretor will send the data from the display 
      readErrDB = 514;
      writeErrDB = 515;
      diagDB = 516;
      break;
    case 3:
      configDB = 521;
      readDB = 522; //DB Where PLC will store the data to send to display 
      writeDB = 523;  //DB Where Integretor will send the data from the display 
      readErrDB = 524;
      writeErrDB = 525;
      diagDB = 526;
      break;
    case 4:
      configDB = 531;
      readDB = 532; //DB Where PLC will store the data to send to display 
      writeDB = 533;  //DB Where Integretor will send the data from the display 
      readErrDB = 534;
      writeErrDB = 535;
      diagDB = 536;
      break;
    case 5:
      configDB = 541;
      readDB = 542; //DB Where PLC will store the data to send to display 
      writeDB = 543;  //DB Where Integretor will send the data from the display 
      readErrDB = 544;
      writeErrDB = 545;
      diagDB = 546;
      break;
    case 6:
      configDB = 551;
      readDB = 552; //DB Where PLC will store the data to send to display 
      writeDB = 553;  //DB Where Integretor will send the data from the display 
      readErrDB = 554;
      writeErrDB = 555;
      diagDB = 556;
      break;
    case 7:
      configDB = 561;
      readDB = 562; //DB Where PLC will store the data to send to display 
      writeDB = 563;  //DB Where Integretor will send the data from the display 
      readErrDB = 564;
      writeErrDB = 565;
      diagDB = 566;
      break;
    default :
      configDB = 501;
      readDB = 502; //DB Where PLC will store the data to send to display 
      writeDB = 503;  //DB Where Integretor will send the data from the display 
      readErrDB = 504;
      writeErrDB = 505;
      diagDB = 506;
  }
}

void initNodes()
{
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

void readTheDisplay(byte displayNumber)
{
  switch(displayNumber)
    {
      case 1:
        node0_Read();
        break;
        
      case 2:
        node1_Read();
        break;

      case 3:
        node2_Read();
        break;

      case 4:
        node3_Read();
        break;

      case 5:
        node4_Read();
        break;

      case 6:
        node5_Read();
        break;

      case 7:
        node6_Read();
        break;

      case 8:
        node7_Read();
        break;

      case 9:
        node8_Read();
        break;

      case 10:
        node9_Read();
        break;

      case 11:
        node10_Read();
        break;

      case 12:
        node11_Read();
        break;

      case 13:
        node12_Read();
        break;

      case 14:
        node13_Read();
        break;

      case 15:
        node14_Read();
        break;

      case 16:
        node15_Read();
        break;

      case 17:
        node16_Read();
        break;

      case 18:
        node17_Read();
        break;

      case 19:
        node18_Read();
        break;

      case 20:
        node19_Read();
        break;

      case 21:
        node20_Read();
        break;
    }
}

void writeTheDisplay(byte displayNumber)
{
  switch(displayNumber)
    {
      case 1:
        node0_Write();
        break;
        
      case 2:
        node1_Write();
        break;

      case 3:
        node2_Write();
        break;

      case 4:
        node3_Write();
        break;

      case 5:
        node4_Write();
        break;

      case 6:
        node5_Write();
        break;

      case 7:
        node6_Write();
        break;

      case 8:
        node7_Write();
        break;

      case 9:
        node8_Write();
        break;

      case 10:
        node9_Write();
        break;

      case 11:
        node10_Write();
        break;

      case 12:
        node11_Write();
        break;

      case 13:
        node12_Write();
        break;

      case 14:
        node13_Write();
        break;

      case 15:
        node14_Write();
        break;

      case 16:
        node15_Write();
        break;

      case 17:
        node16_Write();
        break;

      case 18:
        node17_Write();
        break;

      case 19:
        node18_Write();
        break;

      case 20:
        node19_Write();
        break;

      case 21:
        node20_Write();
        break;
    }
}
