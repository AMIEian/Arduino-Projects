void CheckHandSwitches()
  {
    //For Door 1 Switch
    uint16_t timer = switch_Delay;
      
    while(digitalRead(d1_Switch) == LOW && timer > 0 && d1_Switch_Closed == true)
      {
        //Check switch is continuously pressed for switch delay ms
        delay(10);
        timer = timer - 10;
        serialEvent();
      }
    if(timer <= 0 && d1_Switch_Closed == true)
      {
        //Switch hold delay is complete. Set the switch Status.
        d1_Switch_Status = true;
        d1_Switch_Closed = false;
      }
    else if(digitalRead(d1_Switch) == HIGH && d1_Switch_Closed == false)
      {
        d1_Switch_Status = false;
        d1_Switch_Closed = true;
      }
    else
      {
        d1_Switch_Status = false;
      }

    //For Door 2 Switch
    timer = switch_Delay;
    
    while(digitalRead(d2_Switch) == LOW && timer > 0 && d2_Switch_Closed == true)
      {
        //Check switch is continuously pressed for switch delay ms
        delay(10);
        timer = timer - 10;
        serialEvent();
      }
    if(timer <= 0 && d2_Switch_Closed == true)
      {
        //Switch hold delay is complete. Set the switch flag.
        d2_Switch_Status = true;
        d2_Switch_Closed = false;
      }
    else if(digitalRead(d2_Switch) == HIGH && d2_Switch_Closed == false)
      {
        d2_Switch_Status = false;
        d2_Switch_Closed = true;
      }
    else
      {
        d2_Switch_Status = false;
      }
  }

void CheckDoorsStatus()
  {
    delay(1000);
    if(digitalRead(d1_FB_1) == LOW && digitalRead(d1_FB_2) == LOW)
      {
        //Door 1 is opened
        d1_Status = 1;
      }
    else if(digitalRead(d1_FB_1) == HIGH && digitalRead(d1_FB_2) == HIGH)
      {
        //Door 1 is Closed
        d1_Status = 0;
      }

    if(digitalRead(d2_FB_1) == LOW && digitalRead(d2_FB_2) == LOW)
      {
        //Door 2 is opened
        d2_Status = 2;
      }
    else if(digitalRead(d2_FB_1) == HIGH && digitalRead(d2_FB_2) == HIGH)
      {
        //Door 2 is Closed
        d2_Status = 0;
      }
  }

bool OpenTheDoor(uint8_t door)
  {
    if(door == door_1)
      {
        digitalWrite(d1_Lock, LOW);
        digitalWrite(d1_Red_LED, HIGH);
        digitalWrite(d1_Green_LED, LOW);
        
        digitalWrite(d2_Red_LED, HIGH);
        digitalWrite(d2_Green_LED, LOW);
        
        digitalWrite(d1_Buzzer, HIGH);
        delay(1000);
        digitalWrite(d1_Buzzer, LOW);

        d1_Status = 1;
        UpdateBoardStatus();
        uint16_t timer = door_Opening_Delay;
        while(timer > 0)
          {
            delay(10);
            timer = timer - 10;
            serialEvent();
          }
         
        digitalWrite(d1_Lock, HIGH); 
        CheckDoorsStatus();
        if(d1_Status == 0)
          {
            CloseTheDoor(door);
            UpdateBoardStatus();
            return false;
          }
        else
          {
            UpdateBoardStatus();
            return true;
          }          
      }
    if(door == door_2)
      {
        digitalWrite(d2_Lock, LOW);
        digitalWrite(d2_Red_LED, HIGH);
        digitalWrite(d2_Green_LED, LOW);

        digitalWrite(d1_Red_LED, HIGH);
        digitalWrite(d1_Green_LED, LOW);
        
        digitalWrite(d2_Buzzer, HIGH);
        delay(1000);
        digitalWrite(d2_Buzzer, LOW);

        d2_Status = 2;
        UpdateBoardStatus();
        uint16_t timer = door_Opening_Delay;
        while(timer > 0)
          {
            delay(10);
            timer = timer - 10;
            serialEvent();
          }
          
        digitalWrite(d2_Lock, HIGH); 
        CheckDoorsStatus();
        if(d2_Status == 0)
          {
            CloseTheDoor(door);
            UpdateBoardStatus();
            return false;
          }
        else
          {
            UpdateBoardStatus();
            return true;
          }
      }
  }

void CloseTheDoor(uint8_t door)
  {
    if(door == door_1)
      {
        digitalWrite(d1_Lock, HIGH);
        digitalWrite(d1_Red_LED, LOW);
        digitalWrite(d1_Green_LED, HIGH);

        digitalWrite(d2_Red_LED, LOW);
        digitalWrite(d2_Green_LED, HIGH);
                        
        d1_Status = 0;
      }
    if(door == door_2)
      {
        digitalWrite(d2_Lock, HIGH);
        digitalWrite(d2_Red_LED, LOW);
        digitalWrite(d2_Green_LED, HIGH);

        digitalWrite(d1_Red_LED, LOW);
        digitalWrite(d1_Green_LED, HIGH);
     
        d2_Status = 0;
      }
    UpdateBoardStatus();
    BeepBuzzer(door);
    BeepBuzzer(door);
    //Serial.println("Door Closed");
  }

void EmergencyOpen()
  {
    //Open both doors
    digitalWrite(d1_Lock, LOW);
    digitalWrite(d2_Lock, LOW);

    digitalWrite(d1_Red_LED, HIGH);
    digitalWrite(d1_Green_LED, LOW);

    digitalWrite(d2_Red_LED, HIGH);
    digitalWrite(d2_Green_LED, LOW);
  }

void BeepBuzzer(uint8_t door)
  {
    if(door == door_1)
      {
        digitalWrite(d1_Buzzer, HIGH);
        delay(1000);
        digitalWrite(d1_Buzzer, LOW);
        delay(500);
      }
      
    if(door == door_2)
      {
        digitalWrite(d2_Buzzer, HIGH);
        delay(1000);
        digitalWrite(d2_Buzzer, LOW);
        delay(500);
      }
  }

void DoorLogicControl(uint8_t door)
  {
    //Serial.print("Opening Door ");
    if(OpenTheDoor(door) == true)
      {
        uint16_t timer = door_Open_Delay;
        //Serial.print("Door Opened Delay = ");
        //Serial.println(timer);
        if(door == door_1)
          {
            if(d1_Status == 1)
              {
                //Serial.println("Door Opened");
                while(timer > 0 && d1_Status == 1)
                  {
                    CheckDoorsStatus();
                    timer = timer - 1000;
                    serialEvent();
                  }
                delay(100);
                CheckDoorsStatus();
                if(d1_Status == 1)
                  {
                    while(d1_Status == 1)
                      {
                        BeepBuzzer(door);
                        CheckDoorsStatus();
                        serialEvent();
                      }
                  }
              }
          }
        if(door == door_2)
          {
            if(d2_Status == 2)
              {
                //Serial.println("Door Opened");
                while(timer > 0 && d2_Status == 2)
                  {
                    CheckDoorsStatus();
                    timer = timer - 1000;
                    serialEvent();
                  }
                delay(100);
                CheckDoorsStatus();
                if(d2_Status == 2)
                  {
                    while(d2_Status == 2)
                      {
                        BeepBuzzer(door);
                        CheckDoorsStatus();
                        serialEvent();
                      }
                  }
              }
          }
        delay(5000);  //Delay for actual physical movement of the door to close position.
        CloseTheDoor(door);
      }
  }
