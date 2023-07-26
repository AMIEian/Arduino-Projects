#define On 1
#define off 0
#define full 1
#define Empty 0
#define OK 1
#define Fail 0

#define UU_LED 9
#define UL_LED 10
#define LU_LED 11
#define LL_LED 12
#define Motor_LED 13
#define Booster_LED 3

#define UL_Prob 6
#define UU_Prob 5
#define LL_Prob A1
#define LU_Prob A0
#define Booster_Probe_L A2
#define Booster_Probe_R A3

#define Motor_Relay 8
#define Booster_Relay 7

#define VT_Interrupt 2

char uu = 0;
char ul = 0;
char lu = 0;
char ll = 0;

char txr_not_received = 14;
char level_check = 0, lp_flag = 0, motor_flag = 0;

void display_upper_tank();
void display_lower_tank();

void setup() 
  {
    // put your setup code here, to run once:
    pinMode(UU_LED, OUTPUT);
    pinMode(UL_LED, OUTPUT);
    pinMode(LU_LED, OUTPUT);
    pinMode(LL_LED, OUTPUT);
    pinMode(LL_Prob,OUTPUT);
    pinMode(LU_Prob,OUTPUT);
    pinMode(UL_Prob,INPUT);
    pinMode(UU_Prob,INPUT);
    pinMode(Booster_Probe_L,OUTPUT);
    pinMode(Booster_Probe_R,OUTPUT);
    pinMode(Motor_LED,OUTPUT);
    pinMode(Booster_LED,OUTPUT);
    pinMode(Motor_Relay,OUTPUT);
    pinMode(Booster_Relay,OUTPUT);
    pinMode(VT_Interrupt, INPUT);
    attachInterrupt(digitalPinToInterrupt(VT_Interrupt), Tx_Received, RISING);

    digitalWrite(UU_LED, LOW);
    digitalWrite(UL_LED, LOW);
    digitalWrite(LU_LED, LOW);
    digitalWrite(LL_LED, LOW);
    digitalWrite(LU_Prob, LOW);
    digitalWrite(LL_Prob, LOW);
    digitalWrite(Booster_Probe_L, LOW);
    digitalWrite(Booster_Probe_R, LOW);
    digitalWrite(Motor_LED, LOW);
    digitalWrite(Booster_LED, LOW);
    digitalWrite(Motor_Relay, LOW);
    digitalWrite(Booster_Relay, LOW);
  }
  
void loop() 
  {   
    txr_not_received ++;
    if(txr_not_received > 13)
      txr_not_received = 14;
    if(motor_flag == On)
     {
        if(uu == Empty && ll == full)
          level_check = OK;
        else
          level_check = Fail;
     }
    else 
     {
       if((uu == Empty && ul == Empty) && (lu == full && ll == full))
          level_check = OK;
       else
          level_check = Fail;
     }
  
    if(level_check == OK && txr_not_received <= 13)
     {
       if(lp_flag == off)
        {
           digitalWrite(Motor_Relay,On);   
           lp_flag = On;
           motor_flag = On;
           digitalWrite(Motor_LED, On);
        }
     }
    else
     {
       if(lp_flag == On)
        {
           digitalWrite(Motor_Relay,off);  
           lp_flag = off;
           motor_flag = off;
           digitalWrite(Motor_LED, off);
        }
     }
     check_booster();
     display_upper_tank();
     display_lower_tank();  
     delay(10000);            
  }

// Interrupt
void Tx_Received(void)
  {
    txr_not_received = 0;
    if(digitalRead(UU_Prob) == On)
       {
          delay(10);
          if(digitalRead(UU_Prob) == On)
             {
                uu = full;
                ul = full;
             }
       }
    else if(digitalRead(UL_Prob) == On)
       {
          delay(10);
          if(digitalRead(UL_Prob) == On)
            {
                uu = Empty;
                ul = full;
            }
       }
     else
      {
         uu = Empty;
         ul = Empty;
      }
      
    //MAKE ALL PROBE 0
    
    digitalWrite(LU_Prob,LOW);
    digitalWrite(LL_Prob,LOW);
    pinMode(LL_Prob, OUTPUT);
    pinMode(LU_Prob, INPUT);
    digitalWrite(LL_Prob,HIGH);  
    delay(10);   
    if( digitalRead(LU_Prob) == On )
       {
          delay(10);
          if(digitalRead(LU_Prob) == On)
             {                  
                lu = full;
                ll = full;
             }
       }
    else
       {
          lu = Empty;
          pinMode(LL_Prob, OUTPUT);
          pinMode(LU_Prob, OUTPUT);
          digitalWrite(LU_Prob,LOW);
          digitalWrite(LL_Prob,LOW);
          pinMode(LL_Prob, INPUT);
          pinMode(LU_Prob, OUTPUT);
          digitalWrite(LU_Prob,HIGH);
          delay(10);
          if(digitalRead(LL_Prob) == On)
             {
                delay(10);
                if(digitalRead(LL_Prob) == On)
                  ll = full;
             }
          else
            ll = Empty;
       }
    pinMode(LL_Prob, OUTPUT);
    pinMode(LU_Prob, OUTPUT);
    digitalWrite(LU_Prob,LOW);
    digitalWrite(LL_Prob,LOW);
  }

void check_booster()
  {
    pinMode(Booster_Probe_L, OUTPUT);
    pinMode(Booster_Probe_R, INPUT);
    digitalWrite(Booster_Probe_L,HIGH);  
    delay(10);   
    if(digitalRead(Booster_Probe_R) == On)
       {
          delay(10);
          if(digitalRead(Booster_Probe_R) == On)
             {                  
                pinMode(Booster_Probe_R, OUTPUT);
                pinMode(Booster_Probe_L, INPUT);
                digitalWrite(Booster_Probe_R,HIGH);  
                delay(10);   
                if(digitalRead(Booster_Probe_L) == On)
                  {
                    delay(10);
                    if(digitalRead(Booster_Probe_L) == On)
                      {
                        digitalWrite(Booster_Relay,HIGH);
                        digitalWrite(Booster_LED,HIGH);  
                      }
                    else
                      {
                        digitalWrite(Booster_Relay,LOW);
                        digitalWrite(Booster_LED,LOW);  
                      }
                  }
             }
       }
    pinMode(Booster_Probe_R, OUTPUT);
    pinMode(Booster_Probe_L, OUTPUT);
    digitalWrite(Booster_Probe_R,LOW);
    digitalWrite(Booster_Probe_L,LOW);  
  }

void display_upper_tank()
  {
    if(uu == full)
      digitalWrite(UU_LED,HIGH);
    else
      digitalWrite(UU_LED,LOW);
    if(ul == full)
      digitalWrite(UL_LED,HIGH);
    else
      digitalWrite(UL_LED,LOW); 
  }

void display_lower_tank()
  {
    if(lu == full)
      digitalWrite(LU_LED,HIGH);
    else
       digitalWrite(LU_LED,LOW);
    if(ll == full)
      digitalWrite(LL_LED,HIGH);
    else
      digitalWrite(LL_LED,LOW); 
  }
