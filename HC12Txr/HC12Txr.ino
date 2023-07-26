const int LED = 13;
void setup() {
  // initialize serial:
  Serial.begin(1200);
  pinMode(LED, OUTPUT);
}

void loop() {
  Serial.print('H');
  digitalWrite(LED,HIGH);
  delay(2500);
  Serial.print('L');
  digitalWrite(LED,LOW);
  delay(2500);
}
