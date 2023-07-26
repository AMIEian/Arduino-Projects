#define PanicSwitch 11
#define DoorSwitch A2
#define Siren 2
#define SIMPower 5
#define ARMED 1
#define NORMAL 0

uint8_t mode = NORMAL;
char panic_on = 0;
char door_opened = 0;
char Siren_On = 0;
long siren_on_time = 0;
long max_siren_on_time = 1200000;

void initHardware()
  {
    pinMode(PanicSwitch, INPUT);
    pinMode(DoorSwitch, INPUT);
    pinMode(Siren, OUTPUT);
    pinMode(SIMPower, OUTPUT);
    pinMode(beatLED, OUTPUT);
    
    digitalWrite(Siren, HIGH);
    digitalWrite(SIMPower, HIGH);
    delay(1000);
    digitalWrite(SIMPower, LOW);
    digitalWrite(beatLED, LOW);
    GetUnitMode();
  }

bool PanicSituation()
  {
    if(digitalRead(PanicSwitch) == HIGH)
      {
        delay(100);
        if(digitalRead(PanicSwitch) == HIGH)
          return 1;
        else
          return 0;
      }
    else
      return 0;
  }

bool DoorOpen()
  {
    if(digitalRead(DoorSwitch) == HIGH)
      {
        delay(100);
        if(digitalRead(DoorSwitch) == HIGH)
          return 1;
        else
          return 0;
      }
    else
      return 0;
  }

void SirenOn()
  {
    Siren_On = 1;
    digitalWrite(Siren, LOW);
  }

void SirenOff()
  {
    Siren_On = 0;
    digitalWrite(Siren, HIGH);
  }

void GetUnitMode()
  {
    mode = readStringFromEEPROM(200).toInt();
  }

void LogicController()
  {
    if(mode == ARMED)
      {
        if(door_opened == 0)
          {
            if(DoorOpen() == 1)
              {
                in_call.HangUp();           
                delay(1000);
                SirenOn();
                SendTheftSMS();
                siren_on_time = millis();
                door_opened = 1;
              }
          } 
         if(PanicSituation() == 1)
            {
              in_call.HangUp();              
              delay(1000);
              SendEmergencySMS();
            }       
        if(Siren_On == 1)
          {
            if(millis() - siren_on_time >= max_siren_on_time)
              {
                SirenOff();
              }
          }
      }
    if(mode == NORMAL)
      {
        if(door_opened == 1)
          door_opened = 0;
        if(Siren_On == 1)
          SirenOff();       
        if(PanicSituation() == 1)
          {
            in_call.HangUp();              
            delay(1000);
            SendEmergencySMS();
          }
      }
  }
