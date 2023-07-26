const int Phase_1 = A0;
const int Phase_2 = A1;
const int Phase_3 = A2;
const int Start_Relay = 8;
const int Stop_Relay = 7;

byte receivedCode = 0x00;
char txCode = 0x00;
char oldTxCode = 0x00;
char txData = 0x00;
byte id = 0x20;

//Controlling Variables
boolean Motor_Flag = false;
boolean LP_Flag = false;
boolean Power_Flag = false;
char phasechecktime = 0;

void TxData()
{ 
  txData = id | txCode;
  Serial.print(txData);
}

void setup()
  {
    Serial.begin(9600);
    pinMode(Phase_1, INPUT_PULLUP);
    pinMode(Phase_2, INPUT_PULLUP);
    pinMode(Phase_3, INPUT_PULLUP);
    pinMode(Start_Relay, OUTPUT);
    pinMode(Stop_Relay, OUTPUT);

    //Turn off both relays
    digitalWrite(Start_Relay, LOW);
    digitalWrite(Stop_Relay, LOW);
  }

void loop()
  {
    if(Motor_Flag)
      {
        //If motor flag is on check for phases every 15 sec
        phasechecktime++;
        if(phasechecktime >= 30)
          {
            phasechecktime = 0;
            if(Power_Check())
              {
                if( LP_Flag == false )
                  {
                    Motor_Start();
                    LP_Flag = true;
                  }
              }
            //If level or Phase is not ok then stop the motor & set it's flag to 0
            else
              {
                //Level or Power is fail check that the motor was running or not if it was running stop it now
                if( LP_Flag )
                  {
                    Motor_Stop();
                    LP_Flag = false;
                  }
              }
          }
      }
    else
      Power_Check();
    delay(1000);
    
  }

void serialEvent() {
    while (Serial.available()) {
        // get the new byte:
        receivedCode = Serial.read();
        if((receivedCode & 0xF0) == id)
          {
            receivedCode = receivedCode & 0x0F;
          
            if(receivedCode == 0x01 && Motor_Flag == false)
              {
                Motor_Start();
                Motor_Flag == true;
              }
        
            if(receivedCode == 0x02 && Motor_Flag == true)
              {
                Motor_Stop();
                Motor_Flag == false;
              }
          }
     }
  }

void Motor_Start()
  {
    digitalWrite(Start_Relay,HIGH);
    delay(8000);  //10 Sec delay
    digitalWrite(Start_Relay,LOW);
    delay(1000);
    Serial.println(F("MOTOR ON"));
    txCode = txCode | 0x02;
    txData = id | txCode;
    Serial.print(txData);
  }

void Motor_Stop()
  {
    digitalWrite(Stop_Relay, HIGH);
    delay(8000);  //10 Sec delay
    digitalWrite(Stop_Relay, LOW);
    delay(1000);
    Serial.println(F("MOTOR OFF"));
    txCode = txCode & 0x0D;
    txData = id | txCode;
    Serial.print(txData);
  }

boolean Power_Check()
  {
    //-------------------POWER CHECK-----------------------//
    if(analogRead(Phase_1) >= 900 || analogRead(Phase_2) >= 900 || analogRead(Phase_3) >= 900)
      {
        txCode = txCode & 0x0E;
        txData = id | txCode;
        Serial.print(txData);
        return false; 
      }
    else
      {
        txCode = txCode | 0x01;
        txData = id | txCode;
        Serial.print(txData);
        return true; 
      }
  }
