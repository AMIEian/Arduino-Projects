int16_t oldInputs[8];

void initOldValues()
  {
    for(int i = 0; i < 8; i++)
      oldInputs[i] = -1;
  }
  
void updateIOScreen_1()
  {
    if(inputs[AHU_On_Off] != oldInputs[0])
      {
        myNextion.setComponentText("l1", String(inputs[AHU_On_Off]));
        if(inputs[AHU_On_Off] == 1)
          myNextion.setComponentText("v1", "On");
        else
          myNextion.setComponentText("v1", "Off");
        oldInputs[0] = inputs[AHU_On_Off];
      }

    if(inputs[AHU_Trip] != oldInputs[1])
      {
        myNextion.setComponentText("l2", String(inputs[AHU_Trip]));
        if(inputs[AHU_Trip] == 1)
          myNextion.setComponentText("v2", "Trip");
        else
          myNextion.setComponentText("v2", "Healthy");
        oldInputs[1] = inputs[AHU_Trip];
      }

    if(inputs[Filter_1] != oldInputs[2])
      {
        myNextion.setComponentText("l3", String(inputs[Filter_1]));
        if(inputs[Filter_1] == 1)
          myNextion.setComponentText("v3", "Clog");
        else
          myNextion.setComponentText("v3", "Healthy");
        oldInputs[2] = inputs[Filter_1];
      }

    if(inputs[Filter_2] != oldInputs[3])
      {
        myNextion.setComponentText("l4", String(inputs[Filter_2]));
        if(inputs[Filter_2] == 1)
          myNextion.setComponentText("v4", "Clog");
        else
          myNextion.setComponentText("v4", "Healthy");
        oldInputs[3] = inputs[Filter_2];
      }
      
    if(inputs[Fire_Input] != oldInputs[4])
      {
        myNextion.setComponentText("l5", String(inputs[Fire_Input]));
        if(inputs[Fire_Input] == 1)
          myNextion.setComponentText("v5", "Safe");
        else
          myNextion.setComponentText("v5", "Danger");
        oldInputs[4] = inputs[Fire_Input];
      }

    if(inputs[Fire_Supply] != oldInputs[5])
      {
        myNextion.setComponentText("l6", String(inputs[Fire_Supply]));
        if(inputs[Fire_Supply] == 1)
          myNextion.setComponentText("v6", "Open");
        else
          myNextion.setComponentText("v6", "Closed");
        oldInputs[5] = inputs[Fire_Supply];
      }

    if(inputs[Fire_Return] != oldInputs[6])
      {
        myNextion.setComponentText("l7", String(inputs[Fire_Return]));
        if(inputs[Fire_Return] == 1)
          myNextion.setComponentText("v7", "Open");
        else
          myNextion.setComponentText("v7", "Closed");
        oldInputs[6] = inputs[Fire_Return];
      }

    if(inputs[AHU_Door] != oldInputs[7])
      {
        myNextion.setComponentText("l8", String(inputs[AHU_Door]));
        if(inputs[AHU_Door] == 1)
          myNextion.setComponentText("v8", "Closed");
        else
          myNextion.setComponentText("v8", "Open");
        oldInputs[7] = inputs[AHU_Door];
      }
  }

void updateIOScreen_2()
  {
    int result;
    int mask = 1;
    
    result = outputs[RO_INDEX] & mask;
    if(result != oldInputs[0])
      {
        myNextion.setComponentText("l1", String(result));
        if(result == 1)
          myNextion.setComponentText("v1", "On");
        else
          myNextion.setComponentText("v1", "Off");
        oldInputs[0] = result;
      }
      
    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    if(result != oldInputs[1])
      {
        myNextion.setComponentText("l2", String(result));
        if(result == 1)
          myNextion.setComponentText("v2", "Open");
        else
          myNextion.setComponentText("v2", "Closed");
        oldInputs[1] = result;
      }

    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    if(result != oldInputs[2])
      {
        myNextion.setComponentText("l3", String(result));
        if(result == 1)
          myNextion.setComponentText("v3", "Open");
        else
          myNextion.setComponentText("v3", "Closed");
        oldInputs[2] = result;
      }

    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    if(result != oldInputs[3])
      {
        myNextion.setComponentText("l4", String(result));
        if(result == 1)
          myNextion.setComponentText("v4", "Open");
        else
          myNextion.setComponentText("v4", "Closed");
        oldInputs[3] = result;
      }

    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    if(result != oldInputs[4])
      {
        myNextion.setComponentText("l5", String(result));
        myNextion.setComponentText("v5", "NA");
        oldInputs[4] = result;
      }

    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    if(result != oldInputs[5])
      {
        myNextion.setComponentText("l6", String(result));
        myNextion.setComponentText("v6", "NA");
        oldInputs[5] = result;
      }

    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    if(result != oldInputs[6])
      {
        myNextion.setComponentText("l7", String(result));
        myNextion.setComponentText("v7", "NA");
        oldInputs[6] = result;
      }

    mask = mask << 1;
    result = outputs[RO_INDEX] & mask;
    if(result != oldInputs[7])
      {
        myNextion.setComponentText("l8", String(result));
        myNextion.setComponentText("v8", "NA");
        oldInputs[7] = result;
      }
  }

