void getHolidaysValues()
{
  HOLIDAYS[WEEKLY_OFF] = myNextion.getComponentValue(WeeklyOff);
  HOLIDAYS[SEC_WEEKLY_OFF] = myNextion.getComponentValue(SecWeeklyOff);
  HOLIDAYS[ALT_WEEKLY_OFF] = myNextion.getComponentValue(AltWeeklyOff);
  
  prefs.putBool("HOL", true);
  prefs.putBytes("HOLIDAY", HOLIDAYS, sizeof(HOLIDAYS));

  for(int i = 0; i < 3; i++)
    HOLIDAYS[i] = 99;

  prefs.getBytes("HOLIDAY", HOLIDAYS, sizeof(HOLIDAYS));
    
  for(int i = 0; i < 3; i++)
    SerialBT.println(HOLIDAYS[i]);

  SerialBT.println("------------------------");
  String str;
  String str1;
  String str2;
  for(int i = 1; i <= 48; i++)
    {
      str = "mtnSettings.holiday_" + String(i);
      str2 = "holiday_" + String(i);
      SpecialHolidays[i-1] = myNextion.getComponentText(str,1000);
      SerialBT.println(str);
      SerialBT.println(SpecialHolidays[i-1]);
      prefs.putString(str2.c_str(), SpecialHolidays[i-1]);
      SerialBT.println("SAVED");
      str1 = prefs.getString(str2.c_str(), "DD/MM/YYYY");
      SerialBT.println(str1);
      if(SpecialHolidays[i-1] == "DD/MM/YYYY")
        break;
    }
  SerialBT.println("------------------------");
}

void setHolidaysValues()
{
  String str;
  String str2;
  bool sch_saved = prefs.getBool("SCH");
  bool hol_saved = prefs.getBool("HOL");
  if(sch_saved == true && hol_saved == true)
    {
      SerialBT.println("Holidays Settings Found...");
      SerialBT.println("Setting them to display...");
      
      prefs.getBytes("HOLIDAY", HOLIDAYS, sizeof(HOLIDAYS));

      myNextion.setComponentValue(WeeklyOff, HOLIDAYS[WEEKLY_OFF]);
      myNextion.setComponentValue(SecWeeklyOff, HOLIDAYS[SEC_WEEKLY_OFF]);
      myNextion.setComponentValue(AltWeeklyOff, HOLIDAYS[ALT_WEEKLY_OFF]);

      SerialBT.println("------------------------");
      for(int i = 1; i <= 48; i++)
        {
          str = "mtnSettings.holiday_" + String(i);
          str2 = "holiday_" + String(i);
          SerialBT.println(str);
          SpecialHolidays[i-1] = prefs.getString(str2.c_str(), "DD/MM/YYYY");
          SerialBT.println(SpecialHolidays[i-1]);
          SerialBT.println("   ");
          if(SpecialHolidays[i-1] == "DD/MM/YYYY")
            break;
          else
            myNextion.setComponentText(str, SpecialHolidays[i-1]);
        }
      SerialBT.println("------------------------");
    }
  else
    {
      HOLIDAYS[WEEKLY_OFF] = -1;
      HOLIDAYS[SEC_WEEKLY_OFF] = -1;
      HOLIDAYS[ALT_WEEKLY_OFF] = -1;

      for(int i = 0; i < 48; i++)
        SpecialHolidays[i] = "DD/MM/YYYY";
    }
}
