#include <Bounce2.h>

Bounce prPlus = Bounce();
Bounce prMinus = Bounce();
Bounce flPlus = Bounce();
Bounce flMinus = Bounce();
Bounce onOff = Bounce();
Bounce co2Air = Bounce();
Bounce verTroc = Bounce();
Bounce volReset = Bounce();

void setup() 
{
  Serial.begin(9600);
  
  pinMode(2, INPUT);  //Pressure Plus
  pinMode(3, INPUT);  //Pressure Minus
  pinMode(4, INPUT);  //Flow Plus
  pinMode(5, INPUT);  //Flow Minus
  pinMode(6, INPUT);  //Start Stop
  pinMode(7, INPUT);  //CO2 Air
  pinMode(8, INPUT);  //Verese Trocar
  pinMode(9, INPUT);  //Volume Reset

  prPlus.attach(2);
  prPlus.interval(5);
  prMinus.attach(3);
  prMinus.interval(5);
  flPlus.attach(4);
  flPlus.interval(5);
  flMinus.attach(5);
  flMinus.interval(5);
  onOff.attach(6);
  onOff.interval(5);
  co2Air.attach(7);
  co2Air.interval(5);
  verTroc.attach(8);
  verTroc.interval(5);
  volReset.attach(9);
  volReset.interval(5);
}

void loop()
{
  prPlus.update();
  if(prPlus.read() == LOW)
    {
      Serial.print("65 0 4 1 ffff ffff ffff");
      while(digitalRead(2) == LOW)
        delay(10);
    }
    
  prMinus.update();
  if(prMinus.read() == LOW)
    {
      Serial.print("65 0 2 1 ffff ffff ffff");
      while(prMinus.read() == LOW)
        prMinus.update();
    }
    
  flPlus.update();
  if(flPlus.read() == LOW)
    {
      Serial.print("65 0 5 1 ffff ffff ffff");
      while(flPlus.read() == LOW)
        flPlus.update();
    }
    
  flMinus.update();
  if(flMinus.read() == LOW)
    {
      Serial.print("65 0 3 1 ffff ffff ffff");
      while(flMinus.read() == LOW)
        flMinus.update();
    }
    
  onOff.update();
  if(onOff.read() == LOW)
    {
      Serial.print("65 0 e 1 ffff ffff ffff");
      while(onOff.read() == LOW)
        onOff.update();
    }
    
  co2Air.update();
  if(co2Air.read() == LOW)
    {
      Serial.print("65 0 f 1 ffff ffff ffff");
      while(co2Air.read() == LOW)
        co2Air.update();
    }
    
  verTroc.update();
  if(verTroc.read() == LOW)
    {
      Serial.print("65 0 10 1 ffff ffff ffff");
      while(verTroc.read() == LOW)
        verTroc.update();
    }
    
  volReset.update();
  if(volReset.read() == LOW)
    {
      Serial.print("65 0 d 1 ffff ffff ffff");
      while(volReset.read() == LOW)
        volReset.update();
    }
}