void updateIOScreen_3()
  {
    float val;
    if(inputs[RA_Temp_AI] != oldInputs[0])
      {
        val = myMap(inputs[RA_Temp_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
        myNextion.setComponentText("l1", String(val) + " V");
        val = myMap(inputs[RA_Temp_AI], 0, ADC_MAX_COUNTS, RATemp[zero], RATemp[full]);
        myNextion.setComponentText("v1", String(val) + " °C");
        oldInputs[0] = inputs[RA_Temp_AI];
      }

    if(inputs[RA_Humidity_AI] != oldInputs[1])
      {
        val = myMap(inputs[RA_Humidity_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
        myNextion.setComponentText("l2", String(val) + " V");
        val = myMap(inputs[RA_Humidity_AI], 0, ADC_MAX_COUNTS, RAHum[zero], RAHum[full]);
        myNextion.setComponentText("v2", String(val) + " %");
        oldInputs[1] = inputs[RA_Humidity_AI];
      }

    if(inputs[VFD_Speed_AI] != oldInputs[2])
      {
        val = myMap(inputs[VFD_Speed_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
        myNextion.setComponentText("l3", String(val) + " V");
        val = myMap(inputs[VFD_Speed_AI], 0, ADC_MAX_COUNTS, VFD[zero], VFD[full]);
        myNextion.setComponentText("v3", String(val) + " Hz");
        oldInputs[2] = inputs[VFD_Speed_AI];
      }

    if(inputs[Air_Velocity_AI] != oldInputs[3])
      {
        val = myMap(inputs[Air_Velocity_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
        myNextion.setComponentText("l4", String(val) + " V");
        val = myMap(inputs[Air_Velocity_AI], 0, ADC_MAX_COUNTS, AVS[zero], AVS[full]);
        myNextion.setComponentText("v4", String(val) + " m/s");
        oldInputs[3] = inputs[Air_Velocity_AI];
      }

    if(inputs[CO2_AI] != oldInputs[4])
      {
        val = myMap(inputs[CO2_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
        myNextion.setComponentText("l5", String(val) + " V");
        val = myMap(inputs[CO2_AI], 0, ADC_MAX_COUNTS, CO2[zero], CO2[full]);
        myNextion.setComponentText("v5", String(val) + " PPM");
        oldInputs[4] = inputs[CO2_AI];
      }

    if(inputs[SPARE_AI] != oldInputs[5])
      {
        myNextion.setComponentText("l6", String(inputs[SPARE_AI]));
        myNextion.setComponentText("v6", "NA");
        oldInputs[5] = inputs[SPARE_AI];
      }

    if(oldInputs[6] == -1)
      {
        myNextion.setComponentText("l7", " ");
        myNextion.setComponentText("v7", "NA");
        myNextion.setComponentText("l8", " ");
        myNextion.setComponentText("v8", "NA");
        oldInputs[6] = 1;
      }
  }

void updateIOScreen_4()
  {
    float val;
    if(outputs[Chilled_Water_AO] != oldInputs[0])
      {
        val = myMap(outputs[Chilled_Water_AO], 0, DAC_MAX_COUNTS, 0, MAX_DC_OUTPUT_VOLTAGE);
        myNextion.setComponentText("l1", String(val));
        val = val * 10;
        myNextion.setComponentText("v1", String(val) + " % Opn");
        oldInputs[0] = outputs[Chilled_Water_AO];
      }

    if(outputs[Hot_Water_AO] != oldInputs[1])
      {
        val = myMap(outputs[Hot_Water_AO], 0, DAC_MAX_COUNTS, 0, MAX_DC_OUTPUT_VOLTAGE);
        myNextion.setComponentText("l2", String(val));
        val = val * 10;
        myNextion.setComponentText("v2", String(val) + " % Opn");
        oldInputs[1] = outputs[Hot_Water_AO];
      }

    if(outputs[VFD_AO] != oldInputs[2])
      {
        val = myMap(outputs[VFD_AO], 0, DAC_MAX_COUNTS, 0, MAX_DC_OUTPUT_VOLTAGE);
        myNextion.setComponentText("l3", String(val));
        val = myMap(inputs[VFD_Speed_AI], 0, ADC_MAX_COUNTS, VFD[zero], VFD[full]);
        myNextion.setComponentText("v3", String(val) + " Hz");
        oldInputs[2] = outputs[VFD_AO];
      }

    if(oldInputs[3] == -1)
      {
        myNextion.setComponentText("l4", "NA");
        myNextion.setComponentText("v4", "NA");
        oldInputs[3] = 1;
      }

    if(AHUSetting[trhSens] == 1)
      {
        if(inputs[SA_Temp] != oldInputs[4])
          {
            myNextion.setComponentText("l5", " ");
            val = inputs[SA_Temp]/10;//myMap(inputs[RA_Temp_AI], 0, ADC_MAX_COUNTS, RATemp[zero], RATemp[full]);
            myNextion.setComponentText("v5", String(val) + " °C");
            oldInputs[4] = inputs[SA_Temp];
          }
        if(inputs[SA_Humidity] != oldInputs[5])
          {
            //val = myMap(inputs[RA_Humidity_AI], 0, ADC_MAX_COUNTS, 0, MAX_DC_INPUT_VOLTAGE);
            myNextion.setComponentText("l6", " ");
            val = inputs[SA_Humidity]/10;//myMap(inputs[RA_Humidity_AI], 0, ADC_MAX_COUNTS, RAHum[zero], RAHum[full]);
            myNextion.setComponentText("v6", String(val) + " %");
            oldInputs[5] = inputs[SA_Humidity];
          }
      }
    else
      {
        if(oldInputs[4] == -1)
          {
            myNextion.setComponentText("l5", "NA");
            myNextion.setComponentText("v5", "NA");
            
            myNextion.setComponentText("l6", "NA");
            myNextion.setComponentText("v6", "NA");
            oldInputs[4] = 1;
            oldInputs[5] = 1;
          }
      }

    if(AHUSetting[fineWarn] == 1)
      {
        if(inputs[Fine_Filter] != oldInputs[6])
          {
            myNextion.setComponentText("l7", " ");
            val = inputs[Fine_Filter]/10;//myMap(inputs[RA_Temp_AI], 0, ADC_MAX_COUNTS, RATemp[zero], RATemp[full]);
            myNextion.setComponentText("v7", String(val) + " P");
            oldInputs[6] = inputs[Fine_Filter];
          }
      }
    else
      {
        if(oldInputs[6] == -1)
          {
            myNextion.setComponentText("l7", "NA");
            myNextion.setComponentText("v7", "NA");
            oldInputs[6] = 1;
          }
      }

    if(AHUSetting[hepaWarn] == 1)
      {
        if(inputs[HEPA_Filter] != oldInputs[7])
          {
            myNextion.setComponentText("l8", " ");
            val = inputs[HEPA_Filter]/10;//myMap(inputs[RA_Temp_AI], 0, ADC_MAX_COUNTS, RATemp[zero], RATemp[full]);
            myNextion.setComponentText("v8", String(val) + " P");
            oldInputs[7] = inputs[HEPA_Filter];
          }
      }
    else
      {
        if(oldInputs[7] == -1)
          {
            myNextion.setComponentText("l8", "NA");
            myNextion.setComponentText("v8", "NA");
            oldInputs[7] = 1;
          }
      }
  }

float myMap(int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max) 
{
  return float((x - in_min) * (out_max - out_min)) / float((in_max - in_min) + out_min);
}
