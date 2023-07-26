void updateIOScreen_1()
  {
    myNextion.setComponentText("l1", String(counts[AHU_On_Off]));
    myNextion.setComponentText("l2", String(counts[AHU_Trip]));
    myNextion.setComponentText("l3", String(counts[Filter_1]));
    myNextion.setComponentText("l4", String(counts[Filter_2]));
    myNextion.setComponentText("l5", String(counts[Fire_Input]));
    myNextion.setComponentText("l6", String(counts[Fire_Supply]));
    myNextion.setComponentText("l7", String(counts[Fire_Return]));
    myNextion.setComponentText("l8", String(counts[AHU_Door]));
    
    if(counts[AHU_On_Off] == 1)
      myNextion.setComponentText("v1", "On");
    else
      myNextion.setComponentText("v1", "Off");

    if(counts[AHU_Trip] == 1)
      myNextion.setComponentText("v2", "Trip");
    else
      myNextion.setComponentText("v2", "Healthy");

    if(counts[Filter_1] == 1)
      myNextion.setComponentText("v3", "Clog");
    else
      myNextion.setComponentText("v3", "Healthy");

    if(counts[Filter_2] == 1)
      myNextion.setComponentText("v4", "Clog");
    else
      myNextion.setComponentText("v4", "Healthy");

    if(counts[Fire_Input] == 1)
      myNextion.setComponentText("v5", "Safe");
    else
      myNextion.setComponentText("v5", "Danger");

    if(counts[Fire_Supply] == 1)
      myNextion.setComponentText("v6", "Open");
    else
      myNextion.setComponentText("v6", "Closed");

    if(counts[Fire_Return] == 1)
      myNextion.setComponentText("v7", "Open");
    else
      myNextion.setComponentText("v7", "Closed");

    if(counts[AHU_Door] == 1)
      myNextion.setComponentText("v8", "Closed");
    else
      myNextion.setComponentText("v8", "Open");
  }

void updateIOScreen_2()
  {
    myNextion.setComponentText("l1", String(counts[RA_Temp_AI]));
    myNextion.setComponentText("l2", String(counts[RA_Humidity_AI]));
    myNextion.setComponentText("l3", String(counts[VFD_Speed_AI]));
    myNextion.setComponentText("l4", String(counts[Air_Velocity_AI]));
    myNextion.setComponentText("l5", String(counts[CO2_AI]));
    myNextion.setComponentText("l6", String(counts[SPARE_AI]));
    myNextion.setComponentText("l7", "");
    myNextion.setComponentText("l8", "");
  }

void updateIOScreen_3()
  {
    
  }

void updateIOScreen_4()
  {
    
  }
