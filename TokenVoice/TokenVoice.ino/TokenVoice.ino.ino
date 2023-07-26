#include <SdFat.h>
SdFat sd;

#define SD_ChipSelectPin 4  //use digital pin 4 on arduino Uno, nano etc, or can use other pins
#include <TMRpcm.h>          //  also need to include this library...
#include <SPI.h>

TMRpcm audio;   // create an object for use in this sketch

void setup(){
  audio.speakerPin = 9;
    
  Serial.begin(9600);
  
  if (!sd.begin(4, SPI_FULL_SPEED)) { return; 
  }else{ Serial.println("SD OK"); }
  audio.volume(10);  
}

void loop(){  
  audio.play("tn.wav");
  delay(2000);
  audio.play("0.wav");
  delay(2000);
  audio.play("1.wav");
  delay(2000);
  audio.play("2.wav");
  delay(2000);
  audio.play("3.wav");
  delay(2000);
  audio.play("4.wav");
  delay(2000);
  audio.play("5.wav");
  delay(2000);
  audio.play("6.wav");
  delay(2000);
  audio.play("7.wav");
  delay(2000);
  audio.play("8.wav");
  delay(2000);
  audio.play("9.wav");
  delay(2000);
}
