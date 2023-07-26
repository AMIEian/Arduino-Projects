#include <EEPROM.h>

#define SwitchOff false
#define SwitchOn true

#define PinHigh true
#define PinLow false

const int VT_Interrupt = 0;   //Pin number 2
const int Open = 5;   //D
const int Close = 3;    //C
const int Stop = 6;   //B
const int OpenRelay = 8;
const int CloseRelay = 7;
const int OpenLED = 12;
const int CloseLED = 11;
const int Cal = 13;
const int LED = 9;
const int Over_Current_LED = 10; 
const int Motor_Current = A3;
const int OpenSwitch = A0;
const int CloseSwitch = A1;
const int StopSwitch = A2;

typedef enum States
{
    Opened,
    Closed,
    Opening,
    Closing,
    Stoped
}State;

volatile State door_State = Stoped;  //This will keep trck of current position of door.

float motor_over_current = 0;
int Over_Current_Timer = 0;
unsigned int mili_seconds = 0;
boolean Auto_Close = true;

static unsigned long last_interrupt_time = 0;

void setup()
{
    Serial.begin(9600);
    
    pinMode(Open,INPUT);
    pinMode(Close,INPUT);
    pinMode(Stop,INPUT);
    pinMode(OpenSwitch,INPUT_PULLUP);
    pinMode(CloseSwitch,INPUT_PULLUP);
    pinMode(StopSwitch,INPUT_PULLUP);
    pinMode(OpenRelay,OUTPUT);
    pinMode(CloseRelay,OUTPUT);
    pinMode(OpenLED,OUTPUT);
    pinMode(CloseLED,OUTPUT);
    pinMode(Cal,INPUT_PULLUP);
    pinMode(LED,OUTPUT);
    pinMode(Motor_Current,INPUT);
    
    digitalWrite(OpenRelay,LOW);
    digitalWrite(CloseRelay,LOW);
    digitalWrite(OpenLED,LOW);
    digitalWrite(CloseLED,LOW);
    
    //LED Blinking
    for(int i = 0; i < 5; i++)
        {
            digitalWrite(LED,HIGH);
            delay(1000);
            digitalWrite(LED,LOW);
            delay(1000);
        }
    
    if(CloseSwitchStatus() == SwitchOn)
        {
            door_State = Closed;
            Serial.println("Door is closed");
            if(digitalRead(Cal) == SwitchOff)
                CalConfig();    
        }
    if(OpenSwitchStatus() == SwitchOn)
        {
            door_State = Opened;
            Serial.println("Door is Opened");
        }
    if(OpenSwitchStatus() == SwitchOff && CloseSwitchStatus() == SwitchOff)
        {
            door_State = Opening;
        }
    
    EEPROM.get(0,motor_over_current);    
        
    attachInterrupt(VT_Interrupt,rxr_interrupt,RISING);
    
    Serial.println("Ready to get inputs...!");
}

void loop()
{
    if(door_State == Opening)
          {
            //Turn on door opening relay
            digitalWrite(OpenRelay,HIGH);
            digitalWrite(OpenLED,HIGH);
            //delay(1000);
            Over_Current_Timer = 0;
            Serial.println("Going into door opening loop");
            while(OpenSwitchStatus() == SwitchOff && door_State == Opening && Over_Current_Timer < 3000)
              {
                //Remain here untill door is fully opened
                //Check overcurrent
                if(check_over_current())
                  {
                    delay(1);  //1 MILI-SEC DELAY
                    Over_Current_Timer = Over_Current_Timer + 1;
                  }
                else
                  {
                    Over_Current_Timer = 0;
                  }
              }
             Serial.println("Door opening loop terminated");           
             // Open_Relay = Off;
             digitalWrite(OpenRelay,LOW);
             digitalWrite(OpenLED,LOW);
                    
            //Now it's out of loop. Check the cause of loop termination             
            if(OpenSwitchStatus() == SwitchOn)
              {
                //Door is opened now
                door_State = Opened;
                Serial.println("Door is Opened");
              }         
            if(Over_Current_Timer >= 3000)
              {
                Serial.println("Door is Tripped");
                while(1)
                  {
                    //Remain here till power is on
                    digitalWrite(Over_Current_LED,HIGH);
                    delay(1000);  //DELAY OF 1 SEC
                    digitalWrite(Over_Current_LED,LOW);
                    delay(1000);  //DELAY OF 1 SEC
                  }
              } 
            delay(1000);             
          }
          
        if(door_State == Closing)
          {
            //Turn On door closing relay
            digitalWrite(CloseRelay,HIGH);
            digitalWrite(CloseLED,HIGH);
            //delay(1000);
            Over_Current_Timer = 0;
            
            Serial.println("Going into door closing loop");
            
            while((CloseSwitchStatus() == SwitchOff && StopSwitchStatus() == SwitchOff) && (door_State == Closing && Over_Current_Timer < 3000))
              {
                //Remain here untill door is fully closed or obstacle occured
                //Check overcurrent
                if(check_over_current())
                  {
                    delay(1);  //1 MILI-SEC DELAY
                    Over_Current_Timer = Over_Current_Timer + 1;
                  }
                else
                  {
                    Over_Current_Timer = 0;
                  }
              }

            Serial.println("Door closing loop terminated");             
            //Stop motor  
            digitalWrite(CloseRelay,LOW);
            digitalWrite(CloseLED,LOW);
            
            //Now it's out of loop. Check the cause of loop termination
            if(StopSwitchStatus() == SwitchOn)
              {
                //Obstacle occured. Open the door
                door_State = Opening;
                Serial.println("Obstacle Detected");
              }
            if(CloseSwitchStatus() == SwitchOn)
              {
                //This means door is fully closed
                door_State = Closed;
                Serial.println("Door is closed");
              }           
            if(Over_Current_Timer >= 3000)
              {
                door_State = Opening;
                Serial.println("Door is Tripped");
                /*
                while(1)
                  {
                    //Remain here till power is on
                    digitalWrite(Over_Current_LED,HIGH);
                    delay(1000);  //DELAY OF 1 SEC
                    digitalWrite(Over_Current_LED,LOW);
                    delay(1000);  //DELAY OF 1 SEC
                  }
                */
              }
            delay(1000);
          }
          
        if(door_State == Opened && Auto_Close)
          {
            //Door is opened. Start 60 Sec timer
            mili_seconds = 0;
            while(door_State == Opened && mili_seconds < 6000 && StopSwitchStatus() == SwitchOff)
              {
                delay(10);  //DELAY OF 10 MSEC
                mili_seconds++;
              }
            //Check whether loop is terminated due to complition of 30 seconds
            if(mili_seconds >= 6000)
              {
                //30 seconds complete. Start clossing the door
                door_State = Closing;
              }
          }
}

