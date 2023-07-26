void InitBoard()
  {
    Serial.begin(9600);
    Serial.setTimeout(1000);
    //Serial.println("Hello World...!");
        
    pinMode(d1_FB_1, INPUT);
    pinMode(d1_FB_2, INPUT);
    pinMode(d1_Switch, INPUT);
    pinMode(d1_Lock, OUTPUT);
    pinMode(d1_Red_LED, OUTPUT);
    pinMode(d1_Green_LED, OUTPUT);
    pinMode(d1_Buzzer, OUTPUT);

    pinMode(d2_FB_1, INPUT);
    pinMode(d2_FB_2, INPUT);
    pinMode(d2_Switch, INPUT);
    pinMode(d2_Lock, OUTPUT);
    pinMode(d2_Red_LED, OUTPUT);
    pinMode(d2_Green_LED, OUTPUT);
    pinMode(d2_Buzzer, OUTPUT);

    digitalWrite(d1_Lock, HIGH);
    digitalWrite(d1_Red_LED, LOW);
    digitalWrite(d1_Green_LED, LOW);
    digitalWrite(d1_Buzzer, LOW);

    digitalWrite(d2_Lock, HIGH);
    digitalWrite(d2_Red_LED, LOW);
    digitalWrite(d2_Green_LED, LOW);
    digitalWrite(d2_Buzzer, LOW);

    GetBoardID();
    delay(1000);
    GetDelays();
    delay(1000);
  }

void GetBoardID()
  {
    if(analogRead(adr_1) > 768)
      board_ID = board_ID | (0x01);
    else
      board_ID = board_ID & ~(0x01);

    if(analogRead(adr_2) > 768)
      board_ID = board_ID | (0x02);
    else
      board_ID = board_ID & ~(0x02);

    if(analogRead(adr_3) > 768)
      board_ID = board_ID | (0x04);
    else
      board_ID = board_ID & ~(0x04);

    if(analogRead(adr_4) > 768)
      board_ID = board_ID | (0x08);
    else
      board_ID = board_ID & ~(0x08);

    board_ID = board_ID * 10;
    //Serial.print("Bord ID = ");
    //Serial.print(board_ID);
  }
