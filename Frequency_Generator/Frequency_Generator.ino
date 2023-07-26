//#include <SoftwareSerial.h>
//
//SoftwareSerial mySerial(10, 11); // RX, TX

#define OUT 9          // OUT PIN
#define FourK 3          // OUT PIN

//scan = 0, 1, 2, 3
#define SCAN_0_150 0
#define SCAN_80_150 1
#define SCAN_0_10 2
#define CONT_0_150 3

//freq = 0, 1
#define RUS 0
#define IFT 1

//mode = 0, 1, 2
#define TWO_POLE 0
#define FOUR_POLE_VECT 1
#define FOUR_POLE 2

//sweep = 0, 1, 2
#define WAVE_1 0
#define WAVE_2 1
#define WAVE_3 2

#define dataFrameStart 175
#define dataFrameEnd 200
#define singleBeep 225
#define beepBeep 250

const unsigned char prog_beats[91] PROGMEM = {49,7,18,28,39,49,59,70,78,88,98,109,119,130,140,151,151,150,150,150,150,150,10,150,10,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,10,150,10,150,150,150,150,150,150,150,49,150,151,151,151,151,151,151,151,151,151,10,151,10,151,0,10,0,151,10,151,10,151,0,151,0,0};
const unsigned char prog_times[91] PROGMEM = {5,15,12,15,17,10,15,15,12,15,9,15,20,12,15,20,20,15,15,10,10,15,10,15,15,10,10,15,10,10,15,15,15,15,15,12,15,15,15,10,12,8,15,10,10,15,8,16,20,18,15,10,12,12,15,10,15,15,12,15,15,12,15,15,15,15,15,10,10,15,12,10,15,12,15,15,10,15,10,9,15,17,17,12,15,20,20,10,15,15,15};
const unsigned char prog_freq[91] PROGMEM = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const unsigned char prog_sweep[91] PROGMEM = {1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,2,0,1,2,2,2,0,2,0,2,1,0,1,2,1,0,2,1,2,0,0,1,1,0,0,2,2,2,2,2,2,0,2,0,2,0,1,2,0,0,0,2,1,1,1,2,0,1,0,2,1,1,0,0,1,1,1,0,1,1};
const unsigned char prog_mode[91] PROGMEM = {1,0,0,0,1,2,0,2,1,1,0,1,1,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,1,1,2,1,2,1,2,1,2,1,1,2,0,2,1,1,2,2,1,2,2,2,0,0,2,0,2,1,2,1,1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
const unsigned char prog_scan[91] PROGMEM = {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,1,1,1,1,2,1,2,0,1,0,0,1,1,0,1,0,1,1,1,1,1,1,1,0,0,0,1,0,1,0,0,0,1,0,1,1,2,0,2,1,1,1,0,1,1,0,3,1,1,1,3,1,1,0,0,3,1,2,1,2,1,3,2,3,1,2,1,2,0,3,0,3,3};

unsigned char timer = 0;
unsigned char program = 0;
unsigned char intensity = 0;
unsigned char beat = 0;
unsigned char bal = 0;
unsigned char maxBal = 10;
unsigned char minBeats = 0;
unsigned char maxBeats = 150;
                    
unsigned char scan_pattern = 0;              
unsigned char freq = 0;         
unsigned char sweep = 0;    
unsigned char mode = 0;                                    
unsigned char on_off = 0;    

int currentFreq;
int baseFreq;

int beatChangeTime = 10000;
int balChangeTime = 10000;
unsigned int timerChangeTime = 60000;

long lastBeatChangeTime;
long lastBalChangeTime;
long lastTimerChangeTime;

bool beatsRising = false;
bool balRising = false;

unsigned char currentBeatStep = 0;

void setup()
  {
    delay(2000);
    Serial.begin(1200);
    //mySerial.begin(1200);
    pinMode(OUT, OUTPUT);
    pinMode(FourK, OUTPUT);
  
    turnOffMasterFreq();
    Set_frequency(0);
  
    timer = 5;
    beat = 47;
    scan_pattern = 3;            
    freq = 1;       
    sweep = 1; 
    mode = 1; 
  
    delay(5000);
    
    setCurrentValues();
  }
  
void loop()
  {
    if(on_off == 1)
      ControlOn();
  }

void IFTFreq()
  {
    tone(3,4000);
  }

void RusFreq()
  {
    tone(3,2000);
  }

void turnOffMasterFreq()
  {
    noTone(3);
  }

void getProgValues()
  {
    timer = pgm_read_byte_near(prog_times + program); //prog_times[program];
    beat = pgm_read_byte_near(prog_beats + program);  //prog_beats[program];
    scan_pattern = pgm_read_byte_near(prog_scan + program);  //prog_scan[program];            
    freq = pgm_read_byte_near(prog_freq + program);  //prog_freq[program];       
    sweep = pgm_read_byte_near(prog_sweep + program);  //prog_sweep[program]; 
    mode = pgm_read_byte_near(prog_mode + program);  //prog_mode[program];    
  }
void initMachine()
  {
    if(scan_pattern == 1)
      {
        minBeats = 80;
        maxBeats = 150;
        beat = 150;
      }
    else if(scan_pattern == 2)
      {
        minBeats = 0;
        maxBeats = 10;
        beat = 10;
      }
    else if(scan_pattern == 3)
      {
        minBeats = 0;
        maxBeats = beat;
      }
    else if(scan_pattern == 0)
      {
        minBeats = 0;
        maxBeats = 150;
        beat = 150;
      }
    beatsRising = false;
    balRising = false;
    if(freq == IFT)
      {
        IFTFreq();
        currentFreq = 4000 - beat;
        baseFreq = 4000;
      }
      
    else
      {
        RusFreq();
        currentFreq = 2000 - beat;
        baseFreq = 4000;
      }
    Set_frequency(currentFreq);
    setCurrentValues();
    lastTimerChangeTime = millis();
    lastBeatChangeTime = millis();
    lastBalChangeTime = millis();
    balChangeTime = 8000;
    beatChangeTime = 8000;
  }

void stopMatchine()
  {
    turnOffMasterFreq();
    Set_frequency(0);
    intensity = 0;
    bal = 0;
    on_off = 0;
    setCurrentValues();
  }

void ControlOn()
  {
    unsigned char beatSteps[20] = {2,3,2,3,3,2,3,2,3,3,2,3,2,3,3,2,3,2,3,3};
    
    if(millis() - lastTimerChangeTime >= timerChangeTime)
      {
        timer = timer - 1;
        if(timer <= 0)
          {
            stopMatchine();
            playBeep(beepBeep);
          }
        lastTimerChangeTime = millis();
        setCurrentValues();
      }

    if((mode == FOUR_POLE || mode == FOUR_POLE_VECT) && (scan_pattern == SCAN_0_150 || scan_pattern == SCAN_80_150 || scan_pattern == SCAN_0_10))
      {
        if(millis() - lastBeatChangeTime >= beatChangeTime)
            {
               if(beatsRising == false)
                  {
                    if(beat <= minBeats)
                      {
                        beat = minBeats;
                        beatsRising = true;
                        currentBeatStep = 0;
                        beatChangeTime = 6000;
                      }
                    else
                      {
                        if(freq == IFT)
                          {
                            beat = beat - beatSteps[currentBeatStep];
                            currentBeatStep = currentBeatStep + 1;
                            if(currentBeatStep == 20)
                              currentBeatStep = 0;
                          }
                        else
                          beat = beat - 1;
                        beatChangeTime = 1000;
                      }
                  }
               else
                  {
                    if(beat >= maxBeats)
                      {
                        beat = maxBeats;
                        beatsRising = false;
                        currentBeatStep = 0;
                        beatChangeTime = 6000;
                      }
                    else
                      {
                        if(freq == IFT)
                          {
                            beat = beat + beatSteps[currentBeatStep];
                            currentBeatStep = currentBeatStep + 1;
                            if(currentBeatStep == 20)
                              currentBeatStep = 0;
                          }
                        else
                          beat = beat + 1;
                        beatChangeTime = 1000;
                      }
                  }
               if(freq == IFT)
                  currentFreq = 4000 - beat;
               else
                  currentFreq = 2000 - beat;
               Set_frequency(currentFreq);
               setCurrentValues();
               lastBeatChangeTime = millis();
            }
      }

    if(mode == FOUR_POLE_VECT)
      {
        if(millis() - lastBalChangeTime >= balChangeTime)
            {
               if(balRising == false)
                  {
                    if(bal <= 0)
                      {
                        bal = 0;
                        balRising = true;
                        balChangeTime = 2000;
                      }
                    else
                      {
                        bal = bal - 1;
                        balChangeTime = 1000;
                      }
                  }
               else
                  {
                    if(bal >= maxBal)
                      {
                        bal = maxBal;
                        balRising = false;
                        balChangeTime = 2000;
                      }
                    else
                      {
                        bal = bal + 1;
                        balChangeTime = 1000;
                      }
                  }
               setCurrentValues();
               lastBalChangeTime = millis();
            }
      }
  }

void serialEvent() 
  {
    while (Serial.available()) 
      {
        char key = (char)Serial.read();
        switch(key)
          {
            //Timer Buttons
            case '4':
              if(on_off == 0)
                {
                  if(timer <= 0)
                    timer = 0;
                  else
                    timer = timer - 1;
                }
              break;
              
            case '5':
              if(on_off == 0)
                {
                  if(timer > 99)
                    timer = 99;
                  else
                    timer = timer + 1;
                }
              break;
              
            //Program Buttons
            case 'X':
              if(on_off == 0)
                {
                  if(program <= 0)
                    program = 0;
                  else
                    program = program - 1;
                    
                  getProgValues();    
                  
                  bal = 0;
                  intensity = 0;
                }
              break;
              
            case '6':
              if(on_off == 0)
                {
                  if(program >= 90)
                    program = 90;
                  else
                    program = program + 1;
                    
                  getProgValues();  

                  bal = 0;
                  intensity = 0;
                }
              break;

            //Intensity Buttons
            case '0':
              if(on_off == 1)
                {
                  if(intensity <= 0)
                    intensity = 0;
                  else
                    intensity = intensity - 1;
                  
                  if(bal <= 0)
                    {
                      bal = 0;
                      maxBal = 1;
                    }
                  else
                    {
                      bal = bal - 1;
                      maxBal = bal;
                    }
                  
                  //bal = intensity;
                  //maxBal = bal;
                  balChangeTime = 10000;
                  lastBalChangeTime = millis();
                }
              break;
              
            case 'C':
              if(on_off == 1)
                {
                  if(intensity >= 99)
                    intensity = 99;
                  else
                    intensity = intensity + 1;
                  
                  if(bal >= 99)
                    bal = 99;
                  else
                    {
                      bal = bal + 1;
                      maxBal = bal;
                    }
                  
                  //bal = intensity;
                  //maxBal = bal;
                  balChangeTime = 10000;
                  lastBalChangeTime = millis();
                }
              break;
              
            //Beat Buttons
            case '-':
              if(on_off == 0)
                {
                  if(scan_pattern == CONT_0_150)
                    {
                      if(beat <= 0)
                        beat = 0;
                      else
                        beat = beat - 1;
                    }
                }
              break;

            case '3':
              if(on_off == 0)
                {
                  if(scan_pattern == CONT_0_150)
                    {
                      if(beat >= 150)
                        beat = 150;
                      else
                        beat = beat + 1;
                    }
                }
              break;
              
            //Balance Buttons
            case '+':
              if(on_off == 1)
                {
                  if(bal <= 0)
                    {
                      bal = 0;
                      maxBal = 1;
                    }
                  else
                    {
                      bal = bal - 1;
                      maxBal = bal;
                    }

                  balChangeTime = 10000;
                  lastBalChangeTime = millis();
                }
              break;
              
            case '=':
              if(on_off == 1)
                {
                  if(bal >= 99)
                    bal = 99;
                  else
                    {
                      bal = bal + 1;
                      maxBal = bal;
                    }

                  balChangeTime = 10000;
                  lastBalChangeTime = millis();
                }
              break;
            
            //On Off Button
            case '7':
              if(on_off == 0 && timer > 0)
                {
                  on_off = 1;
                  initMachine();
                }
              else
                {
                  on_off = 0;
                  stopMatchine();
                }
              break;
                
            //Mode Button
            case '8':
              if(on_off == 0)
                {
                  if(mode == 0)
                    {
                      mode = 1;
                    }
                  else if(mode == 1)
                    {
                      mode = 2;
                    }
                  else if(mode == 2)
                    {
                      mode = 0;
                    }
                }
              break;
                
            //Frequency Button
            case '9':
              if(on_off == 0)
                {
                  if(freq == 0)
                    {
                      freq = 1;
                    }
                  else
                    {
                      freq = 0;
                    }
                }
              break;
                
            //Sweep Button
            case '%':
              if(on_off == 0)
                {
                  if(sweep == 0)
                    {
                      sweep = 1;
                    }
                  else if(sweep == 1)
                    {
                      sweep = 2;
                    }
                  else if(sweep == 2)
                    {
                      sweep = 0;
                    }
                }
              break;
                
            //Scan Pattern Button
            case '2':
              if(on_off == 0)
                {
                  if(scan_pattern == 0)
                    {
                      scan_pattern = 1;
                      minBeats = 80;
                      maxBeats = 150;
                      beat = 150;
                      beatsRising = false;
                      balRising = false;
                    }
                  else if(scan_pattern == 1)
                    {
                      scan_pattern = 2;
                      minBeats = 0;
                      maxBeats = 10;
                      beat = 10;
                      beatsRising = false;
                      balRising = false;
                    }
                  else if(scan_pattern == 2)
                    {
                      scan_pattern = 3;
                    }
                  else if(scan_pattern == 3)
                    {
                      scan_pattern = 0;
                      minBeats = 0;
                      maxBeats = 150;
                      beat = 150;
                      beatsRising = false;
                      balRising = false;
                    }
                }
              break;
          }
        playBeep(singleBeep);
      }
    setCurrentValues();
  }

void setCurrentValues()
  {
    Serial.write(dataFrameStart);
    delay(10);
    Serial.write(timer);
    delay(10);
    Serial.write(program);
    delay(10);
    Serial.write(intensity);
    delay(10);
    Serial.write(beat);
    delay(10);
    Serial.write(bal);
    delay(10);
    Serial.write(scan_pattern);
    delay(10);
    Serial.write(freq);
    delay(10);
    Serial.write(sweep);
    delay(10);
    Serial.write(mode);
    delay(10);
    Serial.write(on_off);
    delay(10);
    Serial.write(dataFrameEnd);
    delay(10);
    /*
    mySerial.print('X');
    mySerial.write(timer);
    mySerial.write(program);
    mySerial.write(intensity);
    mySerial.write(beat);
    mySerial.write(bal);
    mySerial.write(scan_pattern);
    mySerial.write(freq);
    mySerial.write(sweep);
    mySerial.write(mode);
    mySerial.write(on_off);
    mySerial.print('X');   
    */ 
  }

void playBeep(unsigned char c)
  {
    Serial.print(c);
  }

void Set_frequency(long fr)
  {
    if(fr == 0)
      {
        TCCR1A = 0b00000000;
      }
    else
      {
        TCCR1A = 0b01000000; 
        uint16_t ocr;
        
        if(freq == IFT)
          fr = fr - 5;
        else
          fr = fr - 2;
          
        //long actualFreq;
        if(fr < 134) 
        {
          TCCR1B = 0b00001010; 
          ocr = (1000000UL/fr) - 1;
          //freq = 1000000UL/(ocr+1);
        }
        else 
        {
          TCCR1B = 0b00001001;
          ocr = (8000000UL/fr) - 1;
          /*
          actualFreq = 8000000UL/(ocr+1);
          if(actualFreq > freq)
            ocr = ocr + 1;
          */
        }
        OCR1AH = highByte(ocr);
        OCR1AL = lowByte(ocr);
        TIMSK1 = 0;
      }
  }
