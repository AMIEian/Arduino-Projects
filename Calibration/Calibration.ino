#include <SoftwareSerial.h>

SoftwareSerial swSer(5, 4, false, 256);

void setup() {
  Serial.begin(115200);
  swSer.begin(9600);
}

void loop() {
  while (swSer.available() > 0) {
    Serial.write(swSer.read());
  }
  while (Serial.available() > 0) {
    cli();
    swSer.write(Serial.read());
    sei();
  }
}
