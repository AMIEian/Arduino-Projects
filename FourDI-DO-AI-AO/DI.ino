#define DI_0 36
#define DI_1 39
#define DI_2 34
#define DI_3 35

void DiSetup()
  {
    pinMode(DI_0, INPUT);
    pinMode(DI_1, INPUT);
    pinMode(DI_2, INPUT);
    pinMode(DI_3, INPUT);
  }

void GetDIs()
  {
    unsigned int result = 0;
    
    if(digitalRead(DI_0) == LOW)
      {
          result = result | 0x0001;
      }
    
    if(digitalRead(DI_1) == LOW)
        {
            result = result | 0x0002;
        }
    
    if(digitalRead(DI_2) == LOW)
        {
            result = result | 0x0004;
        }
    
    if(digitalRead(DI_3) == LOW)
        {
            result = result | 0x0008;
        }
    counts[DI_INDEX] = result;
  }
