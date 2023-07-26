int digitalPin = 10;
int rssiDur = 0;
int led = 13;

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  pinMode(digitalPin, INPUT);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
    {
      byte data = Serial.read();
      
      rssiDur = pulseIn(digitalPin, LOW, 200);
      Serial.println(rssiDur);
      if(rssiDur < 40 && rssiDur != 0)
        digitalWrite(led, LOW);
      if(rssiDur > 40 && rssiDur != 0)
        digitalWrite(led, HIGH);
    }
}
