#define door_1 1
#define door_2 2

#define d1_FB_1 2
#define d1_FB_2 3
#define d1_Switch 4
#define d1_Lock 8
#define d1_Red_LED 9
#define d1_Green_LED 10
#define d1_Buzzer 11

#define d2_FB_1 5
#define d2_FB_2 6
#define d2_Switch 7
#define d2_Lock 12
#define d2_Red_LED 13
#define d2_Green_LED A0
#define d2_Buzzer A1

#define adr_4 A2
#define adr_3 A3
#define adr_2 A6
#define adr_1 A7

const uint8_t d1_ID = 1;
const uint8_t d2_ID = 2;

uint8_t board_ID = 0;
uint8_t bard_Status = 0;
uint8_t d1_Status = 0;
uint8_t d2_Status = 0;
uint8_t door_Array_Status = 0;
uint8_t emergency_Status = 0;

bool d1_Switch_Status = false;
bool d2_Switch_Status = false;
bool d1_Switch_Closed = false;
bool d2_Switch_Closed = false;

uint16_t delays[3] = {2, 5, 20};
uint16_t switch_Delay = 2000;
uint16_t door_Opening_Delay = 5000;
uint16_t door_Open_Delay = 20000;

uint16_t timer = 0;

void setup()
  {
    delay(random(5000, 10000));
    InitBoard();
    /*
    for(int i=0; i<5; i++)
    {
      digitalWrite(d1_Red_LED, HIGH);
      digitalWrite(d1_Green_LED, HIGH);
      digitalWrite(d2_Red_LED, HIGH);
      digitalWrite(d2_Green_LED, HIGH);
      delay(1000);
      digitalWrite(d1_Red_LED, LOW);
      digitalWrite(d1_Green_LED, LOW);
      digitalWrite(d2_Red_LED, LOW);
      digitalWrite(d2_Green_LED, LOW);
      delay(1000);
    }
    */
    CheckDoorsStatus();
    if(d1_Status == 1)
      {
        digitalWrite(d1_Lock, LOW);
        digitalWrite(d1_Red_LED, HIGH);
        digitalWrite(d1_Green_LED, LOW);
        digitalWrite(d1_Buzzer, HIGH);
        //Serial.println("Door 1 Open...");
      }
    else
      {
        digitalWrite(d1_Lock, HIGH);
        digitalWrite(d1_Red_LED, LOW);
        digitalWrite(d1_Green_LED, HIGH);
        digitalWrite(d1_Buzzer, LOW);
        //Serial.println("Door 1 Closed...");
      }
    if(d2_Status == 2)
      {
        digitalWrite(d2_Lock, LOW);
        digitalWrite(d2_Red_LED, HIGH);
        digitalWrite(d2_Green_LED, LOW);
        digitalWrite(d2_Buzzer, HIGH);
        //Serial.println("Door 2 Open...");
      }
    else
      {
        digitalWrite(d2_Lock, HIGH);
        digitalWrite(d2_Red_LED, LOW);
        digitalWrite(d2_Green_LED, HIGH);
        digitalWrite(d2_Buzzer, LOW);
        //Serial.println("Door 2 Closed...");
      }
    UpdateBoardStatus();
    GetDoorArrayStatus();
  }

void loop() 
  {
    CheckHandSwitches();
    if(d1_Switch_Status || d2_Switch_Status)
      {
        //Serial.println("Hand Switch Pressed...");
        GetDoorArrayStatus();
        if(door_Array_Status == 0)
          {
            if(d1_Switch_Status)
              {                
                DoorLogicControl(door_1);
              }
            if(d2_Switch_Status)
              {
                DoorLogicControl(door_2);
              }
          }
      }
    if(door_Array_Status != 0)
      {
        digitalWrite(d1_Red_LED, HIGH);
        digitalWrite(d1_Green_LED, LOW);
        digitalWrite(d2_Red_LED, HIGH);
        digitalWrite(d2_Green_LED, LOW);
      }
    else
      {
        digitalWrite(d1_Red_LED, LOW);
        digitalWrite(d1_Green_LED, HIGH);
        digitalWrite(d2_Red_LED, LOW);
        digitalWrite(d2_Green_LED, HIGH);
      }
  }
