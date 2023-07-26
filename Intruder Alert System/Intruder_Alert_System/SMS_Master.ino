char sms_position;
char smsbuffer[80];

void Check_New_SMS()
  {
    //-------------------------CHECK FOR NEW SMS-------------------------//
    sms_position=sms.IsSMSPresent(SMS_UNREAD);
    if(sms_position)
      {
        //Read new SMS
        sms.GetSMS(sms_position, phone_number, smsbuffer, 80);
        //Check whether SMS is received from authentic number
        if(Authentic_SMS() == 1)
          SMS_Parser();
        //Delete the SMS to free up memory
        sms.DeleteSMS(sms_position);
      }
  }

char Authentic_SMS()
  {
    String Phone_No = phone_number;
    
    //Check for master numbers. (Eagle number)
    if(Phone_No.indexOf(masterNumber, 0) > -1)
      {
        addAdmin = 1;
        //Serial.println("Msg from Master");
        return 1;
      }
    
    //Check for admin numbers
    for(int8_t i = 0; i < totalAdmins; i++)
      {
        if(Phone_No.indexOf(authenticNumbers[i], 0) > -1)
          {
            msgFrmAdmin = 1;
            //Serial.println("Msg from Admin");
            return 1;
          }
      }
    
    //Check for User numbers
    for(int8_t i = 2; i < totalUsers + 2; i++)
      {
        if(Phone_No.indexOf(authenticNumbers[i], 0) > -1)
          {
            msgFrmUser = 1;
            //Serial.println("Msg from User");
            return 1;
          }
      }
    return 0;
  }

