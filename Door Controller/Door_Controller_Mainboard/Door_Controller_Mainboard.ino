#include "BluetoothSerial.h"
#include <Preferences.h>
#include <ModbusRtu.h>

#define beatLED 15
#define comLED 2
#define modbusLED 4

#define fireInput 33
#define hooter 32
#define spareDO 22
#define emergencyInput 35

#define adr_4 12
#define adr_3 13
#define adr_2 14
#define adr_1 34

#define RXD2 16
#define TXD2 17

#define RXD1 27
#define TXD1 26
#define MAX485_DE 25

//Modbus Array Addresses
#define TOTAL_DOORS 0
#define EMERGENCY_PRESSED 1
#define FIRE_ALARM 2
#define AUDIO_ALARM 3
#define DISABLE_INTERLOCK 4
#define EMERGENCY_OPEN 5
#define DOORS_DATA_START 10

bool interlockDisabled = false;

long currentmillis = 0;
int last_blink_time = 0;
int blink_interval = 500;
int ledState = LOW;     // the current state of LED

uint8_t boards_Connected = 0;
uint8_t total_Doors = 0;
uint8_t doors_Status[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t door_Array_Status = 0;
uint16_t ModbusData[90];

uint8_t delays[3] = {2, 5, 20};

BluetoothSerial SerialBT;
//Preferances to store data permanently
Preferences prefs;

uint8_t slaveID = 1;
Modbus slave(1,Serial,MAX485_DE); // this is slave @1 and RS-232 or USB-FTDI

void setup()
  {    
    InitBoard();
    initNode();
  }

void loop() 
  {
    digitalWrite(comLED, HIGH);
    serialEvent();
    serialBTEvent();
    digitalWrite(comLED, LOW);
    digitalWrite(modbusLED, HIGH);
    modbusTasks();
    ModbusControl();
    digitalWrite(modbusLED, LOW);
    delay(10);
    currentmillis = millis();   //Get current millis
    if(currentmillis - last_blink_time >= blink_interval)
      {
        last_blink_time = currentmillis;
        // toggle state of LED
        ledState = !ledState;
    
        // control LED arccoding to the toggled state
        digitalWrite(beatLED, ledState);
        CheckEmergency();
        Serial2.write(door_Array_Status);
        //SerialBT.print("Current Door Array Status = ");
        //SerialBT.println(door_Array_Status);
      }
  }
