#define NUMBER_OF_DIGITS 4
const int Data = 22;//25;
const int Shift = 27;//6;
const int Store = 26;//7;

const int SecData = 18;
const int SecShift = 21;//19;
const int SecStore = 19;//21;

byte DisplayCodes[11] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00};
int Token = 0;
int TokenNumber[] = {0,0,0,0,0,0,0};
boolean UpdateToken = false;

void setup() {
  pinMode(Data,OUTPUT);
  pinMode(Shift,OUTPUT);
  pinMode(Store,OUTPUT);
  
  pinMode(SecData,OUTPUT);
  pinMode(SecShift,OUTPUT);
  pinMode(SecStore,OUTPUT);

  digitalWrite(Data,LOW);
  digitalWrite(Shift,LOW);
  digitalWrite(Store,LOW);
  
  digitalWrite(SecData,LOW);
  digitalWrite(SecShift,LOW);
  digitalWrite(SecStore,LOW);

  Serial.begin(115200); 
}

void loop() {
  displayTest();
  displaySecs();
  delay(100);
}

void DisplayToken()
{
    unsigned short Mask, bits, Flag, digit, number;
    //Disable leading zeroes
    for(int i = (NUMBER_OF_DIGITS - 1); i > 0; i--)
      {
        if(TokenNumber[i] == 10 || TokenNumber[i] == 0)
          TokenNumber[i] = 10;
        else
          break;
      }
    //Pump the data into displays from 0 to max
    for(digit = 0; digit < NUMBER_OF_DIGITS; digit++)
       {
          number = DisplayCodes[TokenNumber[digit]];
          Mask = 0x01;
          for (bits=0; bits<8; bits++)
             {
                Flag = number & Mask;
                if(Flag == 0) 
                  {
                    digitalWrite(Data,LOW);
                    delayMicroseconds(1);
                    digitalWrite(Shift,HIGH);
                    delayMicroseconds(1);
                    digitalWrite(Shift,LOW);
                    delayMicroseconds(1);  
                  }
                if(Flag > 0)
                  {
                    digitalWrite(Data,HIGH);   
                    delayMicroseconds(1);
                    digitalWrite(Shift,HIGH);
                    delayMicroseconds(1);
                    digitalWrite(Shift,LOW);
                    delayMicroseconds(1);
                  }
                Mask = Mask << 1;
             }
       } 
    digitalWrite(Store,HIGH);
    delayMicroseconds(1);
    digitalWrite(Store,LOW);
}

void DisplaySecs()
{
    unsigned short Mask, bits, Flag, digit, number;
    for(digit = 0; digit < 2; digit++)
       {
          number = DisplayCodes[TokenNumber[digit]];
          Mask = 0x01;
          for (bits=0; bits<8; bits++)
             {
                Flag = number & Mask;
                if(Flag == 0) 
                    //digitalWrite(SecData,LOW);
                    digitalWrite(SecData,HIGH);
                if(Flag > 0)
                    //digitalWrite(SecData,HIGH);  
                    digitalWrite(SecData,LOW); 
                delayMicroseconds(1);
                digitalWrite(SecShift,HIGH);
                delayMicroseconds(1);
                digitalWrite(SecShift,LOW);
                Mask = Mask << 1;
             }
       }
    delayMicroseconds(1);   
    digitalWrite(SecStore,HIGH);
    delayMicroseconds(1);
    digitalWrite(SecStore,LOW);
}

void displayTest()
  {
    for(int i = (NUMBER_OF_DIGITS-1); i >= 0; i--)
        {
          for(int j = 9; j >= 0; j--)
            {
              TokenNumber[i] = j;
              DisplayToken();
              delay(200);
            }
        }
  }

void displaySecs()
  {
    for(int i = 1; i >= 0; i--)
        {
          for(int j = 9; j >= 0; j--)
            {
              TokenNumber[i] = j;
              DisplaySecs();
              delay(200);
            }
        }
  }
