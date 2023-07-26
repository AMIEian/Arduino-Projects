//Analog - A0, A1
#include <TimerOne.h>
#include <EEPROM.h>

int RTD_1 = A0;
int RTD_2 = A1;

String inputString;

int RTD_1_Value = 0;
int RTD_2_Value = 0;
float setLowTemp = 0.0;
float setHighTemp = 9.9;
float currentLowTemp = 0.0;
float currentHighTemp = 9.9;
bool lowTempAlarm = false;
bool highTempAlarm = false;

int zero_reading[] = {285, 256};
int full_reading[] = {504, 477}; //182+234;
int offset[] = {0, 0};
float gain[] = {1.0, 1.0};

/*
bool ch1_cal_zero = false;
bool ch1_cal_full = false;
bool ch2_cal_zero = false;
bool ch2_cal_full = false;
*/

unsigned long ch1_total = 0;
unsigned long ch2_total = 0;
int number_of_readings = 0;

const unsigned int table[] = {
  0,0,1,2,3,3,4,4,5,5, // 0
  6,6,7,7,8,8,9,9,10,10, // 10
  10,10,10,11,12,13,14,15,16,17, // 20
  18,19,20,20,20,21,21,22,22,23, // 30
  23,24,24,25,25,26,26,27,27,28, // 40
  28,29,29,30,30,30,30,30,30,31, // 50
  31,32,32,33,33,34,34,35,35,35, // 60
  36,36,37,37,38,38,39,39,40,40, // 70
  40,40,40,40,41,42,43,44,45,45, // 80
  46,47,48,49,50,50,50,50,50,51, // 90
  51,52,52,53,53,54,54,55,55,56, // 100
  57,57,58,58,59,59,60,60,60,60, // 110
  60,61,62,63,64,65,65,66,67,68, // 120
  69,69,70,70,70,70,70,71,71,72, // 130
  72,73,73,74,74,75,75,75,76,76, // 140
  77,77,78,78,79,79,80,80,80,80, // 150
  80,81,82,83,84,85,86,87,88,89, // 160
  90,90,90,90,91,91,92,92,93,93, // 170
  94,94,95,95,95,96,96,97,97,98, // 180
  98,99,99,100,100,100,100,100,101,101, // 190
  102,102,103,103,104,104,105,105,105,105, // 200
  106,106,107,107,108,108,109,109,110,110, // 210
  110,110,110,111,111,112,112,113,113,114, // 220
  114,115,115,116,116,117,117,118,118,119, // 230
  119,120,120,120,120,120,120,120,120,120, // 240
  120,120,120                              // 250
} ;
int ch1_avg, ch2_avg;
int inti;
double doublei;
int index;

