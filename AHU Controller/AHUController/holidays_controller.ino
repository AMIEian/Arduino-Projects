bool todayIsHoliday()
{
  DateTime CurrentTime = rtc.now();
  int dayofweek = CurrentTime.dayOfTheWeek();
  //Check for weekly off
  if(dayofweek == HOLIDAYS[WEEKLY_OFF] || dayofweek == HOLIDAYS[SEC_WEEKLY_OFF] || dayofweek == HOLIDAYS[ALT_WEEKLY_OFF])
    return true;
  else
    {
      //Check for Special Holiday
      String today = convertDateToString();
      for(int i = 0; i < 48; i++)
        {
          if(today == SpecialHolidays[i])
            return true;
        }
    }
  return false;
}

String convertDateToString()
{
  DateTime now = rtc.now();
  return (String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()));
}
