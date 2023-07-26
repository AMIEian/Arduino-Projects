#define TANK_ID 0x00

#define FULL true
#define EMPTY false

#define CHECKING_INTERVAL 10000

//PIN Assignment
const int probeLower = 10;  //Black wire of probe
const int probeUpper = 11;  //Red wire of probe with diode
const int lowerLED = 12;
const int upperLED = 13;

void setup() 
{
  pinMode(probeLower, OUTPUT);
  pinMode(probeUpper, OUTPUT);
  
  pinMode(lowerLED, OUTPUT);
  pinMode(upperLED, OUTPUT);
  
  digitalWrite(probeUpper, LOW);
  digitalWrite(probeLower, LOW);

  digitalWrite(lowerLED, LOW);
  digitalWrite(upperLED, LOW);
  
  Serial.begin(9600);
}

void loop() 
{
  char data = 0;
  //Check Upper Level of Lower Tank
  pinMode(probeUpper, INPUT);
  pinMode(probeLower, OUTPUT);
  
  digitalWrite(probeLower, HIGH);
  delay(1000);
  if(digitalRead(probeUpper) == HIGH)
    {
      //Tank is full. ie. both levels full
      digitalWrite(lowerLED, HIGH);
      digitalWrite(upperLED, HIGH);
      
      data = TANK_ID | 0x02;
      data = data + 48;
      Serial.print(data);
      //Depolarization
      pinMode(probeUpper, OUTPUT);
      pinMode(probeLower, INPUT);
      digitalWrite(probeUpper, HIGH);
      delay(1000);

      pinMode(probeLower, OUTPUT);
      digitalWrite(probeUpper, LOW);
      digitalWrite(probeLower, LOW);
    }
  else
    {
      //Check Lower Level of Tank
      digitalWrite(probeLower, LOW);
      pinMode(probeUpper, OUTPUT);
      pinMode(probeLower, INPUT);
      digitalWrite(probeUpper, HIGH);
      delay(1000);
      if(digitalRead(probeLower) == HIGH)
        {
          //Tank is half empty
          digitalWrite(lowerLED, HIGH);
          digitalWrite(upperLED, LOW);
          data = TANK_ID | 0x01;
          data = data + 48;
          Serial.print(data);
        }
      else
        {
          //Tank is empty
          digitalWrite(lowerLED, LOW);
          digitalWrite(upperLED, LOW);
          data = TANK_ID | 0x00;
          data = data + 48;
          Serial.print(data);
        }
      pinMode(probeLower, OUTPUT);
      digitalWrite(probeUpper, LOW);
      digitalWrite(probeLower, LOW);
    }
  delay(CHECKING_INTERVAL);
}
