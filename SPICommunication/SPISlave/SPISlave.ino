#include <LiquidCrystal.h>
#include <stdlib.h>
#include <SPI.h>

#define SCK_PIN  52
#define MISO_PIN 50
#define MOSI_PIN 51
#define SS_PIN   53  
int i=0;
char k[20],v[20];
int valid=0;
LiquidCrystal lcd(22, 23, 24, 25, 26, 27, 28);
void setup (void)
{
  Serial.begin (9600);  
  pinMode(MOSI_PIN, INPUT);
  pinMode(MISO_PIN, OUTPUT);
  pinMode(SCK_PIN, INPUT);
  pinMode(SS_PIN, INPUT);
  SPCR |= _BV(SPE);
  SPCR != _BV(SPIE);

  SPI.attachInterrupt();

}
ISR (SPI_STC_vect)
{
char c = SPDR;  
if(c=='\n')
{
 lcd.clear();
 Serial.println(k);
lcd.print(k);
  i=0;
}
else if(c!='\n')
{
  k[i]=c;
  i++;
}
}


void loop (void)
{
 
while(1);
}
