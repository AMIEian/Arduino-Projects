const int per0 = 2;
const int per25 = 3;
const int per50 = 4;
const int per75 = 5;
const int per100 = 6;

boolean sendData = false;

void setup() 
{
  pinMode(per0, INPUT_PULLUP);
  pinMode(per25, INPUT_PULLUP);
  pinMode(per50, INPUT_PULLUP);
  pinMode(per75, INPUT_PULLUP);
  pinMode(per100, INPUT_PULLUP);

  Serial.begin(115200);
}

void loop() 
{
  if(sendData)
    {
      int err = 0;
      sendData = false;
      for (uint8_t k = 0; k < 20; k++) 
        {
          err = random(-40,40);
          Serial.print(" ");
          if(digitalRead(per0) == LOW)
            Serial.print(0 + err);
          if(digitalRead(per25) == LOW)
            Serial.print(2500 + err);  
          if(digitalRead(per50) == LOW)
            Serial.print(5000 + err); 
          if(digitalRead(per75) == LOW)
            Serial.print(7500 + err); 
          if(digitalRead(per100) == LOW)
            Serial.print(10000 + err);    
        }
      Serial.print(" ");
      Serial.print(0);
      Serial.println();
    }
}

void serialEvent() 
  {
    while(Serial.available()) 
      {
        // get the new byte:
        char inChar = (char)Serial.read();
        
        if(inChar == '#')
          sendData = true;
      }
  }
