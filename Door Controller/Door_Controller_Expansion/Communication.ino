void UpdateBoardStatus()
  {
    bard_Status = 0;
    bard_Status = board_ID + d1_Status + d2_Status;
    Serial.write(bard_Status);
  }

void GetDoorArrayStatus()
  {
    Serial.print('x');
    timer = 1000;
    while(Serial.available() == 0 && timer > 0)
      {
        timer = timer - 10;
        delay(10);
      }
    if(timer == 0)
      {
        door_Array_Status = 1;
        return;
      }
    door_Array_Status = (uint8_t)Serial.read();
  }

void GetDelays()
  {
    Serial.print('y');
    timer = 1000;
    while(Serial.available() == 0 && timer > 0)
      {
        timer = timer - 10;
        delay(10);
      }
    if(timer == 0)
      {
        return;
      }
    char startFlag = (uint8_t)Serial.read();
    //while(Serial.available() == 0);
    delays[0] = (uint8_t)Serial.read();
    //while(Serial.available() == 0);
    delays[1] = (uint8_t)Serial.read();
    //while(Serial.available() == 0);
    delays[2] = (uint8_t)Serial.read();
    switch_Delay = delays[0] * 1000;
    door_Opening_Delay = delays[1] * 1000;
    door_Open_Delay = delays[2] * 1000;
  }

void SendBoardStatus()
  {
    uint8_t boardStatus = 0;
    if(d1_Switch_Status == true)
      boardStatus = boardStatus | (0x01);
    else
      boardStatus = boardStatus & ~(0x01);
            
    if(d2_Switch_Status == true)
      boardStatus = boardStatus | (0x02);
    else
      boardStatus = boardStatus & ~(0x02);

    if(d1_Status == 1)
      boardStatus = boardStatus | (0x04);
    else
      boardStatus = boardStatus & ~(0x04);      
      
    if(d2_Status == 2)
      boardStatus = boardStatus | (0x08);
    else
      boardStatus = boardStatus & ~(0x08);

    Serial.write(boardStatus);
  }

void serialEvent() 
  {
    while(Serial.available()) 
      {
        uint8_t data = (uint8_t)Serial.read();
        if(data == 'e')
          {
            emergency_Status = 1;
            EmergencyOpen();
          }
        else if(data == 'z')
          {
            while(Serial.available() == 0);
            delays[0] = (uint8_t)Serial.read();
            while(Serial.available() == 0);
            delays[1] = (uint8_t)Serial.read();
            while(Serial.available() == 0);
            delays[2] = (uint8_t)Serial.read();
            switch_Delay = delays[0] * 1000;
            door_Opening_Delay = delays[1] * 1000;
            door_Open_Delay = delays[2] * 1000;
          }
        else if(data == (board_ID + 5))
          {
            SendBoardStatus();
          }
        else
          {
            uint8_t board = 0;
            uint8_t door = 0;
            board = data / 10;
            door = data % 10;      
            if(board >= 0 && board <= 7)   
              {
                doors_Status[board] = door;
                if(door == 1 || door == 2)
                  door_Array_Status = 1;
                else if(door == 0)
                  door_Array_Status = 0;              
              }           
          }
      }
  }
