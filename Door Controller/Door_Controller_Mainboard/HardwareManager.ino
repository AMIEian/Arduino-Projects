void CheckEmergency()
  {
    if(digitalRead(fireInput) == LOW || digitalRead(emergencyInput) == LOW)
      {
        //Fire or Emergency Input Pressed. Send Emergency Signal.
        uint8_t command = 101;
        Serial2.write(command);
        door_Array_Status = 101;
        for(uint8_t i = 0; i < 8; i++)
          doors_Status[i] = 3;
        if(digitalRead(fireInput) == LOW)
          {
            //Fire Input Received. Turn on hooter
            digitalWrite(hooter, HIGH);
            ModbusData[FIRE_ALARM] = 1;
            ModbusData[AUDIO_ALARM] = 1;
          }
        if(digitalRead(emergencyInput) == LOW)
          {
            //Emergency Input Received. Turn on hooter
            ModbusData[EMERGENCY_PRESSED] = 1;            
          }
      }
    else
      {
        digitalWrite(hooter, LOW);
        ModbusData[FIRE_ALARM] = 0;
        ModbusData[AUDIO_ALARM] = 0;
        ModbusData[EMERGENCY_PRESSED] = 0;
      }
  }

void ModbusControl()
  {
    if(ModbusData[DISABLE_INTERLOCK] == 1)
      {
        door_Array_Status = 0;
        interlockDisabled = true;
      }
    else
      {
        door_Array_Status = 0;
        interlockDisabled = false;
      }

    if(ModbusData[EMERGENCY_OPEN] == 1)
      {
        uint8_t command = 101;
        Serial2.write(command);
        door_Array_Status = 101;
        for(uint8_t i = 0; i < 8; i++)
          doors_Status[i] = 3;
      }
  }
