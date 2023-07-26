#include <SoftwareSerial.h>

SoftwareSerial portOne(10, 11); // RX, TX
SoftwareSerial portTwo(8, 9);
SoftwareSerial portThree(6, 7);

void setup() {
  Serial.begin(9600);
  while (!Serial) 
    {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  portOne.begin(9600);
  portTwo.begin(9600);
  portThree.begin(9600);
}

void loop() { 
  portOne.println("Volume = 225.5 L");
  //delay(1);
  portTwo.println("Fat = 25, Degree = 10");
  //delay(1);
  portThree.println("SNF = 4.5, Weight = 100");
  //delay(1);
  /*
  // run over and over
  portOne.listen();
  Serial.println("Data from port one:");
  while (portOne.available()<0);
  while (portOne.available()>0) 
    {
      char inChar = portOne.read();
      Serial.write(inChar);
    }    
  
  portTwo.listen();
  Serial.println("Data from port two:");
  while (portTwo.available()<0);
  while (portTwo.available()>0) 
    {
      char inChar = portTwo.read();
      Serial.write(inChar);
    }

  portThree.listen();
  Serial.println("Data from port three:");
  while (portThree.available()<0);
  while (portThree.available()>0) 
    {
      char inChar = portThree.read();
      Serial.write(inChar);
    }
  */
}

