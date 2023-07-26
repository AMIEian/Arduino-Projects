int LED_BUILTIN = 2;

void setup() 
  {
    Serial.begin(115200);    
    pinMode(LED_BUILTIN, OUTPUT);
  }

void loop() 
  {
    if(Serial.available()) 
      {
        char inChar = (char)Serial.read();
        if(inChar == 'X')
          {
            digitalWrite(LED_BUILTIN, LOW);
            delay(1000);
          }
        else if(inChar == 'O')
          {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(1000);
          }
      }
  }
