void setup() 
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A3, OUTPUT);
}

void loop() 
{
  int light = analogRead(A0);
  Serial.println(light);
  if(light < 750)
    digitalWrite(A3, LOW);
  if(light > 900)
    digitalWrite(A3, HIGH);
  delay(1000);
}