void rxr_interrupt()
{
    unsigned long interrupt_time = millis();
    // If interrupts come faster than 1 sec, assume it's a bounce and ignore
    if (interrupt_time - last_interrupt_time > 1000) 
      {
        last_interrupt_time = interrupt_time;
        Serial.println("Interrupted...");
        if(digitalRead(Open) == PinLow && digitalRead(Close) == PinHigh && digitalRead(Stop) == PinLow && door_State != Opening && CloseSwitchStatus() == SwitchOff && StopSwitchStatus() == SwitchOff)
          {
            door_State = Closing;
            Serial.println("Door State = Closing");
            return;
          }
        if(digitalRead(Open) == PinHigh && digitalRead(Close) == PinLow && digitalRead(Stop) == PinLow && door_State != Closing && OpenSwitchStatus() == SwitchOff)
          {
            door_State = Opening;
            Serial.println("Door State = Opening");
            return;
          }
        if(digitalRead(Open) == PinLow && digitalRead(Close) == PinLow && digitalRead(Stop) == PinHigh)
          {
            door_State = Stoped;
            Serial.println("Door State = Stoped");
            return;
          }
        Serial.println("Invalid Remote Input...");
      }
}

boolean OpenSwitchStatus()
{
    int ana_readings;
    int result = 0;
    for(ana_readings = 0; ana_readings < 3; ana_readings++)
      result = result + analogRead(OpenSwitch);
    result = result / 3;
    
    if(result <= 256)
        return SwitchOn;
    else
        return SwitchOff;
}

boolean CloseSwitchStatus()
{
    int ana_readings;
    int result = 0;
    for(ana_readings = 0; ana_readings < 3; ana_readings++)
      result = result + analogRead(CloseSwitch);
    result = result / 3;
    
    if(result <= 256)
        return SwitchOn;
    else
        return SwitchOff;
}

boolean StopSwitchStatus()
{
    int ana_readings;
    int result = 0;
    for(ana_readings = 0; ana_readings < 3; ana_readings++)
      result = result + analogRead(StopSwitch);
    result = result / 3;
    
    if(result <= 256)
        return SwitchOn;
    else
        return SwitchOff;
}

void CalConfig()
{
    float over_current_reading;
    long readings = 0;
    long normal_current = 0;
    
    Serial.println("Calibrating OverCurrent");
    
    //Start opening door and take readings
    digitalWrite(OpenRelay,HIGH);
    digitalWrite(OpenLED,HIGH);
    delay(1000);
    while(OpenSwitchStatus() == SwitchOff)
        {
            normal_current = normal_current + analogRead(Motor_Current);
            readings++;
            delay(250);
        }
    digitalWrite(OpenRelay,LOW);
    digitalWrite(OpenLED,LOW);
    
    delay(5000);    //5 sec delay
    
    //Start closing door and take readings
    digitalWrite(CloseRelay,HIGH);
    digitalWrite(CloseLED,HIGH);
    delay(1000);
    while(CloseSwitchStatus() == SwitchOff)
        {
            normal_current = normal_current + analogRead(Motor_Current);
            readings++;
            delay(250);
        }
    digitalWrite(CloseRelay,LOW);
    digitalWrite(CloseLED,LOW);
    delay(1000);
    
    over_current_reading = (float)normal_current / (float)readings;   //Average of all readings
    Serial.print("Over Current Reading = ");
    Serial.println(over_current_reading);
    over_current_reading = over_current_reading + (over_current_reading * 0.40);    //Over current will be 70% more than normal current
    Serial.print("70% More Over Current Reading = ");
    Serial.println(over_current_reading);
    EEPROM.put(0,over_current_reading);
}

boolean check_over_current()
{
    float current = 0;
    current = analogRead(Motor_Current);
    current = current + analogRead(Motor_Current);
    if(current/2 > motor_over_current)
        return true;
    else
        return false;
}
