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
        if(Authentic_SMS())
          SMS_Parser();
        //Delete the SMS to free up memory
        sms.DeleteSMS(sms_position);
      }
  }

boolean Authentic_SMS()
  {
    String Phone_No = phone_number;
    
    //Check for master numbers. (Eagle number)
    if(Phone_No.indexOf(masterNumber, 0) > -1)
      {
        addAdmin = true;
        return true;
      }
    
    //Check for admin numbers
    for(int8_t i = 0; i < totalAdmins; i++)
      {
        if(Phone_No.indexOf(authenticNumbers[i], 0) > -1)
          {
            msgFrmAdmin = true;
            return true;
          }
      }
    
    //Check for User numbers
    for(int8_t i = 2; i < totalUsers + 2; i++)
      {
        if(Phone_No.indexOf(authenticNumbers[i], 0) > -1)
          {
            msgFrmUser = true;
            return true;
          }
      }
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
    //Serial.println(command);
    //Serial.println(SMS_Data);
    if(command == "ADMI" && addAdmin == true)
      {
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
        addAdmin = false;
      }
      
    if(command == "USER" && msgFrmAdmin == true)
      {
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
            Message.concat(authenticNumbers[i+2]);
            Message.concat("\n");
            writeStringToEEPROM((50 + (i*20)), authenticNumbers[i+2]);
          }
        Message.toCharArray(smsbuffer,80,0);      
        for(int8_t i = 0; i < totalAdmins; i++)
          {
            authenticNumbers[i].toCharArray(phone_number, 15, 0);
            sms.SendSMS(phone_number, smsbuffer);
          }
        msgFrmAdmin = false;
      }

    if(command == "STAR" && (msgFrmAdmin == true || msgFrmUser == true))
      {
        mode = ARMED;
        Message = F("Panel ARMED");
        Message.toCharArray(smsbuffer,80,0);
        sms.SendSMS(phone_number, smsbuffer);
        writeStringToEEPROM(200, "1");
        msgFrmAdmin = false;
        msgFrmUser = false;
      }

    if(command == "STOP" && (msgFrmAdmin == true || msgFrmUser == true))
      {
        mode = NORMAL;
        Message = F("Panel DISARM");
        Message.toCharArray(smsbuffer,80,0);
        sms.SendSMS(phone_number, smsbuffer);
        writeStringToEEPROM(200, "0");
        msgFrmAdmin = false;
        msgFrmUser = false;
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
      }
  }

int8_t StringSpliter(const String &sourceString, String targetArray[], char splitChar)
  {
    String source = sourceString;  
    int8_t len = source.length();
    int8_t numberOfSplitChar = 0;
    int8_t i;
    
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
