void getSchedularValues()
{
  int16_t tempSch[56];
  int index = 0;

  schedulerEnabled = myNextion.getComponentValue(SchedulerEnabled);
  
  if(schedulerEnabled == 1)
    {
      prefs.putBool("SCH", true);
      for(int i = 0; i < 7; i++)
        {
          for(int j = 0; j < 8; j++)
            {
              SCHEDULES[i][j] = myNextion.getComponentValue(Scheduls[i][j]);
              tempSch[index] = SCHEDULES[i][j];
              index++;
            }
        }

     for(int weekDay = 0; weekDay < 7; weekDay++)
      {
        if( (SCHEDULES[weekDay][ON_TIME_1] != -1 && SCHEDULES[weekDay][OFF_TIME_1] == -1) ||
            (SCHEDULES[weekDay][ON_TIME_1] == -1 && SCHEDULES[weekDay][OFF_TIME_1] != -1) )
            {
              SCHEDULES[weekDay][ON_TIME_1] = -1;
              SCHEDULES[weekDay][OFF_TIME_1] = -1;
            }

        if( (SCHEDULES[weekDay][ON_TIME_2] != -1 && SCHEDULES[weekDay][OFF_TIME_2] == -1) ||
            (SCHEDULES[weekDay][ON_TIME_2] == -1 && SCHEDULES[weekDay][OFF_TIME_2] != -1) )
            {
              SCHEDULES[weekDay][ON_TIME_2] = -1;
              SCHEDULES[weekDay][OFF_TIME_2] = -1;
            }

        if( (SCHEDULES[weekDay][ON_TIME_3] != -1 && SCHEDULES[weekDay][OFF_TIME_3] == -1) ||
            (SCHEDULES[weekDay][ON_TIME_3] == -1 && SCHEDULES[weekDay][OFF_TIME_3] != -1) )
            {
              SCHEDULES[weekDay][ON_TIME_3] = -1;
              SCHEDULES[weekDay][OFF_TIME_3] = -1;
            }

        if( (SCHEDULES[weekDay][ON_TIME_4] != -1 && SCHEDULES[weekDay][OFF_TIME_4] == -1) ||
            (SCHEDULES[weekDay][ON_TIME_4] == -1 && SCHEDULES[weekDay][OFF_TIME_4] != -1) )
            {
              SCHEDULES[weekDay][ON_TIME_4] = -1;
              SCHEDULES[weekDay][OFF_TIME_4] = -1;
            }
        /*
        if(SCHEDULES[weekDay][ON_TIME_1] == 0)
          SCHEDULES[weekDay][ON_TIME_1] = -1;

        if(SCHEDULES[weekDay][ON_TIME_2] == 0)
          SCHEDULES[weekDay][ON_TIME_2] = -1;

        if(SCHEDULES[weekDay][ON_TIME_3] == 0)
          SCHEDULES[weekDay][ON_TIME_3] = -1;

        if(SCHEDULES[weekDay][ON_TIME_4] == 0)
          SCHEDULES[weekDay][ON_TIME_4] = -1;

        if(SCHEDULES[weekDay][OFF_TIME_1] == 0)
          SCHEDULES[weekDay][OFF_TIME_1] = -1;

        if(SCHEDULES[weekDay][OFF_TIME_2] == 0)
          SCHEDULES[weekDay][OFF_TIME_2] = -1;

        if(SCHEDULES[weekDay][OFF_TIME_3] == 0)
          SCHEDULES[weekDay][OFF_TIME_3] = -1;

        if(SCHEDULES[weekDay][OFF_TIME_4] == 0)
          SCHEDULES[weekDay][OFF_TIME_4] = -1;
        */
      }

      index = 0;
      for(int i = 0; i < 7; i++)
        {
          for(int j = 0; j < 8; j++)
            {
              tempSch[index] = SCHEDULES[i][j];
              index++;
            }
        }
        
      prefs.putBytes("SCHEDUL", tempSch, sizeof(tempSch));
    
      for(int i = 0; i < 56; i++)
        tempSch[i] = 99;
    
      prefs.getBytes("SCHEDUL", tempSch, sizeof(tempSch));
    
      for(int i = 0; i < 56; i++)
        SerialBT.println(tempSch[i]);
    }
  else
    {
      prefs.putBool("SCH", false);
      prefs.putBool("HOL", false);
    }
}

void setSchedularValues()
{
  int16_t tempSch[56];
  int index = 0;
  bool saved = prefs.getBool("SCH");
  if(saved == true)
    {
      SerialBT.println("Schedular Settings Found...");
      SerialBT.println("Setting them to display...");
      
      schedulerEnabled = 1;   
      myNextion.setComponentValue(SchedulerEnabled, schedulerEnabled);
      
      prefs.getBytes("SCHEDUL", tempSch, sizeof(tempSch));

      for(int i = 0; i < 7; i++)
        {
          for(int j = 0; j < 8; j++)
            {
              SCHEDULES[i][j] = tempSch[index];
              myNextion.setComponentValue(Scheduls[i][j], SCHEDULES[i][j]);
              index++;
            }
        }
    }
  else
    {
      schedulerEnabled = 0;
    }
}
