/*
  Serial Event example

  When new serial data arrives, this sketch adds it to a String.
  When a newline is received, the loop prints the string and clears it.

  A good test for this is to try it with a GPS receiver that sends out
  NMEA 0183 sentences.

  NOTE: The serialEvent() feature is not available on the Leonardo, Micro, or
  other ATmega32U4 based boards.

  created 9 May 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/SerialEvent
*/

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    //Serial.println(inputString);
    String entity = inputString.substring(0, inputString.lastIndexOf('='));
    
    if(entity == "act_flow.txt" || entity == "act_pres.txt" || entity == "vol.txt" || entity == "temp.txt" || entity == "set_flow" || entity == "set_pres")
      { 
        /*String value = inputString.substring(inputString.indexOf('"')+1, inputString.lastIndexOf('"'));
        Serial.println(entity);
        //Serial.println(value);
        stringToNum(value);*/
      }
    if(entity == "cylinder.pic" || entity == "on_off.pic" || entity == "ver_troc.pic" || entity == "co2_air.pic")
      {
        /*String value = inputString.substring(inputString.indexOf('=')+1);
        Serial.println(entity);
        Serial.println(value.toInt());*/
      }
    if(entity == "temp.pco")
      {
        String value = inputString.substring(inputString.indexOf('=')+1);
        Serial.println(entity);
        Serial.println(value);
      }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void stringToNum(String str)
{
  Serial.println(str.charAt(0));
  Serial.println(str.charAt(1));
  if(str.length() == 3)
    {
      Serial.println(str.charAt(2));
    }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    byte inChar = Serial.read();
    if (inChar == 0xFF) {
      stringComplete = true;
    }
    else
    // add it to the inputString:
    inputString += (char)inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    
  }
}
