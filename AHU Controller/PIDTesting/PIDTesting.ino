#include <Nextion.h>
#include <PID_v1.h>
#include "variables.h"

#define nextion Serial
Nextion myNextion(nextion, 9600);
int screenCounter = 0;

String inputString = ""; 

//COMMUNICATION VARIABLES
int16_t counts[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int16_t outputCounts[25] = {0, 1, 0, 50, 0, 0, 0, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0, 5, 0, 0, 0};

//ANALOG INPUTS ARRAYS
int16_t RATemp[4];
int16_t RATempPID[5];

//Define Variables we'll be connecting to
double tempSetpoint, tempInput, tempOutput;

//Specify the links and initial tuning parameters
double tempKp=0, tempKi=5, tempKd=0;
PID tempPID(&tempInput, &tempOutput, &tempSetpoint, tempKp, tempKi, tempKd, DIRECT);

void setup() 
{
  Serial.begin(9600);
  myNextion.init(); // send the initialization commands for Page 0
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  ahuTurnOn();
}

void loop() 
{
  getNewData();
  RATempPIDRuntime();
  sendNewValues();
  
  switch(screenCounter)
    {
      case 1 :
        updateIOScreen_1();
        break;
      case 2 :
        updateIOScreen_2();
        break;
      case 3 :
        updateIOScreen_3();
        break;
      case 4 :
        updateIOScreen_4();
        break;
    }
  serialEvent();
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (inChar == 'a') {
      screenCounter = 1;
    }
    else if (inChar == 'b') {
      screenCounter = 2;
    }
    else if (inChar == 'c') {
      screenCounter = 3;
    }
    else if (inChar == 'd') {
      screenCounter = 4;
    }else if (inChar == 'z') {
      screenCounter = 5;
    }else if (inChar == '1') {
      ahuTurnOn();
    }else if (inChar == '0') {
      ahuTurnOff();
    }
    else {
      screenCounter = 0;
    }
  }
}

void ahuTurnOn()
{
  delay(10000);
  tempSetpoint = myNextion.getComponentValue(set_temp);
  tempSetpoint = tempSetpoint * (ADC_MAX_COUNTS/RATemp[full]);
  tempInput = counts[RA_Temp_AI];
  tempOutput = outputCounts[Chilled_Water_AO];
  tempPID.SetMode(AUTOMATIC);
  tempPID.SetTunings(tempKp, tempKi, tempKd);
}

void ahuTurnOff()
{
  tempPID.SetMode(MANUAL);
}

void getRATempValues()
{
  RATemp[enabled] = myNextion.getComponentValue(RATempEnabled);
  RATemp[dt] = myNextion.getComponentValue(RATempDt);
  RATemp[zero] = myNextion.getComponentValue(RATempZero);
  RATemp[full] = myNextion.getComponentValue(RATempFull);

  outputCounts[RA_Temp_AI_Damp_Time] = RATemp[dt];
  outputCounts[RA_Temp_Lower] = RATemp[zero];
  outputCounts[RA_Temp_Higher] = RATemp[full];
}

void getRATempPIDValues()
{
  RATempPID[pidMode] = myNextion.getComponentValue(RATempPIDMode);
  RATempPID[kp] = myNextion.getComponentValue(RATempPIDKp);
  RATempPID[ki] = myNextion.getComponentValue(RATempPIDKi);
  RATempPID[kd] = myNextion.getComponentValue(RATempPIDKd);
  RATempPID[kt] = myNextion.getComponentValue(RATempPIDKt);
}

void RATempPIDRuntime()
{
  tempInput = counts[RA_Temp_AI];
  tempPID.Compute();
  outputCounts[Chilled_Water_AO] = tempOutput;
  //Send data by serial for plotting 
  Serial.print("In = ");
  Serial.print(counts[RA_Temp_AI]);
  Serial.print(" Out = ");
  Serial.println(outputCounts[Chilled_Water_AO]);
}