void takeNewReadings()
{ 
  //Take the reading and add it to total until 32 readings
  //analogReference(EXTERNAL);
  RTD_1_Value = analogRead(RTD_1);
  ch1_total = ch1_total + RTD_1_Value;
  RTD_2_Value = analogRead(RTD_2);
  ch2_total = ch2_total + RTD_2_Value;
  number_of_readings++;
  if(number_of_readings >= 32)
    {
      ch1_avg = (int)ch1_total >> 5;
      ch2_avg = (int)ch2_total >> 5;
      ch1_total = 0;
      ch2_total = 0;
      number_of_readings = 0;
      //For Channel 0.
      //Serial.println(ch1_avg);
      //Serial.println(ch2_avg);
      //Serial.println("******");
      if(zero_reading[0] > ch1_avg)
          inti = zero_reading[0] - ch1_avg;
      else
          inti = ch1_avg - zero_reading[0];
      
      doublei = inti;
      doublei = doublei * 200;
      if(full_reading[0] > zero_reading[0]) 
           inti = full_reading[0] - zero_reading[0];  //416 - 234
      else
          inti = 1;
      
      doublei = doublei/inti;  //(adc - calz)/(calf - calz)*200; 
            
      if(doublei > 220) 
          index = 220 ; // 220 + 20 = 240 lkup max
      else 
          index = doublei ;  

      if(zero_reading[0] > ch1_avg)
          {   
            if(20 > index)
                index = 20 - index ;
            else
                index = 0 ;
          }
      else
          index = 20 + index;  //110 + 20 = 130;
      
      currentLowTemp = 0.0;
      //Serial.println(index);
      //Serial.println((float)table[index]);
      currentLowTemp = (float)table[index];  //69  5.9 
      //Serial.println(currentLowTemp);
      doublei = currentLowTemp ;
      doublei = doublei * gain[0] * 1000;
      currentLowTemp = doublei / 1000 ;
      currentLowTemp = currentLowTemp + offset[0] ;
      if(currentLowTemp >= 10)  
          currentLowTemp = currentLowTemp - 10 ;
      else
          currentLowTemp = -10 ;
      
      currentLowTemp = currentLowTemp / 10;
      Serial.print(currentLowTemp);
      Serial.print('a');

      //For Channel 1.
      if(zero_reading[1] > ch2_avg)
          inti = zero_reading[1] - ch2_avg;
      else
          inti = ch2_avg - zero_reading[1];

      doublei = inti;
      doublei = doublei * 200;

      if(full_reading[1] > zero_reading[1]) 
           inti = full_reading[1] - zero_reading[1];  //416 - 234
      else
          inti = 1;
      
      doublei = doublei/inti;  //(adc - calz)/(calf - calz)*200; 
         
      if(doublei > 220) 
          index = 220 ; // 220 + 20 = 240 lkup max
      else 
          index = doublei ;  

      if(zero_reading[1] > ch2_avg)
          {   
            if(20 > index)
                index = 20 - index ;
            else
                index = 0 ;
          }
      else
          index = 20 + index;  //110 + 20 = 130;
      //Serial.println(index);
      currentHighTemp = 0.0;
      currentHighTemp = (float)table[index];  //69  5.9 
      doublei = currentHighTemp ;
      doublei = doublei * gain[1] * 1000;
      currentHighTemp = doublei / 1000 ;
      currentHighTemp = currentHighTemp + offset[1] ;
      if(currentHighTemp >= 10)  
          currentHighTemp = currentHighTemp - 10 ;
      else
          currentHighTemp = -10 ;
      
      currentHighTemp = currentHighTemp / 10;
      //Serial.println(currentHighTemp);
      //Serial.println("--------");
      Serial.print(currentHighTemp);
      Serial.print('b');
      /*
      //If channel 1 calibration is going on
      if(ch1_cal_zero == true)
        {
          EEPROM.put(100,ch1_avg);
          ch1_cal_zero == false;
        }
      if(ch1_cal_full == true)
        {
          EEPROM.put(110,ch1_avg);
          ch1_cal_full == false;
        }
        
      //If channel 2 calibration is going on
      if(ch2_cal_zero == true)
        {
          EEPROM.put(120,ch2_avg);
          ch2_cal_zero == false;
        }
      if(ch2_cal_full == true)
        {
          EEPROM.put(130,ch2_avg);
          ch2_cal_full == false;
        }
      */
    }  
  //currentLowTemp = (float)random(0, 99)/(float)10;
  //currentHighTemp = (float)random(0, 99)/(float)10;
}

void setup() 
{
  Serial.begin(9600);
  /*
  EEPROM.get(0,offset[0]);
  if(isnan(offset[0]))
    offset[0] = 0;  

  EEPROM.get(10,offset[1]);
  if(isnan(offset[1]))
    offset[1] = 1.0;  

  EEPROM.get(20,gain[0]);
  if(isnan(gain[0]))
    gain[0] = 0;  

  EEPROM.get(30,gain[1]);
  if(isnan(gain[1]))
    gain[1] = 1.0;
  */
  Timer1.initialize(20000);
  Timer1.attachInterrupt(takeNewReadings);
}

void loop() {
  
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    if (inChar == 'a') {
      offset[0] = inputString.toFloat();
      EEPROM.put(0,offset[0]);
      inputString = "";
    }
    if (inChar == 'b') {
      offset[1] = inputString.toFloat();
      EEPROM.put(10,offset[1]);
      inputString = "";
    }
    if (inChar == 'c') {
      gain[0] = inputString.toFloat();
      EEPROM.put(20,gain[0]);
      inputString = "";
    }
    if (inChar == 'd') {
      gain[1] = inputString.toFloat();
      EEPROM.put(30,gain[1]);
      inputString = "";
    }
  }
}
