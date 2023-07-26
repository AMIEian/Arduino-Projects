void serialEvent() 
  {
    while(Serial2.available()) 
      {
        uint8_t data = (uint8_t)Serial2.read();        
        if(data == 'x')
          {
            Serial2.write(door_Array_Status);
            SerialBT.print("Sending Door Array Status = ");
            SerialBT.println(door_Array_Status);
          }
        else if(data == 'y')
          {
            SendDelays();          
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
            Serial2.write(door_Array_Status);
            SerialBT.print("Current Door Array Status = ");
            SerialBT.println(door_Array_Status);
          }
      }
  }

void serialBTEvent() 
  {
    if(SerialBT.available())
      {
        char data = (char)SerialBT.read();
        if(data == 'z')
          {
            //uint8_t count = 0;
            String inputString = "";
            while(SerialBT.available() == 0);
            while(SerialBT.available()) 
              {
                data = (char)SerialBT.read();
                inputString = inputString + data;
                if(data == '#')
                  break;                
              }
            //inputString = inputString + '\n';
            SerialBT.print("Input String = ");
            SerialBT.println(inputString); 
            slaveID = inputString.substring(inputString.lastIndexOf('!')+1, inputString.lastIndexOf('@')).toInt();           
            delays[0] = inputString.substring(inputString.lastIndexOf('@')+1, inputString.lastIndexOf('$')).toInt();
            delays[1] = inputString.substring(inputString.lastIndexOf('$')+1, inputString.lastIndexOf('%')).toInt();
            delays[2] = inputString.substring(inputString.lastIndexOf('%')+1, inputString.lastIndexOf('#')).toInt();
            prefs.putBool("saved", true);
            prefs.putBytes("delays", delays, sizeof(delays));
            prefs.putUInt("slvid", slaveID);
            SerialBT.print("Delays has been updated...");            
            SendDelays();
            ESP.restart();
          }
      }
    while (SerialBT.available()) 
      {
        uint8_t data = (uint8_t)SerialBT.read();
      }
  }
  
/*
void serialBTEvent() 
  {
    if(SerialBT.available())
      {
        uint8_t data = (uint8_t)SerialBT.read();
        if(data == 'z')
          {
            while(SerialBT.available() == 0);
            delays[0] = (uint8_t)SerialBT.read() - 48;
            while(SerialBT.available() == 0);
            delays[1] = (uint8_t)SerialBT.read() - 48;
            while(SerialBT.available() == 0);
            delays[2] = (uint8_t)SerialBT.read() - 48;
            prefs.putBool("saved", true);
            prefs.putBytes("delays", delays, sizeof(delays));
            SerialBT.println("Delays Changed...!");
            SendDelays();    
          }
      }
    while (SerialBT.available()) 
      {
        uint8_t data = (uint8_t)SerialBT.read();
      }
  }
*/

void SendDelays()
  {
    Serial2.print('z');
    Serial2.write(delays[0]);
    Serial2.write(delays[1]);
    Serial2.write(delays[2]);

    SerialBT.println("Sending Delays...");
    SerialBT.print('z');
    SerialBT.print(delays[0]);
    SerialBT.print(delays[1]);
    SerialBT.print(delays[2]);
    SerialBT.println();
  }

void initNode()
  {
    Serial.begin(9600);
    slave.setID(slaveID);
    slave.start();
  }

//Read Modbus data and send to PLC
void modbusTasks()
  {
    GetDoorsData();
    slave.poll(ModbusData, 90);
  }

void GetDoorsData()
  {
    uint8_t index = DOORS_DATA_START;
    uint8_t board = 0;
    uint8_t boardStatus = 0;
    int d1_Status = 0;
    int d1_FB_1 = 0;
    int d1_FB_2 = 0;
    int d1_Switch = 0;
    int d2_Status = 0;
    int d2_FB_1 = 0;
    int d2_FB_2 = 0;
    int d2_Switch = 0;
          
    for(board = 0; board <= boards_Connected; board++)
      {
        uint8_t req = board * 10 + 5;
        Serial2.write(req);
        while(Serial2.available() == 0);        
        boardStatus = (uint8_t)Serial2.read();
        uint8_t res = boardStatus & (0x01);
        if(res != 0)
          d1_Switch = 1;
        else
          d1_Switch = 0;

        res = boardStatus & (0x02);
        if(res != 0)
          d2_Switch = 1;
        else
          d2_Switch = 0;

        res = boardStatus & (0x04);
        if(res != 0)
          d1_Status = 1;
        else
          d1_Status = 0;

        res = boardStatus & (0x04);
        if(res != 0)
          d2_Status = 1;
        else
          d2_Status = 0;

        if(d1_Status != 0)
          {
            d1_FB_1 = 1;
            d1_FB_2 = 1;
          }
        else
          {
            d1_FB_1 = 0;
            d1_FB_2 = 0;
          }

        if(d2_Status != 0)
          {
            d2_FB_1 = 1;
            d2_FB_2 = 1;
          }
        else
          {
            d2_FB_1 = 0;
            d2_FB_2 = 0;
          }

        ModbusData[index] = d1_FB_1;
        index++;
        ModbusData[index] = d1_FB_2;
        index++;
        ModbusData[index] = 0;  //Door_1 Open for Long
        index++;
        ModbusData[index] = 0;  //Door_1 Forced Entry
        index++;
        ModbusData[index] = 0;  //Spare
        index++;

        ModbusData[index] = d2_FB_1;
        index++;
        ModbusData[index] = d2_FB_2;
        index++;
        ModbusData[index] = 0;  //Door_2 Open for Long
        index++;
        ModbusData[index] = 0;  //Door_2 Forced Entry
        index++;
        ModbusData[index] = 0;  //Spare
        index++;
      }
  }
