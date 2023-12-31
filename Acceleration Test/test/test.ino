/*
 ADXL3xx

 Reads an Analog Devices ADXL3xx accelerometer and communicates the
 acceleration to the computer.  The pins used are designed to be easily
 compatible with the breakout boards from Sparkfun, available from:
 http://www.sparkfun.com/commerce/categories.php?c=80

 http://www.arduino.cc/en/Tutorial/ADXL3xx

 The circuit:
 analog 0: accelerometer self test
 analog 1: z-axis
 analog 2: y-axis
 analog 3: x-axis
 analog 4: ground
 analog 5: vcc

 created 2 Jul 2008
 by David A. Mellis
 modified 30 Aug 2011
 by Tom Igoe

 This example code is in the public domain.

*/

// these constants describe the pins. They won't change:
//const int groundpin = 18;             // analog input pin 4 -- ground
//const int powerpin = 19;              // analog input pin 5 -- voltage
const int xpin = A0;                  // x-axis of the accelerometer
const int ypin = A1;                  // y-axis
const int zpin = A2;                  // z-axis (only on 3-axis models)
int reading = 0;

void setup() {
  analogReference(EXTERNAL); // use AREF for reference voltage
  // initialize the serial communications:
  Serial.begin(9600);
  // Provide ground and power by using the analog inputs as normal
  // digital pins.  This makes it possible to directly connect the
  // breakout board to the Arduino.  If you use the normal 5V and
  // GND pins on the Arduino, you can remove these lines.
  //pinMode(groundpin, OUTPUT);
  //pinMode(powerpin, OUTPUT);
  //digitalWrite(groundpin, LOW);
  //digitalWrite(powerpin, HIGH);
}

void loop() {
  reading = analogRead(xpin) - 512;
  if(reading >= 0) 
    // print the sensor values:
    Serial.print(map(reading, 0, 512, 0, 100));
  else if(reading < 0)
    // print the sensor values:
    Serial.print(map(reading, -1, -512, -1, -100));
    
  // print a tab between values:
  Serial.print("\t");
  
  reading = analogRead(ypin) - 512;
  if(reading >= 0) 
    // print the sensor values:
    Serial.print(map(reading, 0, 512, 0, 100));
  else if(reading < 0)
    // print the sensor values:
    Serial.print(map(reading, -1, -512, -1, -100));
  
  // print a tab between values:
  Serial.print("\t");
    
  reading = analogRead(zpin) - 512;
  if(reading >= 0) 
    // print the sensor values:
    Serial.print(map(reading, 0, 512, 0, 100));
  else if(reading < 0)
    // print the sensor values:
    Serial.print(map(reading, -1, -512, -1, -100));
    
  Serial.println();
  // delay before next reading:
  delay(1000);
}
