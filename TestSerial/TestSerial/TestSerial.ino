void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  Serial1.begin(2400);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  while(Serial1.available()>0)
    {
      // read serial port "1"
      char dataport1 = Serial1.read();
      Serial.write(dataport1);
    }

  while(Serial2.available()>0)
    {
      // read serial port "2"
      char dataport2 = Serial2.read();
      Serial.write(dataport2);
    }

  while(Serial3.available()>0)
    {
      // read serial port "3"
      char dataport3 = Serial3.read();
      Serial.write(dataport3);
    }

  /*
  Serial1.println("222.55 L");                 // wait for a second
  delay(100);
  Serial2.println("Nitiin N Sangale"); 
  delay(100);
  Serial3.println("FAT = 10, DEGREE = 9, SNF = 1.5"); 
  delay(100);
  */
}
