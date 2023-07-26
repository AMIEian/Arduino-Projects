const int VT_Interrupt = 0;
const int D0 = 3;
const int D1 = 4;
const int D2 = 5;
const int D3 = 6;
static unsigned long last_interrupt_time = 0;
int count = 0;

void setup() {
    Serial.begin(9600);
    pinMode(D0,INPUT);
    pinMode(D1,INPUT);
    pinMode(D2,INPUT);
    pinMode(D3,INPUT);
    attachInterrupt(VT_Interrupt,rxr_interrupt,RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void rxr_interrupt()
{
    
    unsigned long interrupt_time = millis();
    // If interrupts come faster than 200ms, assume it's a bounce and ignore
    if (interrupt_time - last_interrupt_time > 200) 
      {
          last_interrupt_time = interrupt_time;
          
          count = count + 1;
          Serial.println(count);
          
          for(int i = 0; i <= 100; i++)
            delayMicroseconds(10000);  //10 milisec delay
      
          if(digitalRead(D0))
            {
                Serial.println("D0 Pressed...!");
                return;
            }
          else if(digitalRead(D1))
            {
                Serial.println("D1 Pressed...!");
                return;
            }
          else if(digitalRead(D2))
            {
                Serial.println("D2 Pressed...!");
                return;
            }
          else if(digitalRead(D3))
            {
                Serial.println("D3 Pressed...!");
                return;
            }
          else
            {
                Serial.println("Unknown Interrupt ...!");
                return;
            }
      }
}
