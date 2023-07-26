#include <Bounce2.h>
#include <TimerOne.h>

Bounce Button_1 = Bounce();
Bounce Button_2 = Bounce();
Bounce Button_3 = Bounce();
Bounce Button_4 = Bounce();

const int switch_1 = 2;
const int switch_2 = 3;
const int switch_3 = 4;
const int switch_4 = 5;

char txCode = 0x00;
char oldTxCode = 0x00;
char txData = 0x00;
char id = 0x20;

void TxData()
{ 
  txData = id | txCode;
  Serial.print(txData);
}

void setup(){
  Serial.begin(9600);

  pinMode(switch_1, INPUT);
  pinMode(switch_2, INPUT);
  pinMode(switch_3, INPUT);
  pinMode(switch_4, INPUT);

  Button_1.attach(switch_1);
  Button_1.interval(5);
  Button_2.attach(switch_2);
  Button_2.interval(5);
  Button_3.attach(switch_3);
  Button_3.interval(5);
  Button_4.attach(switch_4);
  Button_4.interval(5);

  Timer1.initialize( 1000000 );           //period in microseconds to call TxData.
  Timer1.attachInterrupt( TxData );   //attach the Timer1 interrupt to TxData
}

void loop(){
  Button_1.update();
  Button_2.update();
  Button_3.update();
  Button_4.update();

  if(Button_1.read() == HIGH)
    txCode = txCode | 0x01;
  else
    txCode = txCode & 0x0E;
    
  if(Button_2.read() == HIGH)
    txCode = txCode | 0x02;
  else
    txCode = txCode & 0x0D;
  
  if(Button_3.read() == HIGH)
    txCode = txCode | 0x04;
  else
    txCode = txCode & 0x0B;
    
  if(Button_4.read() == HIGH)
    txCode = txCode | 0x08;
  else
    txCode = txCode & 0x07;

  if(txCode != oldTxCode)
    {
      oldTxCode = txCode;
      txData = id | txCode;
      Serial.print(txData);
    }
}

