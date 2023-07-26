const int Relay_1 = 2;
const int Relay_2 = 3;
const int Relay_3 = 4;
const int Relay_4 = 5;

byte receivedCode = 0x00;
byte id = 0x20;

void allPinsDown()
  {
    digitalWrite(Relay_1,HIGH);
    digitalWrite(Relay_2,HIGH);
    digitalWrite(Relay_3,HIGH);
    digitalWrite(Relay_4,HIGH);
  }

void setup()
  {
    Serial.begin(9600);
    pinMode(Relay_1,OUTPUT);
    pinMode(Relay_2,OUTPUT);
    pinMode(Relay_3,OUTPUT);
    pinMode(Relay_4,OUTPUT);
    allPinsDown();
  }

void loop()
  {
    
  }

void serialEvent() {
    while (Serial.available()) {
        // get the new byte:
        receivedCode = Serial.read();
        if((receivedCode & 0xF0) == id)
          {
            receivedCode = receivedCode & 0x0F;
            
            if(receivedCode == 0x00)
              allPinsDown();
          
            if((receivedCode & 0x01) == 0x01)
              digitalWrite(Relay_1, LOW);
            else
              digitalWrite(Relay_1, HIGH);
        
            if((receivedCode & 0x02) == 0x02)
              digitalWrite(Relay_2, LOW);
            else
              digitalWrite(Relay_2, HIGH);
          
            if((receivedCode & 0x04) == 0x04)
              digitalWrite(Relay_3, LOW);
            else
              digitalWrite(Relay_3, HIGH);
          
            if((receivedCode & 0x08) == 0x08)
              digitalWrite(Relay_4, LOW);
            else
              digitalWrite(Relay_4, HIGH);
          }
     }
  }
