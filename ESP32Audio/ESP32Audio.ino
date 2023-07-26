#include "XT_DAC_Audio.h";
#include "bell.h";
#include "tn.h";
#include "e0.h";
#include "e1.h";
#include "e2.h";
#include "e3.h";
#include "e4.h";
#include "e5.h";
#include "e6.h";
#include "e7.h";
#include "e8.h";
#include "e9.h";

XT_Wav_Class dingDong(bell);
XT_Wav_Class tokenNo(tn);
XT_Wav_Class Zero(e0);          
XT_Wav_Class One(e1);          
XT_Wav_Class Two(e2);           
XT_Wav_Class Three(e3);           
XT_Wav_Class Four(e4);           
XT_Wav_Class Five(e5);       
XT_Wav_Class Six(e6);        
XT_Wav_Class Seven(e7);          
XT_Wav_Class Eight(e8);           
XT_Wav_Class Nine(e9);   

XT_DAC_Audio_Class DacAudio(25,0);    // Create the main player class object. 
                                      // Use GPIO 25, one of the 2 DAC pins and timer 0

XT_Sequence_Class Sequence; 

void setup() {
  Serial.begin(115200); 
  Sequence.AddPlayItem(&dingDong);      // Add the first sound item, this will play first
  Sequence.AddPlayItem(&tokenNo);             // Add the music score, this will play after the first item 
  Sequence.AddPlayItem(&Zero);
  Sequence.AddPlayItem(&One);
  Sequence.AddPlayItem(&Two);
  Sequence.AddPlayItem(&Three);
  Sequence.AddPlayItem(&Four);
  Sequence.AddPlayItem(&Five);
  Sequence.AddPlayItem(&Six);
  Sequence.AddPlayItem(&Seven);
  Sequence.AddPlayItem(&Eight);
  Sequence.AddPlayItem(&Nine);
  Sequence.RepeatForever=false;              // make the sequence repeat forever
  DacAudio.Play(&Sequence);
}

void loop() {
  DacAudio.FillBuffer();
}
