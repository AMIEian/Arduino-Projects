void setup() 
{
  Serial.begin(9600);
  pinMode(4, INPUT);
}

void loop() 
{
  if(digitalRead(4) == HIGH)
  {
    delay(10);
    if(digitalRead(4) == HIGH)
    {
      Serial.println("X");
      delay(3000);
      Serial.println("Y");
    }    
  }
}
