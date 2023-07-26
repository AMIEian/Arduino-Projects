#define PanicSwitch 4
#define DoorSwitch 5
#define Siren 6

#define ARMED 1
#define NORMAL 0

uint8_t mode = NORMAL;
bool panic_on = false;
bool door_opened = false;
bool Siren_On = false;
long siren_on_time = 0;
long max_siren_on_time = 1200000;

void initHardware()
  {
    pinMode(PanicSwitch, INPUT);
    pinMode(DoorSwitch, INPUT);
    pinMode(Siren, OUTPUT);
    digitalWrite(Siren, LOW);
    GetUnitMode();
  }

bool PanicSituation()
  {
    if(digitalRead(PanicSwitch) == HIGH)
      {
        delay(100);
        if(digitalRead(PanicSwitch) == HIGH)
          return true;
        else
          return false;
      }
    else
      return false;
  }

bool DoorOpen()
  {
    if(digitalRead(DoorSwitch) == HIGH)
      {
        delay(100);
        if(digitalRead(DoorSwitch) == HIGH)
          return true;
        else
          return false;
      }
    else
      return false;
  }

void SirenOn()
  {
    Siren_On = true;
    digitalWrite(Siren, HIGH);
  }

void SirenOff()
  {
    Siren_On = false;
    digitalWrite(Siren, LOW);
  }

void GetUnitMode()
  {
    mode = readStringFromEEPROM(200).toInt();
  }

void LogicController()
  {
    if(mode == ARMED)
      {
        if(panic_on == true)
          panic_on = false;
        if(door_opened == false)
          {
            if(DoorOpen() == true)
              {
                SendTheftSMS();
                SirenOn();
                siren_on_time = millis();
                door_opened = true;
              }
          }        
        if(Siren_On == true)
          {
            if(millis() - siren_on_time >= max_siren_on_time)
              {
                SirenOff();
              }
          }
      }
    if(mode == NORMAL)
      {
        if(door_opened == true)
          door_opened = false;
        if(Siren_On == true)
          SirenOff();
        if(panic_on == false)
          {
            if(PanicSituation() == true)
              {
                SendEmergencySMS();
                panic_on = true;
              }
          }
      }
  }
