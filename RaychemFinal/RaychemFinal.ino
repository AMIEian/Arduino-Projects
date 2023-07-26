#include <EEPROM.h>
#include <SPI.h>
#include <MCP3424.h>
#include <Wire.h>

MCP3424 MCP(0); // Declaration of MCP3424 pin addr1 et addr0 are connected to +5V

const int DRDY = 8;
const int ResetPin = 9;
const int ADSSelectPin = 10;

const int DI_Int = 2;
const int DI_1 = 3;
const int DI_2 = 4;
const int DI_3 = 5;

const int DO_0 = 6;
const int DO_1 = 7;
const int DO_2 = A0;
const int DO_3 = A1;
const int DO_4 = A2;
const int DO_5 = A3;

char oldDOs = 0x00;

byte SPIData = 0x00;
byte StatusByte = 0x00;
byte ResultH = 0x00;
byte ResultL = 0x00;
unsigned char rxChan = 0;
int16_t ADSData[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
long ADSAvgData[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int ADSReadings = 15;

int16_t analogResult = 5;

long MCPData[4]; // Array used to store results

const int16_t MAX_COUNTS = 10000;
//By default values
int16_t zeroVals[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int16_t fullVals[20] = {10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000};
int16_t counts[21] = {1000, 970, 760, -550, 930, 120, 680, 540, 8743, 3190, 7310, -2340, -4560, 810, 620, 1960, 0, 0, 0, 0, 640};

long baudRate = 115200;
String pcCommand = "";         // a String to hold incoming data
boolean commandReceiving = false; //Flag for PC command
boolean pcCommandReceived = false;  // whether the string is complete
char spacer = ' ';
boolean sendNewData = false; //Flag for # character received
boolean invert = false;

long currentTime = 0; //Current time of system

long lastMCPCheckTime = 0;  //Maintains last MCP check time
long MCPCheckInterval = 1500; //Check MCP after every 1 & 1/2 sec

static unsigned long last_interrupt_time = 0;
static unsigned long onTime = 0;
static unsigned long offTime = 0;
static long cycleCounts = -4;
static boolean edge = false;
static boolean intr = false;
static boolean getNewData = false;

void setup() 
  {
    pinMode(ResetPin, OUTPUT);
    pinMode(ADSSelectPin, OUTPUT);
    pinMode(DRDY, INPUT);
    
    pinMode(DI_Int, INPUT);
    pinMode(DI_1, INPUT);
    pinMode(DI_2, INPUT);
    pinMode(DI_3, INPUT);
    
    pinMode(DO_0, OUTPUT);
    pinMode(DO_1, OUTPUT);
    pinMode(DO_2, OUTPUT);
    pinMode(DO_3, OUTPUT);
    pinMode(DO_4, OUTPUT);
    pinMode(DO_5, OUTPUT);
    
    digitalWrite(ResetPin, LOW);
    digitalWrite(ADSSelectPin, HIGH);

    char oldOutPuts;
    
    EEPROM.get(90,oldOutPuts);
    if(oldOutPuts == 'Y')
      {
        EEPROM.get(91,oldDOs);
      }
    changeDOs(oldDOs);

    char oldBaud;
    
    EEPROM.get(900,oldBaud);
    if(oldBaud == 'A')
      baudRate = 9600;
    else
      baudRate = 115200;
    
    Serial.begin(baudRate); // initialize serial for PC and ESP interface
    SPI.begin();  // initialize SPI for ADS1258
    MCP.begin(0); // initialize I2C(MCP) for MCP3234. MCP Address = 0x00
  
    // reserve 200 bytes for the pcCommand:
    pcCommand.reserve(200);
    loadCalFromEEPROM();
    initADS();

    attachInterrupt(digitalPinToInterrupt(DI_Int), di_interrupt, CHANGE);
  }

void loop() 
  {
    currentTime = millis();
    if(currentTime - last_interrupt_time > 60000)
      {
        //Test End Situation. Resume normal functions.
        intr = false;
        last_interrupt_time = 0;
        onTime = 0;
        offTime = 0;
        cycleCounts = -4;
        ADSReadings = 7;
        getNewData = false;
      }
    
    if(intr == true && cycleCounts > 0)
      {
        //Test routine
        if(getNewData == true)
          {
            currentTime = millis();
            while(currentTime - last_interrupt_time < 20)
              {
                //Wait here for 20 ms
                currentTime = millis();
              }
              
            //Disable average reading
            ADSReadings = 1;
            
            //Take new reading
            readADS();
            
            //Fill other details in array
            counts[16] = cycleCounts;
            
            if(edge == true)
              {
                spacer = 'R';
                counts[18] = 1;
                counts[17] = offTime;
              }
            if(edge == false)
              {
                spacer = 'F';
                counts[18] = 0;
                counts[17] = onTime;
              }
    
            counts[19] = 0;
            getNewData = false; //Don't take new data until next interrupt received
          }
        //Wait for sendNewData command from ESP
        while(sendNewData == false)
          {
            while(Serial.available()) 
              {
                char inChar = (char)Serial.read();
                if (inChar == '#') 
                  {
                    sendNewData = true;
                  }
              }
          }
      }
    else
      {
        //Normal routine
        readADS(); //Read ADS on every itration
        
        currentTime = millis();
        if(currentTime - lastMCPCheckTime >= MCPCheckInterval)
          {
            lastMCPCheckTime = currentTime;
            readMCP();
          }
        
        spacer = ' ';
      }
    
    if(sendNewData == true)
      {
        sendNewData = false;  
        checkDIs();
        for(int i = 0; i < 21; i++)
          {
            Serial.print(spacer);
            Serial.print(counts[i]);
          } 
        Serial.println();
      }
  
    if(pcCommandReceived == true)
      {
        pcCommandReceived = false;
        
        if(pcCommand.charAt(0) == 'x')
          calibrateZero();  //Calibrate 0 Values
        else if(pcCommand.charAt(0) >= 'X')
          calibrateFull();  //Calibrate Full Values
        else if(pcCommand.charAt(0) == '>')
          saveCalibration(pcCommand.charAt(1));  //Save Calibrations to EEPROM
        else if(pcCommand.charAt(0) == '<')
          loadCalFromEEPROM();  //Discard Calibrations and Load from EEPROM
        else if(pcCommand.charAt(0) == '!')
          saveCalibration('Z');  //Factory Reset. Format user calibration and load calibrations from Factory Values 
        else if(pcCommand.charAt(0) == '.')
          allClear();  //Format everything. All calibrations and WiFi settings
        else if(pcCommand.charAt(0) == 'W')
          changeBaud(); //Change Baud Rate
        else if(pcCommand.charAt(0) == '$')
          saveWiFiConfig(); //Save WiFi Settings to EEPROM
        else if(pcCommand.charAt(0) == '@')  
          sendWiFiConfig(); //Read and send WiFi Settings from EEPROM
        else if(pcCommand.charAt(0) == '0')
          changeDOs(pcCommand.charAt(1) - '0'); //Change DOs
          
        pcCommand = "";
      }
  }

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() 
  {
    while(Serial.available()) 
      {
        // get the new byte:
        char inChar = (char)Serial.read();
        //Check whether it is data send command
        if (inChar == '#' && commandReceiving == false) 
          {
            sendNewData = true;
          }
        else
          {
            commandReceiving = true;
            // If its not then add it to the pcCommand:
            pcCommand += inChar;
            // if the incoming character is a newline, set a flag so the main loop can
            // do calibration with it:
            if (inChar == '\n') 
              {
                pcCommandReceived = true;
                commandReceiving = false;
              }
          }
      }
  }

void di_interrupt()
  {
    intr = true;
    if(digitalRead(DI_Int) == HIGH)
      {
        edge = true;
        offTime = millis() - last_interrupt_time;
        cycleCounts = cycleCounts + 1;
      }
    else
      {
        edge = false;  
        onTime = millis() - last_interrupt_time;
      }
    getNewData = true;
    last_interrupt_time = millis();
  }

void changeDOs(byte DOs)  
  {
    if(oldDOs != DOs)
      {
        byte mask = 0x01;
        byte outBit;
        
        outBit = DOs & mask;
        if(outBit == 0)
          digitalWrite(DO_0,LOW);
        if(outBit > 0)
          digitalWrite(DO_0,HIGH);
          
        mask = mask << 1;
    
        outBit = DOs & mask;
        if(outBit == 0)
          digitalWrite(DO_1,LOW);
        if(outBit > 0)
          digitalWrite(DO_1,HIGH);
          
        mask = mask << 1;
    
        outBit = DOs & mask;
        if(outBit == 0)
          digitalWrite(DO_2,LOW);
        if(outBit > 0)
          digitalWrite(DO_2,HIGH);
          
        mask = mask << 1;
    
        outBit = DOs & mask;
        if(outBit == 0)
          digitalWrite(DO_3,LOW);
        if(outBit > 0)
          digitalWrite(DO_3,HIGH);
     
        mask = mask << 1;
    
        outBit = DOs & mask;
        if(outBit == 0)
          digitalWrite(DO_4,LOW);
        if(outBit > 0)
          digitalWrite(DO_4,HIGH);
    
        mask = mask << 1;
    
        outBit = DOs & mask;
        if(outBit == 0)
          digitalWrite(DO_5,LOW);
        if(outBit > 0)
          digitalWrite(DO_5,HIGH);  
    
        //Store in EEPROM
        EEPROM.put(90, 'Y');
        EEPROM.put(91, DOs); 

        oldDOs = DOs;
      }
  }

void checkDIs()
  {
    unsigned int DIs = 0;
    
    if(digitalRead(DI_1) == HIGH)
      DIs = DIs | 1;
      
    if(digitalRead(DI_2) == HIGH)
      DIs = DIs | 2;
      
    if(digitalRead(DI_3) == HIGH)
      DIs = DIs | 4; 
      
    if(digitalRead(DO_0) == HIGH)
      DIs = DIs | 8;   

    if(digitalRead(DO_1) == HIGH)
      DIs = DIs | 16;

    if(digitalRead(DO_2) == HIGH)
      DIs = DIs | 32;  

    if(digitalRead(DO_3) == HIGH)
      DIs = DIs | 64;

    if(digitalRead(DO_4) == HIGH)
      DIs = DIs | 128;  

    if(digitalRead(DO_5) == HIGH)
      DIs = DIs | 256;  
      
    counts[20] = DIs;
    
    //Invert communication indicator bit
    if(invert == true)
      {
        counts[20] = counts[20] | 0x8000;
        invert = false;
      }
    else
      {
        counts[20] = counts[20] & 0x7FFF;
        invert = true;
      }
  }

void calibrateZero()
  {
    int index = 0;
    pcCommand.trim(); //Removes Extra Spaces and Newline Character from Command String
    if(validCalZeroString())
      {
        for(int i = 1; i < pcCommand.length(); i++)
          {
            //Store current readings as zeroValues
            index = pcCommand.charAt(i) - 'a';
            zeroVals[index] = zeroVals[index] + counts[index];
          }
      }
  }

boolean validCalZeroString()
  {
    boolean res = true;
    for(int i = 1; i < pcCommand.length(); i++)
      {
        if(pcCommand.charAt(i) >= 'a' && pcCommand.charAt(i) <= 't')
          continue;
        else
          {
            res = false;
            break;
          }
      }
    return res;
  }

void calibrateFull()
  {
    int index = 0;
    pcCommand.trim(); //Removes Extra Spaces and Newline Character from Command String
    if(validCalFullString())
      {
        for(int i = 1; i < pcCommand.length(); i++)
          {
            //Store current readings as zeroValues
            index = pcCommand.charAt(i) - 'A';
            fullVals[index] = counts[index];
          }
      }
  }

boolean validCalFullString()
  {
    boolean res = true;
    for(int i = 1; i < pcCommand.length(); i++)
      {
        if(pcCommand.charAt(i) >= 'A' && pcCommand.charAt(i) <= 'T')
          continue;
        else
          {
            res = false;
            break;
          }
      }
    return res;
  }

void saveCalibration(char mode)
  {
    noInterrupts();
    int16_t zero = 0;
    //Clear old user calibration
    for(int i = 0; i <= 80; i++)
      EEPROM.put(i,zero);
    //Check the mode
    if(mode == 'C') //User Mode
      {          
        //Save new user calibration
        EEPROM.put(0,'C');  //Indicates calibration is done
        int index = 0;
        //Save these zeroValues into EEPROM from location 1 to 40.  Total 40 bytes for 20 ints.
        for(int i = 1; i <= 40; i = i+2)
          {
            EEPROM.put(i,zeroVals[index]);
            index++;
          }
        index = 0;
        //Save these zeroValues into EEPROM from location 41 to 80. Total 40 bytes for 20 ints.
        for(int i = 41; i <= 80; i = i+2)
          {
            EEPROM.put(i,fullVals[index]);
            index++;
          }  
      }
    if(mode == 'F') //Factory Mode
      {
        //Clear old factory calibration
        for(int i = 100; i <= 180; i++)
          EEPROM.put(i,zero);
          
        //Save new factory calibration
        EEPROM.put(100,'F');  //Indicates calibration is done
        int index = 0;
        //Save these zeroValues into EEPROM from location 1 to 40.  Total 40 bytes for 20 ints.
        for(int i = 101; i <= 140; i = i+2)
          {
            EEPROM.put(i,zeroVals[index]);
            index++;
          }
        index = 0;
        //Save these zeroValues into EEPROM from location 41 to 80. Total 40 bytes for 20 ints.
        for(int i = 141; i <= 180; i = i+2)
          {
            EEPROM.put(i,fullVals[index]);
            index++;
          }  
      }  
    loadCalFromEEPROM();  //Load the Calibration from EEPROM to get in effect.
    interrupts();
  }

void loadCalFromEEPROM()
  {
    noInterrupts();
    char calDone;
    int index = 0;
    EEPROM.get(0, calDone);
    if(calDone == 'C')
      {
        //User calibration is found.
        //Get Zero Values from memory location 1 to 40
        for(int i = 1; i <= 40; i = i+2)
          {
            int k;
            EEPROM.get(i,k);
            zeroVals[index] = k;
            index++;
          }
          
        //Get Full Values from memory location 41 to 80
        index = 0;
        for(int i = 41; i <= 80; i = i+2)
          {
            int k;
            EEPROM.get(i,k);
            fullVals[index] = k;
            index++;
          }  
      }
    else
      {
        EEPROM.get(100, calDone);
        if(calDone == 'F')
          {
            //Factory calibration is found
            //Get Zero Values from memory location 101 to 140
            for(int i = 101; i <= 140; i = i+2)
              {
                int k;
                EEPROM.get(i,k);
                zeroVals[index] = k;
                index++;
              }
              
            //Get Full Values from memory location 141 to 180
            index = 0;
            for(int i = 141; i <= 180; i = i+2)
              {
                int k;
                EEPROM.get(i,k);
                fullVals[index] = k;
                index++;
              }   
          }
        else
          {
            //If User and Factory Calibration is not found then load default values
            for(int i = 0; i < 20; i++)
              {
                zeroVals[i] = 0;
                fullVals[i] = 10000;
              }
          }
      }
    interrupts();
  }

void changeBaud()
  {
    if(pcCommand.charAt(1) == 'b' && pcCommand.charAt(2) == 'D')
      {
        if(baudRate == 115200)
          {
            EEPROM.write(900,'A');
            baudRate = 9600;
          }
        else
          {
            EEPROM.write(900,'B');
            baudRate = 115200;
          }
        Serial.flush();
        delay(2);
        Serial.end();
        delay(500);
        Serial.begin(baudRate);
        while(Serial.available())
          Serial.read();
      }
  }

void saveWiFiConfig()
  {
    noInterrupts();
    char ch;
    int Write_Pointer = 300;
    int c = pcCommand.length();
    for (int x = 0; x < c; x++)
      {
        ch = pcCommand.charAt(x);
        EEPROM.write(Write_Pointer,ch);
        Write_Pointer++;
        delay(1);
      }
    ch = '\0';
    EEPROM.write(Write_Pointer,ch);
    delay(1);
    interrupts();
  }

void sendWiFiConfig()
  {
     noInterrupts();
     char ch;
     int Read_Pointer = 300;
     ch = (char)EEPROM.read(Read_Pointer);
     if(ch == '$')
      {
        while(ch != '\0')
          {
            Serial.print(ch);
            Read_Pointer++;
            ch = (char)EEPROM.read(Read_Pointer);
          }
         Serial.println(); 
      }
    interrupts();
  }

void initADS()
  {
    digitalWrite(ResetPin, LOW);
    delay(40);
    digitalWrite(ResetPin, HIGH);
    delay(40);
    
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));

    //Start Communication with ADS
    digitalWrite(ADSSelectPin, LOW);
    delayMicroseconds(10);

    //Initialise ADS registers one by one
    SPIData = SPI.transfer(0x60); //Write to register 0x00
    SPIData = SPI.transfer(0x02); //Register CONFIG0 = 0x12 (00010010)
    
    SPIData = SPI.transfer(0x61); //Write to register 0x01
    SPIData = SPI.transfer(0x73); //Register CONFIG1 = 0x73 (‭01110011‬)
  
    SPIData = SPI.transfer(0x62); //Write to register 0x02
    SPIData = SPI.transfer(0x00); //Register MUXSCH = 0x00 (‭00000000‬)
  
    SPIData = SPI.transfer(0x63); //Write to register 0x03
    SPIData = SPI.transfer(0x00); //Register MUXDIF = 0x00 (‭00000000‬)
  
    SPIData = SPI.transfer(0x64); //Write to register 0x04
    SPIData = SPI.transfer(0xff); //Register MUXSG0 = 0xff (11111111)
  
    SPIData = SPI.transfer(0x65); //Write to register 0x05
    SPIData = SPI.transfer(0xff); //Register MUXSG1 = 0xff (11111111)
  
    SPIData = SPI.transfer(0x66); //Write to register 0x06
    SPIData = SPI.transfer(0x00); //Register SYSRED = 0x00 (00000000)
  
    SPIData = SPI.transfer(0x67); //Write to register 0x07
    SPIData = SPI.transfer(0x00); //Register GPIOC = 0x00 (00000000)
  
    SPIData = SPI.transfer(0x68); //Write to register 0x08
    SPIData = SPI.transfer(0x00); //Register GPIOD = 0x00 (00000000)
  
    SPIData = SPI.transfer(0x69); //Write to register 0x09
    SPIData = SPI.transfer(0x8B); //Register GPIOC = 0x8B (‭10001011‬)
  }

void readADS()
  {
    for(int i = 0; i < 16; i++)
      {
        ADSAvgData[i] = 0;
      }
    for(int k = 0; k < ADSReadings; k++)
      {
        for(int i = 0; i< 32; i++)
          {
            while(digitalRead(DRDY) == HIGH)
              {
                
              }
            if(digitalRead(DRDY) == LOW)
              {
                SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
                
                digitalWrite(ADSSelectPin, LOW);
                delayMicroseconds(10);
                
                SPIData = SPI.transfer(0x30);
                delayMicroseconds(1);
                StatusByte = SPI.transfer(0x00);
                delayMicroseconds(1);
              
                StatusByte = StatusByte & 0x1f;
                if(StatusByte > 7 && StatusByte < 24)
                  rxChan = StatusByte - 8 ;
                else
                  rxChan = 16 ;
              
                ResultH = SPI.transfer(0x00);
                ResultL = SPI.transfer(0x00);
                SPIData = SPI.transfer(0x00);
                
                analogResult = 0x00;
            
                analogResult = ResultH;
                analogResult = analogResult << 8;
                analogResult = analogResult | ResultL ;
                ADSData[rxChan] = analogResult;
                
                digitalWrite(ADSSelectPin, HIGH);
            
                SPI.endTransaction();
              }
          }
        for(int i = 0; i < 16; i++)
          {
            ADSAvgData[i] = ADSAvgData[i] + ADSData[i];
          }
      }

    //Find average
    for(int i = 0; i < 16; i++)
      {
        ADSAvgData[i] = (long)ADSAvgData[i]/ADSReadings;
      }
            
    //Apply Calibration zero values
    for(int i = 0; i < 16; i++)
      {
        ADSAvgData[i] = ADSAvgData[i] - zeroVals[i];
      }  
    //Apply calibration for fullValues now
    for(int k = 0; k < 16; k++)
      {
        float ratio = (float)ADSAvgData[k]/(float)fullVals[k];
        ADSAvgData[k] = (int16_t)(ratio * MAX_COUNTS);
      }
      
    //Store Values
    for(int i = 0; i < 16; i++)
      {
        counts[i] = (int16_t)ADSAvgData[i];
      }
  }

void readMCP()
  {
    for (int i=1;i<=4;i++)
      {  
        MCP.configuration(i,16,1,1); // MCP3424 is configured to channel i with 16 bits resolution, continous mode and gain defined to 8 
        MCPData[i-1] = MCP.measure(); // Measure is stocked in array Voltage, note that the library will wait for a completed conversion that takes around 200 ms@18bits
      }
      
    //Apply calibration for zeroValue first
    for(int i = 0; i < 4; i++)
      {
        MCPData[i] = MCPData[i] - zeroVals[i + 16];
      }
    //Apply calibration for fullValues now
    for(int k = 0; k < 4; k++)
      {
        float ratio = (float)MCPData[k]/(float)fullVals[k + 16];
        MCPData[k] = (int16_t)(ratio * MAX_COUNTS);
      }

    //Store MCP values
    for(int k = 0; k < 4; k++)
      {
        counts[k + 16] = MCPData[k];
      }
  }

void allClear()
  {
    for (int x = 0; x < 1000; x++)
      EEPROM.write(x,0);
    loadCalFromEEPROM();
  }
