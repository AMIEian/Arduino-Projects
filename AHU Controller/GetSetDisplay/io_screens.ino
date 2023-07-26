void updateIOScreen_1()
  {
    myNextion.setComponentText("l1", String(inputs[AHU_On_Off]));
    myNextion.setComponentText("l2", String(inputs[AHU_Trip]));
    myNextion.setComponentText("l3", String(inputs[Filter_1]));
    myNextion.setComponentText("l4", String(inputs[Filter_2]));
    myNextion.setComponentText("l5", String(inputs[Fire_Input]));
    myNextion.setComponentText("l6", String(inputs[Fire_Supply]));
    myNextion.setComponentText("l7", String(inputs[Fire_Return]));
    myNextion.setComponentText("l8", String(inputs[AHU_Door]));
    
    if(inputs[AHU_On_Off] == 1)
      myNextion.setComponentText("v1", "On");
    else
      myNextion.setComponentText("v1", "Off");

    if(inputs[AHU_Trip] == 1)
      myNextion.setComponentText("v2", "Trip");
    else
      myNextion.setComponentText("v2", "Healthy");

    if(inputs[Filter_1] == 1)
      myNextion.setComponentText("v3", "Clog");
    else
      myNextion.setComponentText("v3", "Healthy");

    if(inputs[Filter_2] == 1)
      myNextion.setComponentText("v4", "Clog");
    else
      myNextion.setComponentText("v4", "Healthy");

    if(inputs[Fire_Input] == 1)
      myNextion.setComponentText("v5", "Safe");
    else
      myNextion.setComponentText("v5", "Danger");

    if(inputs[Fire_Supply] == 1)
      myNextion.setComponentText("v6", "Open");
    else
      myNextion.setComponentText("v6", "Closed");

    if(inputs[Fire_Return] == 1)
      myNextion.setComponentText("v7", "Open");
    else
      myNextion.setComponentText("v7", "Closed");

    if(inputs[AHU_Door] == 1)
      myNextion.setComponentText("v8", "Closed");
    else
      myNextion.setComponentText("v8", "Open");
  }

void updateIOScreen_2()
  {
    int result;
    int mask = 1;
    
    result = outputs[RO_INDEX] & mask;
    myNextion.setComponentText("l1", String(result));
    if(result == 1)
      myNextion.setComponentText("v1", "On");
    else
      myNextion.setComponentText("v1", "Off");
      
    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    myNextion.setComponentText("l2", String(result));
    if(result == 1)
      myNextion.setComponentText("v2", "Open");
    else
      myNextion.setComponentText("v2", "Closed");

    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    myNextion.setComponentText("l3", String(result));
    if(result == 1)
      myNextion.setComponentText("v3", "Open");
    else
      myNextion.setComponentText("v3", "Closed");

    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    myNextion.setComponentText("l4", String(result));
    if(result == 1)
      myNextion.setComponentText("v4", "Open");
    else
      myNextion.setComponentText("v4", "Closed");

    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    myNextion.setComponentText("l5", String(result));

    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    myNextion.setComponentText("l6", String(result));

    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    myNextion.setComponentText("l7", String(result));

    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    myNextion.setComponentText("l8", String(result));
  }

void updateIOScreen_3()
  {
    float val;
    val = map(inputs[RA_Temp_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
    myNextion.setComponentText("l1", String(val) + " V");
    val = map(inputs[RA_Temp_AI], 0, ADC_MAX_COUNTS, RATemp[zero], RATemp[full]);
    myNextion.setComponentText("v1", String(val) + " °C");

    val = map(inputs[RA_Humidity_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
    myNextion.setComponentText("l2", String(val) + " V");
    val = map(inputs[RA_Humidity_AI], 0, ADC_MAX_COUNTS, RAHum[zero], RAHum[full]);
    myNextion.setComponentText("v2", String(val) + " %");
    
    val = map(inputs[VFD_Speed_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
    myNextion.setComponentText("l3", String(val) + " V");
    val = map(inputs[VFD_Speed_AI], 0, ADC_MAX_COUNTS, VFD[zero], VFD[full]);
    myNextion.setComponentText("v3", String(val) + " Hz");
    
    val = map(inputs[Air_Velocity_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
    myNextion.setComponentText("l4", String(val) + " V");
    val = map(inputs[Air_Velocity_AI], 0, ADC_MAX_COUNTS, AVS[zero], AVS[full]);
    myNextion.setComponentText("v4", String(val) + " m/s");
    
    val = map(inputs[CO2_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
    myNextion.setComponentText("l5", String(val) + " V");
    val = map(inputs[CO2_AI], 0, ADC_MAX_COUNTS, CO2[zero], CO2[full]);
    myNextion.setComponentText("v5", String(val) + " PPM");
    
    myNextion.setComponentText("l6", String(inputs[SPARE_AI]));
    myNextion.setComponentText("l7", "");
    myNextion.setComponentText("l8", "");
  }

void updateIOScreen_4()
  {
    float val;
    val = map(outputs[Chilled_Water_AO], 0, DAC_MAX_COUNTS, 0, MAX_DC_OUTPUT_VOLTAGE);
    myNextion.setComponentText("l1", String(val));
    val = val * 10;
    myNextion.setComponentText("v1", String(val) + " % Opn");
    
    val = map(outputs[Hot_Water_AO], 0, DAC_MAX_COUNTS, 0, MAX_DC_OUTPUT_VOLTAGE);
    myNextion.setComponentText("l2", String(val));
    val = val * 10;
    myNextion.setComponentText("v2", String(val) + " % Opn");
    
    val = map(outputs[VFD_AO], 0, DAC_MAX_COUNTS, 0, MAX_DC_OUTPUT_VOLTAGE);
    myNextion.setComponentText("l3", String(val));
    val = map(inputs[VFD_Speed_AI], 0, ADC_MAX_COUNTS, VFD[zero], VFD[full]);
    myNextion.setComponentText("v3", String(val) + " Hz");
    
    myNextion.setComponentText("l4", "NA");
    myNextion.setComponentText("v4", "NA");

    if(AHUSetting[trhSens] == 1)
      {
        val = map(inputs[RA_Temp_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
        myNextion.setComponentText("l5", String(val) + " V");
        val = map(inputs[RA_Temp_AI], 0, ADC_MAX_COUNTS, RATemp[zero], RATemp[full]);
        myNextion.setComponentText("v5", String(val) + " °C");

        val = map(inputs[RA_Humidity_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
        myNextion.setComponentText("l6", String(val) + " V");
        val = map(inputs[RA_Humidity_AI], 0, ADC_MAX_COUNTS, RAHum[zero], RAHum[full]);
        myNextion.setComponentText("v6", String(val) + " %");
      }
    else
      {
        myNextion.setComponentText("l5", "NA");
        myNextion.setComponentText("v5", "NA");
        
        myNextion.setComponentText("l6", "NA");
        myNextion.setComponentText("v6", "NA");
      }
    
    myNextion.setComponentText("l7", "NA");
    myNextion.setComponentText("v7", "NA");
    
    myNextion.setComponentText("l8", "NA");
    myNextion.setComponentText("v8", "NA");
  }
