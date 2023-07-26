void getAHUDateTimeValues()
{
  AHUDateTime[DATE] = myNextion.getComponentText(Date,1000);
  AHUDateTime[TIME] = myNextion.getComponentText(Time,1000);
  SerialBT.println(AHUDateTime[DATE]);
  SerialBT.println(AHUDateTime[TIME]);
  convertStringsToValues();
  rtc.adjust(DateTime(AHUDate[YEAR], AHUDate[MONTH], AHUDate[DAY], AHUTime[HOURS], AHUTime[MINUTES], 0));
}

void setAHUDateTimeValues()
{
  convertValuesToStrings();
  myNextion.setComponentText(Date, AHUDateTime[DATE]);
  myNextion.setComponentText(Time, AHUDateTime[TIME]);
}

void setCurDateTimeValues()
{
  convertValuesToStrings();
  myNextion.setComponentText(CurrentDate, AHUDateTime[DATE]);
  myNextion.setComponentText(CurrentTime, AHUDateTime[TIME]);
}

void convertStringsToValues()
{
  int ind1;
  int ind2;
  //Convert Date to Date Values
  ind1 = AHUDateTime[DATE].indexOf('/');  //finds location of first /
  ind2 = AHUDateTime[DATE].indexOf('/', ind1+1);   //finds location of second /             //AHUDateTime[DATE].lastIndexOf('/');  //finds location of last /
  
  AHUDate[DAY] = AHUDateTime[DATE].substring(0, ind1).toInt();
  AHUDate[MONTH] = AHUDateTime[DATE].substring(ind1+1, ind2).toInt();
  AHUDate[YEAR] = AHUDateTime[DATE].substring(ind2+1).toInt();

  SerialBT.println();
  
  SerialBT.println(AHUDate[DAY]);
  SerialBT.println(AHUDate[MONTH]);
  SerialBT.println(AHUDate[YEAR]);

  SerialBT.println();

  //Convert Time to Time Values
  ind1 = AHUDateTime[TIME].indexOf(':');  //finds location of first :

  AHUTime[HOURS] = AHUDateTime[TIME].substring(0, ind1).toInt();
  AHUTime[MINUTES] = AHUDateTime[TIME].substring(ind1+1).toInt();

  SerialBT.println(AHUTime[HOURS]);
  SerialBT.println(AHUTime[MINUTES]);
}

void convertValuesToStrings()
{
  DateTime now = rtc.now();
  
  AHUDate[DAY] = now.day();
  AHUDate[MONTH] = now.month();
  AHUDate[YEAR] = now.year();

  AHUDateTime[DATE] = String(AHUDate[DAY]) + "/" + String(AHUDate[MONTH]) + "/" + String(AHUDate[YEAR]);

  AHUTime[HOURS] = now.hour();
  AHUTime[MINUTES] = now.minute();

  AHUDateTime[TIME] = String(AHUTime[HOURS]) + ":" + String(AHUTime[MINUTES]);
}

/*
String getComponentText(String Str)
{
  boolean stringComplete = false;
  int waitCount = 0;

  String cmd = "";
  int countEnd = 0;
  char _bite;
  char _end = 0xff;//end of file x3

  String tempStr = "get " + Str + ".txt";
    
  Serial2.print(tempStr.c_str());
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.write(0xFF);

  while(stringComplete == false)
    {    
      while(Serial2.available()) 
        {
          // get the new byte:
          _bite = Serial2.read();
          cmd += _bite;
          if(_bite == _end){
            countEnd++;
          }//end if
          if(countEnd == 3){
            stringComplete = true;
          }
        }
      delay(1);
      waitCount++;
      if(waitCount > 1000)
        return "";
    }
  cmd = cmd.substring(1, cmd.length()-3);
  return cmd;
}
*/
