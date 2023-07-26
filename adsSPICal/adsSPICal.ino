#include <EEPROM.h>

//By default all values are 0
int zeroVals[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int fullVals[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int counts[20] =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

String pcCommand = "";         // a String to hold incoming data
boolean commandReceiving = false;
boolean pcCommandReceived = false;  // whether the string is complete
boolean sendData = false;

void setup() 
{
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the pcCommand:
  pcCommand.reserve(200);
  char calDone;
  EEPROM.get(0, calDone);
  if(calDone == 'C')
    {
      for(int i = 1; i <= 40; i = i+2)
        {
          int k;
          EEPROM.get(i,k);
          Serial.print(" ");
          Serial.print(k);
        }
    }
  Serial.println();  
  for(int i = 0; i < 20; i++)
    {
      counts[i] = random(-100, 100)+random(-10000, 10000);
    }
}

void loop() 
{
  if(sendData)
  {
    sendData = false;
    for(int i = 0; i < 20; i++)
    {
      Serial.print(" ");
      Serial.print(counts[i]);
    }
    Serial.println();
  }

  if(pcCommandReceived)
  {
    pcCommandReceived = false;
    
    if(pcCommand.charAt(0) >= 'a' && pcCommand.charAt(0) <= 'z')
      calibrateZero();
    else if(pcCommand.charAt(0) >= 'A' && pcCommand.charAt(0) <= 'Z')
      calibrateFull();
    else
      pcCommand = "";
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() 
{
  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    //Check whether it is data send command
    if (inChar == '#' && commandReceiving == false) 
    {
      sendData = true;
    }
    else
    {
      commandReceiving = true;
      // If its not then add it to the pcCommand:
      pcCommand += inChar;
      // if the incoming character is a newline, set a flag so the main loop can
      // do calibration with it:
      if (inChar == '\n') 
      {
        pcCommandReceived = true;
        commandReceiving = false;
      }
    }
  }
}

void calibrateZero()
{
  int index = 0;
  pcCommand.trim(); //Removes Extra Spaces and Newline Character from Command String
  for(int i = 0; i < pcCommand.length(); i++)
    {
      index = pcCommand.charAt(i) - 'a';
      zeroVals[index] = counts[index];
    }
  for(int i = 0; i < 20; i++)
    {
      Serial.print(" ");
      Serial.print(zeroVals[i]);
    }
    Serial.println();  
  EEPROM.put(0,'C');  //Indicates calibration is done
  index = 0;
  for(int i = 1; i <= 40; i = i+2)
    {
      EEPROM.put(i,zeroVals[index]);
      index++;
    }
  for(int i = 1; i <= 40; i = i+2)
    {
      int k;
      EEPROM.get(i,k);
      Serial.print(" ");
      Serial.print(k);
    }  
  pcCommand = "";
}

void calibrateFull()
{
  int index = 0;
  pcCommand.trim(); //Removes Extra Spaces and Newline Character from Command String
  for(int i = 0; i < pcCommand.length(); i++)
    {
      index = pcCommand.charAt(i) - 'A';
      zeroVals[index] = counts[index];
    }
  for(int i = 0; i < 20; i++)
    {
      Serial.print(" ");
      Serial.print(zeroVals[i]);
    }
    Serial.println();  
  EEPROM.put(0,'C');  //Indicates calibration is done
  index = 0;
  for(int i = 1; i <= 40; i = i+2)
    {
      EEPROM.put(i,zeroVals[index]);
      index++;
    }
  for(int i = 1; i <= 40; i = i+2)
    {
      int k;
      EEPROM.get(i,k);
      Serial.print(" ");
      Serial.print(k);
    }  
  pcCommand = "";
}
