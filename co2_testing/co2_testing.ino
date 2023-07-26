#include <Nextion.h>
#include <EEPROM.h>
#include <TimerOne.h>

#define nextion Serial

#define ENABLED true
#define DISABLED false
#define VERESS true
#define TROCAR false
#define CO_2 true
#define AIR false
#define OPENED true
#define CLOSED false

//Pin Assignment
const int LED = 13;
const int controlVolve = 12;
const int exhaustVolve = 11;
const int motor = 10;
const int heater = 9;
const int buzzer = 5;
const int pressureInput = A0;
const int temperatureInput = A1;
const int cylinderSwitch = 8;

Nextion myNextion(nextion, 9600); //create a Nextion object named myNextion using the nextion serial port @ 9600bps

//Display Variables
const String pressureDisplay = "act_pres";
const String setPressureDisplay = "set_pres";
const String flowDisplay = "act_flow";
const String setFlowDisplay = "set_flow";
const String temperatureDisplay = "temp";
const String volumeCo2Display = "vol";
const String cylinderPic = "cylinder";
const String onOffButton = "on_off";
const String co2AirButton = "co2_air";
const String verTrocButton = "ver_troc";

const int fullCylinder = 9;
const int emptyCylinder = 10;
const int offButton = 7;
const int onButton = 8;
const int co2Button = 13;
const int airButton = 14;
const int verButton = 5;
const int trocButton = 6;

//Controling Variables
int setPressure = 0;
int actualPressure = 0;
int setFlow = 0;
int actualFlow = 0;
int tempFlow = 0;
int temperature = 30;
float real_volumeCo2 = 0;
int volumeCo2 = 0;
int timer = 0;  //Timer to control on and off periode of volves
int temperatureTimer = 1000;  //Takes temperature reading after every 1 sec.
int beepTimer = 0;
int displayTimer = 0;
int displayUpdateTimer = 0;
unsigned long currentTime = 0;
int oldPressure = 0;
int oldFlow = 0;
int oldTemperature = 0;
int oldvolumeCo2 = 0;

boolean control = DISABLED;
boolean needleType = TROCAR;
boolean outputType = CO_2;
boolean volve = CLOSED;
boolean highPressureAlarm = false;
int hand_flag = 0;

