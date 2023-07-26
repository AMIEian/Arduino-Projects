#include <Bounce2.h>
#include <TimerOne.h>

Bounce Button_1 = Bounce();
Bounce Button_2 = Bounce();

const int switch_1 = 2;
const int switch_2 = 3;
const int powerLED = 4;
const int motorLED = 5;
const int comLED = 5;

char txCode = 0x00;
char oldTxCode = 0x00;
char txData = 0x00;
byte receivedCode = 0x00;
byte id = 0x20;
char comTime = 5;

void TxData()
{ 
  txData = id | txCode;
  Serial.print(txData);
  comTime = comTime - 1;
  if(comTime < 0)
    {
      comTime = 0;
      digitalWrite(comLED, LOW);
    }
}

void setup(){
  Serial.begin(9600);

  pinMode(switch_1, INPUT);
  pinMode(switch_2, INPUT);
  
  pinMode(powerLED, OUTPUT);
  pinMode(motorLED, OUTPUT);
  pinMode(comLED, OUTPUT);

  digitalWrite(powerLED, LOW);
  digitalWrite(motorLED, LOW);
  digitalWrite(comLED, LOW);
  
  Button_1.attach(switch_1);
  Button_1.interval(5);
  Button_2.attach(switch_2);
  Button_2.interval(5);

  Timer1.initialize( 1000000 );           //period in microseconds to call TxData.
  Timer1.attachInterrupt( TxData );   //attach the Timer1 interrupt to TxData
}

void loop(){
  Button_1.update();
  Button_2.update();

  if(Button_1.read() == HIGH)
    txCode = 0x01;
  else
    txCode = 0x00;
      
  if(Button_2.read() == HIGH)
    txCode = 0x02;
  else
    txCode = 0x00;

  if(txCode != oldTxCode)
    {
      oldTxCode = txCode;
      txData = id | txCode;
      Serial.print(txData);
    }
}

void serialEvent() {
    while (Serial.available()) {
        // get the new byte:
        receivedCode = Serial.read();
        if((receivedCode & 0xF0) == id)
          {
            digitalWrite(comLED, HIGH);
            comTime = 5;
            
            receivedCode = receivedCode & 0x0F;
          
            if((receivedCode & 0x01) == 0x01)
              digitalWrite(powerLED, HIGH);
            else
              digitalWrite(powerLED, LOW);
        
            if((receivedCode & 0x02) == 0x02)
              digitalWrite(motorLED, HIGH);
            else
              digitalWrite(motorLED, LOW);
          }
     }
  }
