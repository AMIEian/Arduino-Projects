#include <SPI.h>
#include <MCP3424.h>
#include <Wire.h>

MCP3424 MCP(0); // Declaration of MCP3424 pin addr1 et addr0 are connected to +5V
long MCPData[4]; // Array used to store results

const int ResetPin = 9;
const int ADSSelectPin = 10;
const int DRDY = 8;

byte SPIData = 0x00;
byte StatusByte = 0x00;
byte ResultH = 0x00;
byte ResultL = 0x00;
unsigned char rxChan = 0;
int ADSData[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
long ADSAvgData[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int16_t zeroVals[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int16_t fullVals[20] = {10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000};
int16_t counts[21] = {1000, 970, 760, -550, 930, 120, 680, 540, 8743, 3190, 7310, -2340, -4560, 810, 620, 1960, 10070, 5180, 840, -230, 640};
int analogResult = 0;
int ADSReadings = 15;

unsigned long Elapsed; // To calc the execution time

//----------------------------------------------------------------------
// Time related routines
//----------------------------------------------------------------------

void MarkTime()
  {
    Elapsed = millis();
  }
//----------------------------------------------------------------------

void ShowTime()
  {
    // Calcs the time
    Elapsed = millis()-Elapsed;
    Serial.print("Job time (ms) : ");
    Serial.println(Elapsed);
  }

void setup() 
{
  Serial.begin(115200);
  pinMode(ResetPin, OUTPUT);
  pinMode(ADSSelectPin, OUTPUT);
  pinMode(DRDY, INPUT);
  
  digitalWrite(ResetPin, LOW);
  digitalWrite(ADSSelectPin, HIGH);

  delay(1000);
  
  SPI.begin();
  
  MCP.begin(0);
  
  digitalWrite(ResetPin, LOW);
  delay(40);
  digitalWrite(ResetPin, HIGH);
  delay(40);
  
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
 
  digitalWrite(ADSSelectPin, LOW);
  delayMicroseconds(10);
  
  SPIData = SPI.transfer(0x60); //Write to register 0x00
  SPIData = SPI.transfer(0x02); //Register CONFIG0 = 0x12 (00010010)
  
  SPIData = SPI.transfer(0x61); //Write to register 0x01
  SPIData = SPI.transfer(0x73); //Register CONFIG1 = 0x70 (‭01110000‬) 0x73

  SPIData = SPI.transfer(0x62); //Write to register 0x02
  SPIData = SPI.transfer(0x00); //Register MUXSCH = 0x00 (‭00000000‬)

  SPIData = SPI.transfer(0x63); //Write to register 0x03
  SPIData = SPI.transfer(0x00); //Register MUXDIF = 0x00 (‭00000000‬)

  SPIData = SPI.transfer(0x64); //Write to register 0x04
  SPIData = SPI.transfer(0xff); //Register MUXSG0 = 0xff (11111111)

  SPIData = SPI.transfer(0x65); //Write to register 0x05
  SPIData = SPI.transfer(0xff); //Register MUXSG1 = 0xff (11111111)

  SPIData = SPI.transfer(0x66); //Write to register 0x06
  SPIData = SPI.transfer(0x00); //Register SYSRED = 0x00 (00000000)

  SPIData = SPI.transfer(0x67); //Write to register 0x07
  SPIData = SPI.transfer(0x00); //Register GPIOC = 0x00 (00000000)

  SPIData = SPI.transfer(0x68); //Write to register 0x08
  SPIData = SPI.transfer(0x00); //Register GPIOD = 0x00 (00000000)

  SPIData = SPI.transfer(0x69); //Write to register 0x09
  SPIData = SPI.transfer(0x8B); //Register GPIOC = 0x8B (‭10001011‬)
  
  delay(1000);
  
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
 
  digitalWrite(ADSSelectPin, LOW);
  delayMicroseconds(10);
  SPIData = SPI.transfer(0x40); //Register read command address 0x00
  SPIData = SPI.transfer(0x00); //Extra clocks to get data
  Serial.println(SPIData, HEX);
  SPIData = SPI.transfer(0x41); //Register read command address 0x01
  SPIData = SPI.transfer(0x00); //Extra clocks to get data
  Serial.println(SPIData, HEX);
  SPIData = SPI.transfer(0x42); //Register read command address 0x02
  SPIData = SPI.transfer(0x00); //Extra clocks to get data
  Serial.println(SPIData, HEX);
  SPIData = SPI.transfer(0x43); //Register read command address 0x03
  SPIData = SPI.transfer(0x00); //Extra clocks to get data
  Serial.println(SPIData, HEX);
  SPIData = SPI.transfer(0x44); //Register read command address 0x04
  SPIData = SPI.transfer(0x00); //Extra clocks to get data
  Serial.println(SPIData, HEX);
  SPIData = SPI.transfer(0x45); //Register read command address 0x05
  SPIData = SPI.transfer(0x00); //Extra clocks to get data
  Serial.println(SPIData, HEX);
  SPIData = SPI.transfer(0x46); //Register read command address 0x06
  SPIData = SPI.transfer(0x00); //Extra clocks to get data
  Serial.println(SPIData, HEX);
  SPIData = SPI.transfer(0x47); //Register read command address 0x07
  SPIData = SPI.transfer(0x00); //Extra clocks to get data
  Serial.println(SPIData, HEX);
  SPIData = SPI.transfer(0x48); //Register read command address 0x08
  SPIData = SPI.transfer(0x00); //Extra clocks to get data
  Serial.println(SPIData, HEX);
  SPIData = SPI.transfer(0x49); //Register read command address 0x09
  SPIData = SPI.transfer(0x00); //Extra clocks to get data
  Serial.println(SPIData, HEX);
  delayMicroseconds(10);
  digitalWrite(ADSSelectPin, HIGH);

  SPI.endTransaction();
}

void loop() 
{
  MarkTime();
  
  for(int i = 0; i < 16; i++)
    {
      ADSAvgData[i] = 0;
    }
  for(int k = 0; k < ADSReadings; k++)
    {
      for(int i = 0; i<32; i++)
        {
          while(digitalRead(DRDY) == HIGH)
            {
              
            }
          if(digitalRead(DRDY) == LOW)
            {
              SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
              
              digitalWrite(ADSSelectPin, LOW);
              delayMicroseconds(10);
              
              SPIData = SPI.transfer(0x30);
              delayMicroseconds(1);
              StatusByte = SPI.transfer(0x00);
              delayMicroseconds(1);
            
              StatusByte = StatusByte & 0x1f;
              if(StatusByte > 7 && StatusByte < 24)
                rxChan = StatusByte - 8 ;
              else
                rxChan = 16 ;
            
              ResultH = SPI.transfer(0x00);
              //delayMicroseconds(1);
              ResultL = SPI.transfer(0x00);
              //delayMicroseconds(1);
              SPIData = SPI.transfer(0x00);
              //delayMicroseconds(1);
              
              analogResult = 0x00;
          
              analogResult = ResultH;
              analogResult = analogResult << 8;
              analogResult = analogResult | ResultL ;
              ADSData[rxChan] = analogResult;
              
              digitalWrite(ADSSelectPin, HIGH);
          
              SPI.endTransaction();
            }
        }
      
      for(int i = 0; i < 16; i++)
        {
          ADSAvgData[i] = ADSAvgData[i] + ADSData[i];
        }
    }

  //Find average
  for(int i = 0; i < 16; i++)
    {
      ADSAvgData[i] = (long)ADSAvgData[i]/ADSReadings;
    }
          
  //Apply Calibration zero values
  for(int i = 0; i < 16; i++)
    {
      ADSAvgData[i] = ADSAvgData[i] - zeroVals[i];
    }  
  //Apply calibration for fullValues now
  for(int k = 0; k < 16; k++)
    {
      float ratio = (float)ADSAvgData[k]/(float)fullVals[k];
      ADSAvgData[k] = (int16_t)(ratio * 10000);
    }
    
  //Store Values
  for(int i = 0; i < 16; i++)
    {
      counts[i] = (int16_t)ADSAvgData[i];
    }
  
  //MCP Testing
  for (int i=1;i<=4;i++)
    {  
      MCP.configuration(i,16,1,1); // MCP3424 is configured to channel i with 16 bits resolution, continous mode and gain defined to 8 
      MCPData[i-1] = MCP.measure(); // Measure is stocked in array Voltage, note that the library will wait for a completed conversion that takes around 200 ms@18bits
    }
    
  //Apply calibration for zeroValue first
  for(int i = 0; i < 4; i++)
    {
      MCPData[i] = MCPData[i] - zeroVals[i + 16];
    }
  //Apply calibration for fullValues now
  for(int k = 0; k < 4; k++)
    {
      float ratio = (float)MCPData[k]/(float)fullVals[k + 16];
      MCPData[k] = (int16_t)(ratio * 10000);
    }

  //Store MCP values
  for(int k = 0; k < 4; k++)
    {
      counts[k + 16] = MCPData[k];
    }
  ShowTime();
  
  for (uint8_t k = 0; k < 21; k++) 
    {
      Serial.print(" ");
      Serial.print(counts[k]);
      ADSData[k] = 0;
    }
  Serial.println();
}
