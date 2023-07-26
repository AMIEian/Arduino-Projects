int portSelector = 0;
void setup() 
  {
    Serial.begin(9600);
    Serial1.begin(2400);
    Serial2.begin(2400);
    Serial3.begin(2400);
  }

void loop() 
  {
    while(Serial1.available() && portSelector == 1)
      {
        // read serial port "1"
        char dataport1 = Serial1.read();
        Serial.write(dataport1);
      }
      
    while(Serial2.available() && portSelector == 2)
      {
        // read serial port "1"
        char dataport2 = Serial2.read();
        Serial.write(dataport2);
      }

    while(Serial3.available() && portSelector == 3)
      {
        // read serial port "1"
        char dataport3 = Serial3.read();
        Serial.write(dataport3);
      }
  }

void serialEvent() 
  {
    while (Serial.available()) 
      {
        char inChar = (char)Serial.read();
        
        if(inChar == '@')
          {
            Serial.print("Port 1 Selected");
            portSelector = 1;
          }
        if(inChar == '#')
          {
            Serial.print("Port 2 Selected");
            portSelector = 2;
          }
        if(inChar == '$')
          {
            Serial.print("Port 3 Selected");
            portSelector = 3;
          }
        if(inChar == '%')
          {
            Serial.print("Port 4 Selected");
            portSelector = 4;
          }

        if(portSelector == 1)
            Serial1.print(inChar);
        if(portSelector == 2)
            Serial2.print(inChar);
        if(portSelector == 3)
            Serial3.print(inChar);
        if(portSelector == 4)
            Serial.print(inChar);
      }
  }