void SMS_Parser()
  {
    String SMS_Data = smsbuffer;  
    String temp;
    String command;
    char length = 0;
    char count;
    String Message;
  
    //Get command string from SMS
    command = SMS_Data.substring(0,4);
    //Serial.println(SMS_Data);
    //Serial.println(command);
    if(command == "ADMI" && addAdmin == 1)
      {
        //Serial.println("Inside Admin");
        int8_t totalAdminAdded = StringSpliter(SMS_Data, authenticNumbers, '\n');
        Message = F("ADMINS \n");
        if(totalAdminAdded >= 1 && totalAdminAdded <= 2)
          {
            writeStringToEEPROM(0, String(totalAdminAdded));   
          } 
        else
          Message = F("INVALID MESSAGE FORMAT...!");

        for(int8_t i = 0; i < totalAdminAdded; i++)
          {
            Message.concat(authenticNumbers[i]);
            Message.concat("\n");
            writeStringToEEPROM((10 + (i*20)), authenticNumbers[i]);
          }
        Message.toCharArray(smsbuffer,80,0);      
        masterNumber.toCharArray(phone_number, 15, 0);
        sms.SendSMS(phone_number, smsbuffer);
        addAdmin = 0;
        //Serial.println("Exiting Admin");
        //Reset to load from EEPROM
        delay(5000);
        gsm.setBusy(1);
        resetFunc(); //call reset 
      }      
    else if(command == "USER" && msgFrmAdmin == 1)
      {
        //Serial.println("Inside User");
        String users[5];
        int8_t totalUsersAdded = StringSpliter(SMS_Data, users, '\n');
        Message = F("USERS \n");
        if(totalUsersAdded >= 1 && totalUsersAdded <= 5)
          {
            writeStringToEEPROM(5, String(totalUsersAdded));                   
          }  
        else
          Message = F("INVALID MESSAGE FORMAT...!");

        for(int8_t i = 0; i < totalUsersAdded; i++)
          {
            authenticNumbers[i+2] = users[i];            
            writeStringToEEPROM((50 + (i*20)), authenticNumbers[i+2]);
            Message.concat(authenticNumbers[i+2]);
            Message.concat("\n");
          }
        Message.toCharArray(smsbuffer,80,0);      
        for(int8_t i = 0; i < totalAdmins; i++)
          {
            authenticNumbers[i].toCharArray(phone_number, 15, 0);
            sms.SendSMS(phone_number, smsbuffer);            
          }
        msgFrmAdmin = 0;
        //Serial.println("Exiting User");
        delay(5000);
        //Reset to load from EEPROM
        gsm.setBusy(1);
        resetFunc(); //call reset 
      }
    else if(command == "VOLU" && msgFrmAdmin == 1)
      {
        //Serial.println("Inside Volume");
        String users[1];
        int8_t vol = StringSpliter(SMS_Data, users, '\n');
        Message = F("Volume has been set to \n");
        if(vol >= 1)
          {
            String cmd = "AT+CLVL=" + String(users[0]);
            cmd.toCharArray(smsbuffer,15,0);
            gsm.setVolume(smsbuffer);
            writeStringToEEPROM(250, String(vol));    
            Message.concat(users[0]);             
          }  
        else
          Message = F("INVALID MESSAGE FORMAT...!");
        
        Message.toCharArray(smsbuffer,80,0);      
        for(int8_t i = 0; i < totalAdmins; i++)
          {
            authenticNumbers[i].toCharArray(phone_number, 15, 0);
            sms.SendSMS(phone_number, smsbuffer);            
          }
        msgFrmAdmin = 0;
        //Serial.println("Exiting Volume");
        delay(5000);
        //Reset to load from EEPROM
        gsm.setBusy(1);
        resetFunc(); //call reset 
      }
    else if(command == "STAR" && (msgFrmAdmin == 1 || msgFrmUser == 1))
      {
        mode = ARMED;
        Message = F("Panel ARMED");
        Message.toCharArray(smsbuffer,80,0);
        sms.SendSMS(phone_number, smsbuffer);
        writeStringToEEPROM(200, "1");
        msgFrmAdmin = 0;
        msgFrmUser = 0;
      }
    else if(command == "STOP" && (msgFrmAdmin == 1 || msgFrmUser == 1))
      {
        mode = NORMAL;
        Message = F("Panel DISARM");
        Message.toCharArray(smsbuffer,80,0);
        sms.SendSMS(phone_number, smsbuffer);
        writeStringToEEPROM(200, "0");
        msgFrmAdmin = 0;
        msgFrmUser = 0;
      }
    else if(command == "MODE" && (msgFrmAdmin == 1 || msgFrmUser == 1))
      {
        Message = F("Current Mode is ");
        if(mode == NORMAL)
          Message = Message + "PANEL DISARMED";
        else
          Message = Message + "PANEL ARMED";
        Message.toCharArray(smsbuffer,80,0);
        sms.SendSMS(phone_number, smsbuffer);
        msgFrmAdmin = 0;
        msgFrmUser = 0;
      }
    else
      {
        Message = F("Invalid Message...!");
        Message.toCharArray(smsbuffer,80,0);
        sms.SendSMS(phone_number, smsbuffer);
        msgFrmAdmin = 0;
        msgFrmUser = 0;
        addAdmin = 0;
      }
  } 

void SendEmergencySMS()
  {
    String Message = "Emergency At Home";
    Message.toCharArray(smsbuffer,80,0);
    for(int8_t i = 2; i < totalUsers + 2; i++)
      {
        authenticNumbers[i].toCharArray(phone_number, 15, 0);
        sms.SendSMS(phone_number, smsbuffer);
        //delay(5000);
      }
  }

void SendTheftSMS()
  {
    String Message = "Theft attending At Home";
    Message.toCharArray(smsbuffer,80,0);
    for(int8_t i = 2; i < totalUsers + 2; i++)
      {
        authenticNumbers[i].toCharArray(phone_number, 15, 0);
        sms.SendSMS(phone_number, smsbuffer);
        //delay(5000);
      }
  }

int8_t StringSpliter(const String &sourceString, String targetArray[], char splitChar)
  {
    String source = sourceString;  
    int8_t len = source.length();
    int8_t numberOfSplitChar = 0;
    int8_t i = 0;
    //Serial.println(source);
    for(i = 0; i < len; i++)
      {
        if(source.charAt(i) == splitChar)
          numberOfSplitChar = numberOfSplitChar + 1;
      }
    String temp = source.substring(source.indexOf(splitChar)+1);
    for(i = 0; i < numberOfSplitChar; i++)
      {
        targetArray[i] = "";
        targetArray[i] = temp.substring(0, temp.indexOf(splitChar));
        temp = temp.substring(temp.indexOf(splitChar)+1);
      }
    return i;
  }
