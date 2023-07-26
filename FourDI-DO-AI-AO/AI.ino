#include <MCP3424.h>
#include <Wire.h>

MCP3424 MCP(0);

long MCPData[4]; // Array used to store results

void AiSetup()
  {
    MCP.begin(0);
  }

void GetAIs()
  {
    for (int i=1;i<=4;i++)
      {  
        MCP.configuration(i,16,1,1); // MCP3424 is configured to channel i with 16 bits resolution, continous mode and gain defined to 8 
        MCPData[i-1] = MCP.measure(); // Measure is stocked in array Voltage, note that the library will wait for a completed conversion that takes around 200 ms@18bits
      }
    
    //Apply calibration for zeroValue first
    for(int i = 0; i < 4; i++)
      {
        MCPData[i] = MCPData[i] - zeroVals[i];
      }
    //Apply calibration for fullValues now
    for(int k = 0; k < 4; k++)
      {
        float ratio = (float)MCPData[k]/(float)fullVals[k];
        MCPData[k] = (int16_t)(ratio * MAX_COUNTS);
        //Serial.println(MCPData[k]);
      }
    //Serial.println("***************");
    
    //Store MCP values
    for(int k = 0; k < 4; k++)
      {
        counts[k] = (int16_t)MCPData[k];
      }
  }
