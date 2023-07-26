#include <SimpleModbusSlave.h>
#include "BluetoothSerial.h"

#define RXD2 16
#define TXD2 17
#define MAX485_DE 4

int16_t value[1];
int16_t old_value;
String inputString = "";

BluetoothSerial SerialBT;

//////////////// registers of your slave ///////////////////
enum 
{     
  // just add or remove registers and you're good to go...
  // The first register starts at address 0
  DI0,     
  DI1,        
  DI2,
  DI3,
  DI4,
  DI5,  
  DI6,
  DI7,
  DI8,
  DI9,
  DI10,
  DI11,
  DO0,
  DO1,
  DO2,
  TOTAL_ERRORS,
  // leave this one
  TOTAL_REGS_SIZE 
  // total number of registers for function 3 and 16 share the same register array
};

unsigned int holdingRegs[TOTAL_REGS_SIZE]; // function 3 and 16 register array
////////////////////////////////////////////////////////////

long comBaud = 9600;
unsigned char modSlaveID = 1;

void setup() 
{    
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_DE, 0);
  
  Serial.begin(9600);
  
  SerialBT.begin("POLYGON");
  
  /* parameters(long baudrate, 
                unsigned char ID, 
                unsigned char transmit enable pin, 
                unsigned int holding registers size,
                unsigned char low latency)
                
     The transmit enable pin is used in half duplex communication to activate a MAX485 or similar
     to deactivate this mode use any value < 2 because 0 & 1 is reserved for Rx & Tx.
     Low latency delays makes the implementation non-standard
     but practically it works with all major modbus master implementations.
  */
  modbus_configure(comBaud, modSlaveID, MAX485_DE, TOTAL_REGS_SIZE, 0);

  Serial2.begin(comBaud, SERIAL_8N1, RXD2, TXD2);
}

void loop() 
{
  bool newData = false;
  while(newData == false)
    newData = getNewData();
  checkControlLogic();
  holdingRegs[TOTAL_ERRORS] = modbus_update(holdingRegs);
}

void checkControlLogic()
{
  int16_t mask;
  uint16_t result, bitSum;

  bitSum = 0;
  mask = 0x01;

  for(uint16_t i = 0; i < 12; i++)
    {
      result = value[0] & mask;
      if(result != 0)
        {
          holdingRegs[i] = 1;
          SerialBT.print("holdingRegs[");SerialBT.print(i);SerialBT.print("] = ");SerialBT.println(holdingRegs[i]);
          bitSum++;
        }
      else
        holdingRegs[i] = 0;
      mask = mask << 1;
    }

  SerialBT.print("Bit Sum = ");SerialBT.println(bitSum);
  if(bitSum < 11)
    {
      holdingRegs[12] = holdingRegs[13] = holdingRegs[14] = 1;
      Serial.println(7);
      SerialBT.println("Turning Outputs High...");
    }
  else if(bitSum > 11)
    {
      holdingRegs[12] = holdingRegs[13] = holdingRegs[14] = 0;
      Serial.println(0);
      SerialBT.println("Turning Outputs Low...");
    }
}

boolean getNewData()
{
  boolean stringComplete = false;
  inputString = "";
  int waitCount = 0;

  Serial.print('#');
  //digitalWrite(Arduino_Error_LED, HIGH);
  
  while(Serial.available() <= 0)
    {
      delay(1);
      waitCount++;
      if(waitCount > 5000)
        {
          waitCount = 0;
          Serial.print('#');
        }
    }

  waitCount = 0;
  
  while(stringComplete == false)
    {    
      while(Serial.available()) 
        {
          // get the new byte:
          char inChar = (char)Serial.read();
          inputString += inChar;
          if (inChar == '\n' || inChar == '\r') 
            stringComplete = true;
        }
      delay(1);
      waitCount++;
      if(waitCount > 1000)
        return false;
    }
  SerialBT.print("String = ");
  SerialBT.println(inputString);
  value[0] = inputString.toInt();
  inputString = "";
  //digitalWrite(Arduino_Error_LED, LOW);
  return true;
}
