void setup() {
  Serial.begin(9600);
  String SMS_Data = "ADMIN";
  SMS_Data = SMS_Data + "\n";
  SMS_Data = SMS_Data + "9819895913";
  SMS_Data = SMS_Data + "\n";
  SMS_Data = SMS_Data + "9321178317";
  //SMS_Data = SMS_Data + "\n";
  String authenticNumbers[5];
  StringSpliter(SMS_Data, authenticNumbers, '\n');
  /*
  SMS_Data = SMS_Data + "\n";
  SMS_Data = SMS_Data + "9850139005";
  SMS_Data = SMS_Data + "\n";
  SMS_Data = SMS_Data + "7588093105";
  SMS_Data = SMS_Data + "\n";
  SMS_Data = SMS_Data + "9967910423";
  SMS_Data = SMS_Data + "\n";
  
  /*
  int numberOfnewLine = 0;
  int len = SMS_Data.length();
  for(int i = 0; i < len; i++)
    {
      if(SMS_Data.charAt(i) == '\n')
        numberOfnewLine = numberOfnewLine + 1;
    }
  Serial.println(numberOfnewLine);
  
  String temp = SMS_Data.substring(SMS_Data.indexOf('\n')+1, SMS_Data.lastIndexOf('\n'));
  for(int i = 0; i < numberOfnewLine; i++)
    {
      authenticNumbers[i] = temp.substring(0, temp.indexOf('\n'));
      temp = temp.substring(temp.indexOf('\n')+1);
    }
  */
  //Serial.println(SMS_Data);
  Serial.println("--------------------------");
  Serial.println(authenticNumbers[0]);
  Serial.println("--------------------------");
  Serial.println("--------------------------");
  Serial.println(authenticNumbers[1]);
  Serial.println("--------------------------");
  Serial.println("--------------------------");
  Serial.println(authenticNumbers[2]);
  Serial.println("--------------------------");
  Serial.println("--------------------------");
  Serial.println(authenticNumbers[3]);
  Serial.println("--------------------------");
  Serial.println("--------------------------");
  Serial.println(authenticNumbers[4]);
  Serial.println("--------------------------");
  //authenticNumbers[0] = temp.substring(0, temp.indexOf('\n'));
  //authenticNumbers[1] = temp.substring(temp.indexOf('\n')+1);
  //Serial.println(authenticNumbers[0]);
  //Serial.println("--------------------------");
  //Serial.println(authenticNumbers[1]);
  //Serial.println("--------------------------");
}

void loop() {
  // put your main code here, to run repeatedly:

}

int8_t StringSpliter(const String &sourceString, String targetArray[], char splitChar)
  {
    String source = sourceString;  
    int8_t len = source.length();
    int8_t numberOfSplitChar = 0;
    int8_t i;
    Serial.println("-----------------------------");
    //Serial.println(source);
    for(i = 0; i < len; i++)
      {
        if(source.charAt(i) == splitChar)
          numberOfSplitChar = numberOfSplitChar + 1;
      }
    Serial.println(numberOfSplitChar);
    String temp = source.substring(source.indexOf(splitChar)+1);
    for(i = 0; i < numberOfSplitChar; i++)
      {
        Serial.println(temp);
        Serial.println("============================");
        targetArray[i] = "";
        targetArray[i] = temp.substring(0, temp.indexOf(splitChar));
        //Serial.println(targetArray[i]);
        temp = temp.substring(temp.indexOf(splitChar)+1);
      }
    Serial.println(i);
    Serial.println("-----------------------------");
    return i;
  }
