#define PinHigh true
#define PinLow false

const int VT_Interrupt = 0;   //Pin number 2
const int Open = 5;   //D
const int Close = 3;    //C
const int Stop = 6;   //B
const int OpenRelay = 8;
const int CloseRelay = 7;

typedef enum States
{
    Opened,
    Closed,
    Opening,
    Closing,
    Stoped
}State;

volatile State door_State = Stoped;  //This will keep trck of current position of door.

static unsigned long last_interrupt_time = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(Open,INPUT);
  pinMode(Close,INPUT);
  pinMode(Stop,INPUT);
  pinMode(OpenRelay,OUTPUT);
  pinMode(CloseRelay,OUTPUT);

  digitalWrite(OpenRelay,LOW);
  digitalWrite(CloseRelay,LOW);

  attachInterrupt(VT_Interrupt,rxr_interrupt,RISING);
}

void loop() {
  if(door_State == Opening)
    {
      //Turn on door opening relay
      digitalWrite(OpenRelay,HIGH);
      while(door_State == Opening)
        {
          delay(1);
        }
      //Turn off door opening relay
      digitalWrite(OpenRelay,LOW);
    }

  if(door_State == Closing)
    {
      //Turn on door opening relay
      digitalWrite(CloseRelay,HIGH);
      while(door_State == Closing)
        {
          delay(1);
        }
      //Turn off door opening relay
      digitalWrite(CloseRelay,LOW);
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

        if(digitalRead(Open) == PinLow && digitalRead(Close) == PinHigh && digitalRead(Stop) == PinLow && door_State == Closing)
          {
            door_State = Stoped;
            Serial.println("Door State = Closing Stopped");
            return;
          }
        
        if(digitalRead(Open) == PinLow && digitalRead(Close) == PinHigh && digitalRead(Stop) == PinLow && door_State != Opening)
          {
            door_State = Closing;
            Serial.println("Door State = Closing");
            return;
          }
          
        if(digitalRead(Open) == PinHigh && digitalRead(Close) == PinLow && digitalRead(Stop) == PinLow && door_State == Opening)
          {
            door_State = Stoped;
            Serial.println("Door State = Opening Stopped");
            return;
          }
        
        if(digitalRead(Open) == PinHigh && digitalRead(Close) == PinLow && digitalRead(Stop) == PinLow && door_State != Closing)
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
