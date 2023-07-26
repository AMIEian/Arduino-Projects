#define SDO 13
#define SCL 14
#define DAC1 15
#define DAC2 14

void setup() 
  {
    pinMode(SDO, OUTPUT);
    pinMode(SCL, OUTPUT);
    pinMode(DAC1, OUTPUT);
    pinMode(DAC2, OUTPUT);

    digitalWrite(DAC1, HIGH);
    digitalWrite(DAC2, HIGH);

    int outValue = 1020;
    int res = 0;
    digitalWrite(DAC1, LOW); //DAC1_SetLow();
    res = setChannelA(outValue);
    digitalWrite(DAC1, HIGH); //DAC1_SetHigh();
    
    outValue = outValue * 2;
    digitalWrite(DAC1, LOW); //DAC1_SetLow();
    res = setChannelB(outValue);
    digitalWrite(DAC1, HIGH); //DAC1_SetHigh();

    outValue = outValue * 2;
    digitalWrite(DAC2, LOW); //DAC1_SetLow();
    res = setChannelA(outValue);
    digitalWrite(DAC2, HIGH); //DAC1_SetHigh();

    outValue = outValue * 2;
    digitalWrite(DAC2, LOW); //DAC1_SetLow();
    res = setChannelB(outValue);
    digitalWrite(DAC2, HIGH); //DAC1_SetHigh();
  }

void loop() 
  {
    
  }

uint16_t setChannelA(int val)
    {
        //Set channel A
        int16_t out = 0;
        uint16_t result = 0;
        out = 0x3000;   //Channel B, Unbuffered, Output Gain = 1, Active mode
        out = out | val;
        SPI1_Exchange16bit(out);
        //result = SPI1_Exchange16bit(out);
        return result;
    }

uint16_t setChannelB(int val)
    {
        //Set channel B
        int16_t out = 0;
        uint16_t result = 0;
        out = 0xB000;   //Channel B, Unbuffered, Output Gain = 1, Active mode
        out = out | val;
        SPI1_Exchange16bit(out);
        //result = SPI1_Exchange16bit(out);
        return result;
    }

void SPI1_Exchange16bit(int16_t data)
    {
        uint16_t intk = 0x8000;
        int16_t inti = data ;
        while(intk != 0)
            {
                if ( (inti & intk)==0 )
                    digitalWrite(SDO, LOW); //SDO_SetLow() ;
                else
                    digitalWrite(SDO, HIGH); //SDO_SetHigh() ;
                delay(1);
                digitalWrite(SCL, HIGH); //SCL_SetHigh() ;
                delay(1);
                digitalWrite(SCL, LOW); //SCL_SetLow() ;
                intk = intk >> 1 ;    
            }
    }
