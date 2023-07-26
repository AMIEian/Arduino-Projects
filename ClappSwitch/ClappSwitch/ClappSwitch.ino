const int transistorpin = 13;
int sensorvalue=0;
int eventstatus = 0;
int sensorthresholdhigh = 1000;
int sensorthresholdlow = 300;
boolean blinking = false;
int currentmillis = 0;
int toggle_millis = 0;
int toggle_time = 1000;

void setup () {
  Serial.begin(9600);
  pinMode(transistorpin, OUTPUT);
}

void loop() { 
  eventstatus = 0;  
  sensorvalue = analogRead(A2);  
  delay(1);
  
  if(eventstatus==0){
    if (sensorvalue < sensorthresholdlow){
      eventstatus=1;
      Serial.println(eventstatus);      
    }
  }
  
  if(eventstatus==1){  
    delay(100);
    for(int k = 500; k>0;k--){
      delay(1);
      sensorvalue=analogRead(A2);
      if (sensorvalue < sensorthresholdlow){
        eventstatus=2;
        Serial.println(eventstatus);
        break;
      }      
    }
  }
  
  if(eventstatus==2){
    delay(100);
    for(int n=500;n>0;n--){
      delay(1);
      sensorvalue=analogRead(A2);
      if (sensorvalue < sensorthresholdlow){
        eventstatus=3;
      Serial.println(eventstatus);
      break;
      }
      
    }
  }
  
  if(eventstatus>=2){
    //digitalWrite(transistorpin, !digitalRead(transistorpin));
    blinking = !blinking;
    if(blinking == false)
      digitalWrite(transistorpin, LOW);
  }
  
  if(blinking) {
    currentmillis = millis();   //Get current millis
    if(currentmillis - toggle_millis >= toggle_time)
        {
            toggle_millis = currentmillis;
            digitalWrite(transistorpin, !digitalRead(transistorpin));
        }
  }
}
