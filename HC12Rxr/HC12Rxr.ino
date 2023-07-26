const int LED = 13;
void setup() {
  // initialize serial:
  Serial.begin(2400);
  pinMode(LED, OUTPUT);
}

void loop() {
  
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    Serial.print(inChar);
    if (inChar == 'H') {
      digitalWrite(LED,HIGH);
    }
    if (inChar == 'L') {
      digitalWrite(LED,LOW);
    }
  }
}


