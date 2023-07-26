void setup()
{
  pinMode(2, OUTPUT);
}

void loop()
{
  rf_send('0');
  rf_send('1');
  rf_send('2');
  rf_send('3');
  rf_send('4');
  rf_send('5');
  rf_send('6');
  rf_send('7');
  rf_send('8');
  rf_send('9');
  rf_send('#');
  delay(10000);
}

void rf_send(byte input)
{
  int i;
  for(i=0; i<20; i++)
    {
      digitalWrite(2, HIGH);
      delayMicroseconds(500);
      digitalWrite(2, LOW);
      delayMicroseconds(500);
    }
    
  digitalWrite(2, HIGH);
  delayMicroseconds(3000);
  digitalWrite(2, LOW);
  delayMicroseconds(500);
  
  for(i=0; i<8; i++)
    {
      if(bitRead(input,i)==1)
        digitalWrite(2, HIGH);
      else
        digitalWrite(2, LOW);
      delayMicroseconds(500);
  
      if(bitRead(input,i)==1)
        digitalWrite(2, LOW);
      else
        digitalWrite(2, HIGH);
      delayMicroseconds(500);
    }
  digitalWrite(2, LOW);
}



