/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CNS, CE
const byte address[6] = "00001";
int count = 0;
//char text[20];
String num;
char data;

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}
void loop() {
  //const char text[] = "Hello World";
  //radio.write(&text, sizeof(text));
  //num = String(count);
  //num.toCharArray(text,10);
  data = 'H';
  radio.write(&data, sizeof(data));
  delay(1000);
  data = 'L';
  radio.write(&data, sizeof(data));
  delay(1000);
}
