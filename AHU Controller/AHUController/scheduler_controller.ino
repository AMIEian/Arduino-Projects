void checkScheduler()
{
  if(schedulerEnabled == 1 && schedulerTempDisable == 0)
    {
      if(todayIsHoliday() == false)
        {
          int currentTime = getTimeInMins();
          int today = getCurrentDay();

          SerialBT.println(currentTime);
          SerialBT.println(SCHEDULES[today][ON_TIME_1]);
          SerialBT.println(SCHEDULES[today][OFF_TIME_1]);
          SerialBT.println(SCHEDULES[today][ON_TIME_2]);
          SerialBT.println(SCHEDULES[today][OFF_TIME_2]);
          SerialBT.println(SCHEDULES[today][ON_TIME_3]);
          SerialBT.println(SCHEDULES[today][OFF_TIME_3]);
          SerialBT.println(SCHEDULES[today][ON_TIME_4]);
          SerialBT.println(SCHEDULES[today][OFF_TIME_4]);
          
          if(currentTime >= SCHEDULES[today][ON_TIME_1] && currentTime <= SCHEDULES[today][OFF_TIME_1])
              {
                if(AHUSetting[ahuStatus] == AHU_OFF)
                  {
                    startAHU();
                    onBySchedular = true;
                    prefs.putBool("SCHON", true);
                  }
              }
          else if(currentTime >= SCHEDULES[today][ON_TIME_2] && currentTime <= SCHEDULES[today][OFF_TIME_2])
              {
                if(AHUSetting[ahuStatus] == AHU_OFF)
                  {
                    startAHU();
                    onBySchedular = true;
                    prefs.putBool("SCHON", true);
                  }
              }
          else if(currentTime >= SCHEDULES[today][ON_TIME_3] && currentTime <= SCHEDULES[today][OFF_TIME_3])
              {
                if(AHUSetting[ahuStatus] == AHU_OFF)
                  {
                    startAHU();
                    onBySchedular = true;
                    prefs.putBool("SCHON", true);
                  }
              }
          else if(currentTime >= SCHEDULES[today][ON_TIME_4] && currentTime <= SCHEDULES[today][OFF_TIME_4])
              {
                if(AHUSetting[ahuStatus] == AHU_OFF)
                  {
                    startAHU();
                    onBySchedular = true;
                    prefs.putBool("SCHON", true);
                  }
              }
          else
              {
                if(AHUSetting[ahuStatus] == AHU_ON && onBySchedular == true)
                  {
                    stopAHU();
                    onBySchedular = false;
                    prefs.putBool("SCHON", false);
                  }
              }
                    
        }
    }
  else
    {
      if(AHUSetting[ahuStatus] == AHU_ON && onBySchedular == true)
        {
          stopAHU();
          onBySchedular = false;
        }
    }
}

int getTimeInMins()
{
  DateTime now = rtc.now();

  int mins = (now.hour() * 60) + now.minute();
  return mins;
}

int getCurrentDay()
{
  DateTime now = rtc.now();

  int d = now.dayOfTheWeek();
  return d;
}
