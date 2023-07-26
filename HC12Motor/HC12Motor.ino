//PIN Alias
const int LED = 13;
const int Comm_LED = 12;
const int Motor_LED = 11;
const int Phase_LED = 10;
const int Phase_1 = A0;
const int Phase_2 = A1;
const int Phase_3 = A2;
const int Start_Relay = 8;
const int Stop_Relay = 7;

//Controlling Variables
boolean Motor_Flag = false;
boolean LP_Flag = false;
boolean Power_Flag = false;
char phasechecktime = 0;

void setup()
	{
		pinMode(LED, OUTPUT);
		pinMode(Phase_1, INPUT_PULLUP);
		pinMode(Phase_2, INPUT_PULLUP);
		pinMode(Phase_3, INPUT_PULLUP);
		pinMode(Start_Relay, OUTPUT);
		pinMode(Stop_Relay, OUTPUT);
		pinMode(Comm_LED, OUTPUT);
		pinMode(Motor_LED,OUTPUT);
    pinMode(Phase_LED,OUTPUT);	
		
		//Turn off both relays
		digitalWrite(Start_Relay, LOW);
		digitalWrite(Stop_Relay, LOW);
    digitalWrite(Comm_LED,LOW);
		digitalWrite(Motor_LED,LOW);
    digitalWrite(Phase_LED, LOW);
                
    Power_Flag = Power_Check();
                
		char i;
  
		//LED blinking
		for(i = 0; i <= 5; i++)
			{
				digitalWrite(LED,HIGH);
				delay(1000);
				digitalWrite(LED,LOW);
				delay(1000);
			}
  
		//Serial connection.
		Serial.begin(1200);
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
								if( LP_Flag == true )
									{
										Motor_Stop();
										LP_Flag = false;
									}
							}
					}
			}
    Power_Flag = Power_Check();
	}

void serialEvent() 
  {
    while (Serial.available()) 
      {
        // get the new byte:
        char inChar = (char)Serial.read();
        if(inChar == '#')
          sendStatus();
        if(inChar == '@')
          toggleMotor();
      }
  }

void sendStatus()
  {
    byte motorData = 0;
    if(Power_Flag)     
      motorData = motorData | 0x01;
    if(Motor_Flag && LP_Flag)
      motorData = motorData | 0x02;
    Serial.print(motorData);
  }

void toggleMotor()
  {
    //Motor is on. Turn it off.
    if(Motor_Flag)
      {
        Motor_Stop();
        Motor_Flag = false;
        LP_Flag = false;
        sendStatus();
      }
    else
      {
        //Motor is off. Check the power and turn it on.
        if(Power_Check())
          {
            Motor_Start();
            Motor_Flag = true;
            LP_Flag = true;
          }
        sendStatus();
      }
  }

void Motor_Start()
	{
		digitalWrite(Start_Relay,HIGH);
		delay(8000);	//10 Sec delay
		digitalWrite(Start_Relay,LOW);
		delay(1000);
		digitalWrite(Motor_LED,HIGH);	
	}

void Motor_Stop()
	{
		digitalWrite(Stop_Relay, HIGH);
		delay(8000);	//10 Sec delay
		digitalWrite(Stop_Relay, LOW);
		delay(1000);
		digitalWrite(Motor_LED,LOW);
	}

boolean Power_Check()
	{
		//-------------------POWER CHECK-----------------------//
		if(analogRead(Phase_1) >= 900 || analogRead(Phase_2) >= 900 || analogRead(Phase_3) >= 900)
      {
        digitalWrite(Phase_LED, LOW);
        return false; 
      }
		else
			{
        digitalWrite(Phase_LED, HIGH);
        return true; 
      }
	}
