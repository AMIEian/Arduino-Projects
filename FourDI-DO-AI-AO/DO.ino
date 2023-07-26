#define DO_0 32
#define DO_1 33
#define DO_2 25
#define DO_3 26

void DoSetup()
  {
    pinMode(DO_0, OUTPUT);
    pinMode(DO_1, OUTPUT);
    pinMode(DO_2, OUTPUT);
    pinMode(DO_3, OUTPUT);
  
    digitalWrite(DO_0, LOW);
    digitalWrite(DO_1, LOW);
    digitalWrite(DO_2, LOW);
    digitalWrite(DO_3, LOW);
  }

void SetDOs()
  {
      uint16_t value = inputCounts[DO_INDEX];
      uint16_t mask = 0x0001;
      uint16_t res;
      res = value & mask;
      if(res != 0)
        {
            digitalWrite(DO_0, HIGH);
        }
      else
        {
            digitalWrite(DO_0, LOW);
        }
      
      mask = mask << 1;
      res = value & mask;
      if(res != 0)
        {
            digitalWrite(DO_1, HIGH);
        }
      else
        {
            digitalWrite(DO_1, LOW);
        }
      
      mask = mask << 1;
      res = value & mask;
      if(res != 0)
        {
            digitalWrite(DO_2, HIGH);
        }
      else
        {
            digitalWrite(DO_2, LOW);
        }
      
      mask = mask << 1;
      res = value & mask;
      if(res != 0)
        {
            digitalWrite(DO_3, HIGH);
        }
      else
        {
            digitalWrite(DO_3, LOW);
        }
  }
