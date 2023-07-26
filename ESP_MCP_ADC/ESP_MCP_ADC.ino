#include <MCP3424.h>
#include <Wire.h>

MCP3424 MCP(0);

const int16_t MAX_COUNTS = 10000;

int16_t zeroVals[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int16_t fullVals[20] = {10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 28500, 28500, 28500, 28500};
int16_t counts[21] = {1000, 970, 760, -550, 930, 120, 680, 540, 8743, 3190, 7310, -2340, -4560, 810, 620, 1960, 0, 0, 0, 0, 640};

long MCPData[4]; // Array used to store results

long currentTime = 0; //Current time of system

long lastMCPCheckTime = 0;  //Maintains last MCP check time
long MCPCheckInterval = 1500; //Check MCP after every 1 & 1/2 sec

void setup() 
{
  Serial.begin(115200); // initialize serial for PC and ESP interface
  MCP.begin(0); // initialize I2C(MCP) for MCP3234. MCP Address = 0x00
}

void loop() 
{
  currentTime = millis();
  if(currentTime - lastMCPCheckTime >= MCPCheckInterval)
    {
      lastMCPCheckTime = currentTime;
      readMCP();
    }
}

void readMCP()
  {
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
        MCPData[k] = (int16_t)(ratio * MAX_COUNTS);
        Serial.println(MCPData[k]);
      }
    Serial.println("***************");
    
    //Store MCP values
    for(int k = 0; k < 4; k++)
      {
        counts[k + 16] = MCPData[k];
      }
  }