boolean pressureChange = false;
boolean flowChange = false;
boolean volumeCo2Change = false;
boolean temperatureChange = false;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(controlVolve, OUTPUT);
  pinMode(exhaustVolve, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(heater,OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(cylinderSwitch, INPUT);
  pinMode(pressureInput, INPUT);
  pinMode(temperatureInput, INPUT);

  digitalWrite(LED, LOW);
  digitalWrite(controlVolve, LOW);
  digitalWrite(exhaustVolve, LOW);
  digitalWrite(motor, LOW);
  digitalWrite(buzzer, LOW);
  
  Serial.begin(9600);
  myNextion.init(); // send the initialization commands for Page 0
  Timer1.initialize( 1000 );
  Timer1.attachInterrupt( tmrISR );
  // EEPROM.get(0,setPressure);
  setPressure = 12;
  actualPressure = 0;
  myNextion.setComponentText(pressureDisplay, numToString(actualPressure));
  myNextion.setComponentText(setPressureDisplay, numToString(setPressure));
  EEPROM.get(10,setFlow);
  myNextion.setComponentText(setFlowDisplay, numToString(setFlow));
  EEPROM.get(20,volumeCo2);
  myNextion.setComponentText(volumeCo2Display, numToString(volumeCo2));
  real_volumeCo2 = volumeCo2;
  actualFlow = 0;
  myNextion.setComponentText(flowDisplay, numToString(actualFlow));
  myNextion.setComponentText(temperatureDisplay, numToString(temperature));
}

void loop() {
  if(beepTimer <= 0)
    {
      beepTimer = 0;
      if(highPressureAlarm == false)
        digitalWrite(buzzer, LOW);
    }
  if(displayTimer <= 0)
    {
      checkTouchScreen();
      displayTimer = 100;//250;
    }  
  if(displayUpdateTimer <= 0)
    {
      updateDisplay();
      displayUpdateTimer = 500;//1000;
    }    
  if(control == ENABLED)
    {
      if(timer <= 0)
        {
          if(volve == CLOSED)
            {
              checkPressureAndVolveOpen();
            }
          else
            {
              calculateOffTimeAndVolveClose();
            }
        }
    }
  else
    {
      if(timer <= 0)
        {
          monitorPressureAndUpdateDisplay();
          digitalWrite(heater,LOW);
          digitalWrite(motor,LOW);
          hand_flag = 0;
          digitalWrite(LED,LOW);
          //digitalWrite(exhaustVolve,HIGH);
         
        }
    }
  //IF AIR IS NOT USED CHECK CO2 BLOCK TEMPERATURE AFTER EVERY 15 MS
  if( outputType == CO_2 )
    {
      if(temperatureTimer <= 0)
        {
          monitorAndControlTemperature();
        }
    }
  else
    {
      temperatureTimer = 0;
    }
}

void tmrISR()
{ 
  //Serial.println("Pressure Minus");
  timer = timer - 1;
  temperatureTimer = temperatureTimer - 1;
  beepTimer = beepTimer - 1;
  displayTimer = displayTimer - 1;
  displayUpdateTimer = displayUpdateTimer - 1;
}

void updateDisplay()
{
  if(flowChange)
  {
     myNextion.setComponentText(flowDisplay, numToString(actualFlow));
     flowChange = false;
  }
  if(pressureChange)
  {
      myNextion.setComponentText(pressureDisplay,numToString(actualPressure));
      pressureChange = false;
  }
  if(volumeCo2Change)
  {
      myNextion.setComponentText(volumeCo2Display, numToString(volumeCo2));
      volumeCo2Change = false;
  }
  if(temperatureChange)
  {
      myNextion.setComponentText(temperatureDisplay, numToString(temperature));
      temperatureChange = false;
  }
  
  if(digitalRead(cylinderSwitch) == HIGH)
    myNextion.setComponentPicture(cylinderPic, fullCylinder);
  else
    myNextion.setComponentPicture(cylinderPic, emptyCylinder);

  if(control == ENABLED)
    myNextion.setButtonPic(onOffButton, onButton);
  else
    myNextion.setButtonPic(onOffButton, offButton);

  if(needleType == VERESS)
    myNextion.setButtonPic(verTrocButton, verButton);
  else
    myNextion.setButtonPic(verTrocButton, trocButton);

  if(outputType == CO_2)
    myNextion.setButtonPic(co2AirButton, co2Button);
  else
    myNextion.setButtonPic(co2AirButton, airButton);
}

String numToString(int num)
{
  if(num < 10)
      return String(0) + String(num);
  else
      return String(num);
}

void checkTouchScreen()
{
  String message = myNextion.listen(); //check for message
  
  if (message == "65 0 2 1 ffff ffff ffff") {
    //Serial.println("Pressure Minus");
    if(setPressure > 0)
      setPressure--;
    else
      setPressure = 0;
    //Serial.println(numToString(setPressure));  
    //Play Beep
    digitalWrite(buzzer,HIGH);
    beepTimer = 250;//            
    //Update Display
    myNextion.setComponentText(setPressureDisplay, numToString(setPressure));
    EEPROM.put(0,setPressure);
  }
  
  if (message == "65 0 3 1 ffff ffff ffff") {
    //Serial.println("Flow Minus");
    if(setFlow > 0)
      setFlow--;
    else
      setFlow = 0;
    //Serial.println(numToString(setFlow));  
    //Play Beep
    digitalWrite(buzzer,HIGH);
    beepTimer = 250;
    //Update Display
    myNextion.setComponentText(setFlowDisplay, numToString(setFlow));
    EEPROM.put(10,setFlow);
  }
  
  if (message == "65 0 4 1 ffff ffff ffff") {
    //Serial.println("Pressure Plus");
    if(setPressure < 30)
      setPressure++;
    else
      setPressure = 30;
    //Serial.println(numToString(setPressure));  
    //Play Beep
    digitalWrite(buzzer,HIGH);
    beepTimer = 250;
    //Update Display
    myNextion.setComponentText(setPressureDisplay, numToString(setPressure));
    EEPROM.put(0,setPressure);
  }
  
  if (message == "65 0 5 1 ffff ffff ffff") {
    //Serial.println("Flow Plus");
    if(setFlow < 30)
      setFlow++;
    else
      setFlow = 30;
    //Serial.println(numToString(setFlow));
    //Play Beep
    digitalWrite(buzzer,HIGH);
    beepTimer = 250;
    //Update Display
    myNextion.setComponentText(setFlowDisplay, numToString(setFlow));
    EEPROM.put(10,setFlow);
  }
  
  if (message == "65 0 f 1 ffff ffff ffff" && control== DISABLED){
    //Serial.println("Air / CO2");
    if(outputType == CO_2)
      outputType = AIR;
    else
      outputType = CO_2;
    //Serial.println(outputType);

    //Change button picture
    if(outputType == CO_2)
      myNextion.setButtonPic(co2AirButton, co2Button);
    else
      myNextion.setButtonPic(co2AirButton, airButton);
    
    //Play Beep
    digitalWrite(buzzer,HIGH);
    beepTimer = 250;
  }
  
  if (message == "65 0 10 1 ffff ffff ffff") {
    //Serial.println("VER / TROC");
    if(needleType == TROCAR)
      {
        needleType = VERESS;
        tempFlow = setFlow;
        setFlow = 5;
      }
    else
      {
        setFlow = tempFlow;
        needleType = TROCAR;
      }
    //Serial.println(needleType);
    
    myNextion.setComponentText(setFlowDisplay, numToString(setFlow));
    
    //Change button picture
    if(needleType == VERESS)
      myNextion.setButtonPic(verTrocButton, verButton);
    else
      myNextion.setButtonPic(verTrocButton, trocButton);
    
    //Play Beep
    digitalWrite(buzzer,HIGH);
    beepTimer = 250;
  }
  
  if (message == "65 0 e 1 ffff ffff ffff") {
    //Serial.println("ON / OFF");
    if(control == DISABLED)
      control = ENABLED;
    else
    {
      control = DISABLED;
      digitalWrite(controlVolve,LOW);
      digitalWrite(controlVolve,LOW);
      actualFlow = 0; 
       if(oldFlow != actualFlow)
        {
          flowChange = true;
          //myNextion.setComponentText(pressureDisplay,numToString(actualFlow));
          oldFlow = actualFlow;
        }/////////////////////////////////////////////////////////////////////////////
    }     
    //Serial.println(control);

    //Change button picture
    if(control == ENABLED)
      myNextion.setButtonPic(onOffButton, onButton);
    else
      myNextion.setButtonPic(onOffButton, offButton);

    //Play Beep
    digitalWrite(buzzer,HIGH);
    beepTimer = 250;
  }

  if (message == "65 0 d 1 ffff ffff ffff") {
    //Serial.println("volumeCo2 Reset");
    volumeCo2 = 0;
    real_volumeCo2 = 0;
    EEPROM.put(20,0);
     
    volumeCo2Change = true;
    //Play Beep
    digitalWrite(buzzer,HIGH);
    beepTimer = 250;
  }
}

void checkPressureAndVolveOpen()
{
  int analog_result = analogRead(pressureInput); // Read result
                              
  analog_result = analog_result - 512;  //NULLIFY OFFSET
                              
  //ADJUSTMENTS FOR REAL CALIBRATION
  analog_result = analog_result >> 1;
  actualPressure = analog_result / 2.5;
                            
  if( actualPressure > 40 )
    {
      digitalWrite(buzzer,HIGH); //TURN ON ALARM
      highPressureAlarm = true;
    }
  if( actualPressure < 20 )
    {
      digitalWrite(buzzer,LOW); //TURN OFF ALARM
      highPressureAlarm = false;
    }
                              
  if( (actualPressure <= (setPressure - 1)) && outputType == AIR ) //TURN ON MOTOR FOR FIX TIME SAY 1 SECOND
    {
      if((setPressure - actualPressure) <= 2)//
        {
          timer = 2000;//
        }
      else
        {
          timer = 1000;  //SET TIMER FOR 1 SEC DELAY//chanded from 800
        }
      
      digitalWrite(motor,HIGH); //TURN ON MOTOR
      volve = OPENED;  //SET VOLVE OPEN
      hand_flag = 0;//RESET HAND FLAG
       digitalWrite(LED,LOW);                             
      //UPDATE ACTUAL FLOW DISPLAY WITH CONSTANT 05 VALUE
      actualFlow = 5;
      if(oldFlow != actualFlow)
        {
          flowChange = true;
          //myNextion.setComponentText(pressureDisplay,numToString(actualFlow));
          oldFlow = actualFlow;
        }
   }
                                 
  //IF ACTUAL PRESSURE IS LESS THAN 12 (UP TO 11) PROGRAM WILL GO IN THIS BLOCK
  if( (actualPressure <= (setPressure - 1)) && outputType == CO_2 )   //TURN ON THE CO2 VOLVE
    {
      if(( setPressure - actualPressure ) > 3 )//...............................////////////////////////////////////////////////
        {
           //CALCULATE TURN ON TIME
          timer = setFlow * 70;// ////////////////////////////////////////
          if(timer > 1500)//////////////////////////////////////////////////////
            timer = 1500;//////////////////////////////////////////////////////////////////////////////////////////
          actualFlow = setFlow;
        }
       if(( setPressure - actualPressure ) <= 3 )
        {
          timer = 200;
          actualFlow = 2;
        }                             
      
      digitalWrite(controlVolve, HIGH); //OPEN THE CONTROL VOLVE
      volve = OPENED;  //SET VOLVE 1
      hand_flag = 0; //RESET HAND FLAG
      
      digitalWrite(LED,LOW);
      //UPDATE ACTUAL FLOW DISPLAY
      if(oldFlow != actualFlow)
        {
          flowChange = true;
          //myNextion.setComponentText(pressureDisplay,numToString(actualFlow));
          oldFlow = actualFlow;
        }
    }
       //IF ACTUAL PRESSURE IS GREATOR THAN OR EQUAL TO 15 PROGRAM WILL GO IN THIS BLOCK 
  if( actualPressure >= (setPressure + 3) && hand_flag < 10)
  
    {
      digitalWrite(exhaustVolve, HIGH); //Turn On Exhaust Volve
      hand_flag = 0;
      digitalWrite(LED,LOW);
      while(actualPressure >= (setPressure ))
        {
          analog_result = analogRead(pressureInput); // Read result
                              
          analog_result = analog_result - 512;  //NULLIFY OFFSET
                              
          //ADJUSTMENTS FOR REAL CALIBRATION
          analog_result = analog_result >> 1;
          actualPressure = analog_result / 2.5;
        if( actualPressure > 40 )
          {
          digitalWrite(buzzer,HIGH); //TURN ON ALARM
          highPressureAlarm = true;
         }
          if( actualPressure < 20 )
         {
            digitalWrite(buzzer,LOW); //TURN OFF ALARM
            highPressureAlarm = false;
          }
        
          
        }
      digitalWrite(exhaustVolve, LOW); //Turn On Exhaust Volve
       
       delay(1);
      actualFlow = 0;
      //UPDATE ACTUAL FLOW DISPLAY
      if(oldFlow != actualFlow)
        {
          flowChange = true;
          //myNextion.setComponentText(pressureDisplay,numToString(actualFlow));
          oldFlow = actualFlow;
        }
    }                     
  //IF ACTUAL PRESSURE IS FROM 12 TO 14 PROGRAM WILL GO IN THIS BLOCK   
  if( actualPressure >= setPressure && actualPressure <= (setPressure + 2) )
      {
       
      hand_flag = hand_flag + 1;//...removed the comments
      
      if(hand_flag >= 10)//...it was 2000..................
        {
          hand_flag = 10;//...it was 2000...............
         
          digitalWrite(LED,HIGH);
           timer = 1000;  //TAKE NEXT READING AFTER 500 MS//...........changed it was 1000
        }
      actualFlow = 0;  //STOP THE FLOW   

                              
      //UPDATE ACTUAL FLOW DISPLAY
      if(oldFlow != actualFlow)
        {
          flowChange = true;
          //myNextion.setComponentText(pressureDisplay,numToString(actualFlow));
          oldFlow = actualFlow;
        }
    }
                                                    
 
                                 
  if(actualPressure > setPressure)
    {
      //IF ACTUAL PRESSURE IS OVER THE SET PRESSURE CHECK WHETHER IT IS GREATOR THAN 1 OR LESS
      if((actualPressure - setPressure) > 1 && (hand_flag < 10))
        {
          //IF ACTUAL PRESSURE IS MORE THAN 2 OVER SET PRESSURE THEN DISPLAY ACTUAL PRESSURE
          //UPDATE PRESSURE DISPLAY
          if(oldPressure != actualPressure)
            {
              pressureChange = true;                                
              //myNextion.setComponentText(pressureDisplay,numToString(actualPressure));
              oldPressure = actualPressure;
            }
        }
      else
        {
          //IF ACTUAL PRESSURE IS NOT MORE THAN 1 OVER SET PRESSURE THEN DISPLAY ACTUAL PRESSURE AS SET PRESSURE
          //UPDATE PRESSURE DISPLAY
          actualPressure = setPressure;
          if(oldPressure != actualPressure)
            {
              pressureChange = true;
              //myNextion.setComponentText(pressureDisplay,numToString(actualPressure));
              oldPressure = actualPressure;
            }
        }
    }
  else
    {
      //IF ACTUAL PRESSURE IS LESS THAN SET PRESSURE THEN DIS[LAY ACTUAL PRESSURE
      //UPDATE PRESSURE DISPLAY
      if(oldPressure != actualPressure)
        {
          pressureChange = true;
          //myNextion.setComponentText(pressureDisplay,numToString(actualPressure));
          oldPressure = actualPressure;
        }
    }
}

void calculateOffTimeAndVolveClose()
{
  if( outputType == CO_2 )
    {
      //TURN OFF VOLVE
      digitalWrite(controlVolve,LOW);
      volve = CLOSED;
                                    
      //CALCULATE TURN OFF TIME
      int temp_time = 0;
      
      temp_time = setFlow * 9;
                                    
      float temp_volumeCo2 = 0.0012 * temp_time;   //CALCULATE volumeCo2 OF C02 SUPPLIED DURING ON TIME OF VOLVE
      real_volumeCo2 = real_volumeCo2 + temp_volumeCo2;  //ADD THIS volumeCo2 TO TOTAL volumeCo2
      
      volumeCo2 = real_volumeCo2;   //GET INTEGER PART OF volumeCo2 FOR DISPLAY PURPOSE
   
      
   
      //UPDATE volumeCo2 DISPLAY
      if(oldvolumeCo2 != volumeCo2)
        {
          volumeCo2Change = true;
          //myNextion.setComponentText(volumeCo2Display, numToString(volumeCo2));
          oldvolumeCo2 = volumeCo2;
        }
      EEPROM.put(20,volumeCo2);
      temp_time = 270 - temp_time;
      timer = 500 + temp_time;
    }
  else
    {
      digitalWrite(motor, LOW); //TURN OFF THE MOTOR
      volve = CLOSED;
      timer = 500;  //SET OFF TIME = 500 MS
    }
}

void monitorPressureAndUpdateDisplay()
{
  int analog_result = analogRead(pressureInput); // Read result
  analog_result = analog_result - 512;  //NULLIFY OFFSET
                              
  //ADJUSTMENTS FOR REAL CALIBRATION
  analog_result = analog_result >> 1;
  actualPressure = analog_result / 2.5;/////////////////////////
  
 delay(10);
  if( actualPressure < 20 )/////////////////////////////////////////////////////////////////
    {
      digitalWrite(buzzer,LOW); //TURN ON 
      highPressureAlarm = false;     
    }
   
  if( actualPressure > 40 )////////////////// added            ///////////////////////////////////////////////
    {
      digitalWrite(buzzer,HIGH); //TURN ON ALARM
      highPressureAlarm = true;     
    }
      
  if( actualPressure < 13 )
    {
      //IF HIGH PRESSURE ALARM IS ON THEN TURN IT OFF
      //if(highPressureAlarm)
       // {
       //   digitalWrite(buzzer,LOW); //TURN OFF ALARM
        //  highPressureAlarm =false ;
       //  }
      //TURN OFF EXHAUST VOLVE
      digitalWrite(exhaustVolve, LOW);
    }
  //UPDATE PRESSURE DISPLAY  
  if(oldPressure != actualPressure)
    {
      pressureChange = true;
      //myNextion.setComponentText(pressureDisplay,numToString(actualPressure));
      oldPressure = actualPressure;
    }
  timer = 10;  
}

void monitorAndControlTemperature()
{
  //TAKE THE TEMPERATURE READING
  int analog_temp_result = analogRead(temperatureInput); // Read result                      
                        
  temperature = analog_temp_result >> 3;
                        
  if(control == ENABLED)
    {
      float real_temp = analog_temp_result / 8;
      if(real_temp >= 38.0)
        {
          digitalWrite(heater,LOW); //TURN OFF HEATER
        }
      if(real_temp <= 37.8)
        {
          digitalWrite(heater,HIGH); //TURN ON HEATER
        }
    }
    
  if(temperature < 38)
    myNextion.setComponentColor(temperatureDisplay, "YELLOW");
  if(temperature >= 50)
    myNextion.setComponentColor(temperatureDisplay, "RED");
  if(temperature >= 38 && temperature < 50)
    myNextion.setComponentColor(temperatureDisplay, "GREEN");

  if(temperature > 38)
    temperature = 38;
    
  //UPDATE TEMERATURE DISPLAY
  if(oldTemperature != temperature)
    {
      temperatureChange = true;
      //myNextion.setComponentText(temperatureDisplay, numToString(temperature));
      oldTemperature = temperature;
    }
  temperatureTimer = 10000;  //10 SEC DELAY FOR NEXT TEMPERATURE READING
}
