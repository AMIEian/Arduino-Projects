#include <EEPROM.h>

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
byte str_len = 0;
int EEPROM_Adr = 0;
byte strCount = 0;
char buff[100];
String eeData = "";

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    
    //Get length of input string
    str_len = (byte) inputString.length();
    
    //Store this number in EEPROM
    EEPROM.put(EEPROM_Adr, str_len);
    
    //Convert input string to char array
    inputString.toCharArray(buff,str_len);
    
    //Get next EEPROM address
    EEPROM_Adr = EEPROM_Adr + 1;

    //Store char array at above address
    EEPROM.put(EEPROM_Adr, buff);

    //Set EEPROM address pointer to next location
    EEPROM_Adr = EEPROM_Adr + str_len;
    strCount++;
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
 if (strCount == 5)
  {
    EEPROM_Adr = 0;
    EEPROM.get(EEPROM_Adr, str_len);
    while(str_len != 0)
      {
        //Increament EEPROM pointer to next location to get actual string
        EEPROM_Adr = EEPROM_Adr + 1;
        //Get the actual string from EEPROM to char array buffer
        EEPROM.get(EEPROM_Adr, buff);
        eeData = buff;
        Serial.println(eeData);
        //Increament EEPROM pointer to next string length location
        EEPROM_Adr = EEPROM_Adr + str_len;
        //Read memory location and get first string length
        EEPROM.get(EEPROM_Adr, str_len);
      }
    
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.write(i, 0);
    }  
    strCount = 0;  
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
