void initModem()
  {
    Wire.begin(I2C_SDA, I2C_SCL);
    bool   isOk = setPowerBoostKeepOn(1);
    String info = "IP5306 KeepOn " + String((isOk ? "PASS" : "FAIL"));
    Serial.println(info);

    gsm.begin(UART_BANUD_RATE, SERIAL_8N1, UART_RX, UART_TX);
    
    Serial.println("Setup Complete!");
    
    //wifi_test();

    if (gprs.preInit()) {
        Serial.println("SIM800 Begin PASS");
    } else {
        Serial.println("SIM800 Begin FAIL");
    }
    
    Serial.println("Test motor ...");
    int i = 3;
    while (i--) {
        gsm.print("AT+SPWM=0,1000,1\r\n");
        updateSerial();
        gsm.print("AT+SPWM=0,0,0\r\n");
        updateSerial();
    }
    delay(200);

    if (0 != gprs.init()) {
        Serial.println("Not checked out to SIM card...");
        while (1);
    }

    // Switch audio channels
    gsm.print("AT+CHFA=1\r\n");
    updateSerial();
    gsm.print("AT+CRSL=100\r\n");
    updateSerial();
    gsm.print("AT+CLVL=100\r\n");
    updateSerial();
    gsm.print("AT+CLIP=1\r\n");
    updateSerial();
    Serial.println("Init success");
    
  }

bool setPowerBoostKeepOn(int en)
{
    Wire.beginTransmission(IP5306_ADDR);
    Wire.write(IP5306_REG_SYS_CTL0);
    if (en)
        Wire.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
    else
        Wire.write(0x35); // 0x37 is default reg value
    return Wire.endTransmission() == 0;
}

void wifi_test()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("no networks found");
        return;
    }
    Serial.println("Found " + String(n) + " networks");
}

void modemLoop()
{
  if (gsm.available()) {
        const char *s = gsm.readStringUntil('\n').c_str();
        if (strstr(s, "OK" ) != NULL) {
            Serial.println("SIM OK");
        } else if (strstr(s, "+CPIN: NOT READY") != NULL) {
            Serial.println("SIM +CPIN: NOT READY");
        } else if (strstr(s, "+CPIN: READY") != NULL) {
            Serial.println("SIM +CPIN: READY");
        } else if (strstr(s, "+CLIP:") != NULL) {
            Serial.printf("SIM %s\n", s);
        } else if (strstr(s, "RING") != NULL) {
            delay(200);
            gsm.write("ATA\r\n");
            Serial.println("SIM RING");
        } else if (strstr(s, "Call Ready") != NULL) {
            Serial.println("SIM Call Ready");
        } else if (strstr(s, "SMS Ready") != NULL) {
            Serial.println("SIM SMS Ready");
        } else if (strstr(s, "NO CARRIER") != NULL) {
            Serial.println("SIM NO CARRIER");
        } else if (strstr(s, "NO DIALTONE") != NULL) {
            Serial.println("SIM NO DIALTONE");
        } else if (strstr(s, "BUSY") != NULL) {
            Serial.println("SIM BUSY");
        } else {
            Serial.print(s);
        }
        Serial.println("==========");
    }

    if (Serial.available()) {
        String r = Serial.readString();
        gsm.write(r.c_str());
    }
}

String checkRinging()
{
  if(gsm.available()) 
    {
        String s = gsm.readStringUntil('\n');
        if(strstr(s.c_str(), "+CLIP:") != NULL)
          {
            String num = midString(s, '"', '"');
            return num;
          }
        else
          return "";
    }
  else
    return "";
}

void SendSMS(char *number_str, char *message_str) 
{
  gsm.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();

  gsm.print(F("AT+CMGS=\""));
  gsm.print(number_str);  
  gsm.println("\"");
  updateSerial();
  
  gsm.print(message_str); //text content
  updateSerial();
  
  gsm.write(26);
}

int IsSMSPresent(byte required_status)
{
  int result = 0;
  char *p_char;
  gsm.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  switch (required_status) 
  {
    case 0:
      gsm.print("AT+CMGL=\"REC UNREAD\"");
      gsm.println(",1");
      break;
    case 1:
      gsm.println("AT+CMGL=\"REC READ\"");
      break;
    case 2:
      gsm.println("AT+CMGL=\"ALL\"");
      break;
  }
  //updateSerial();
  while(gsm.available() <= 0);
  delay(500);
  const char *s = gsm.readString().c_str();
  if (strstr(s, "+CMGL:") != NULL)
  {
    p_char = strchr(s,':');
    if (p_char != NULL) 
        {
          result = atoi(p_char+1);
        }
  }
  return result;
}

char GetSMS(byte position, char *phone_number, char *SMS_text, byte max_SMS_len) 
  {
    char ret_val = -1;
    char *p_char; 
    char *p_char1;
    byte len;
  
    if (position == 0)
      return (-3);
  
    phone_number[0] = 0;  // end of string for now
    
    gsm.println("AT+CMGF=1"); // Configuring TEXT mode
    updateSerial();
    
    //send "AT+CMGR=X" - where X = position
    gsm.print("AT+CMGR=");
    gsm.println((int)position);
  
    while(gsm.available() <= 0);
    delay(500);
    const char *s = gsm.readString().c_str();
    
    if(strstr(s, "+CMGR") != NULL)
      {
        if(strstr(s, "\"REC UNREAD\"") != NULL)
          {
            // extract phone number string
            p_char = strchr(s,',');
            p_char1 = p_char+2; // we are on the first phone number character
            p_char = strchr((char *)(p_char1),'"');
            if (p_char != NULL) 
              {
                *p_char = 0; // end of string
                strcpy(phone_number, (char *)(p_char1));
              }
      
            // get SMS text and copy this text to the SMS_text buffer
            p_char = strchr(p_char+1, '\n');  // find <LF>
            if (p_char != NULL) 
              {
                p_char++; // now we are on the first SMS character 
                // find <CR> as the end of SMS string
                p_char1 = strchr((char *)(p_char), '\r');  
                if(p_char1 != NULL) 
                  *p_char1 = 0; 
                len = strlen(p_char);
        
                if (len < max_SMS_len) 
                  strcpy(SMS_text, (char *)(p_char));
                else 
                  {
                    memcpy(SMS_text, (char *)(p_char), (max_SMS_len-1));
                    SMS_text[max_SMS_len] = 0; // finish string
                  }
                ret_val = 1;
            }
        }
      }
    return ret_val;
  }

char DeleteSMS(byte position)
{
  char ret_val = -1;
  if (position == 0) 
    return (-3);
  
  //send "AT+CMGD=XY" - where XY = position
  gsm.print("AT+CMGD=");
  gsm.println((int)position);  

  while(gsm.available() <= 0);
  delay(500);
  const char *s = gsm.readString().c_str();
    
  if(strstr(s, "OK") != NULL)
    ret_val = 1;
  return ret_val;
}

String midString(String str, char start, char finish)
{
  int locStart = str.indexOf(start);
  if (locStart==-1)
    return "";
  locStart += 1;//start.length();
  int locFinish = str.indexOf(finish, locStart);
  if (locFinish==-1)
    return "";
  return str.substring(locStart, locFinish);
}

void updateSerial()
{
  delay(1000);
  while (Serial.available()) 
  {
    gsm.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(gsm.available()) 
  {
    Serial.write(gsm.read());//Forward what Software Serial received to Serial Port
  }
}
