#define ADC_MAX_COUNTS 10000
#define DAC_MAX_COUNTS 10000

//Indexes for various readings in counts array
#define RA_Temp_AI 0
#define RA_Humidity_AI 1
#define VFD_Speed_AI 2
#define Air_Velocity_AI 3
#define CO2_AI 4
#define SPARE_AI 5

#define AHU_On_Off 6
#define AHU_Trip 7
#define Filter_1 8
#define Filter_2 9
#define Fire_Input 10
#define Fire_Supply 11
#define Fire_Return 12
#define AHU_Door 13
#define SA_Temp 14
#define SA_Humidity 15
#define Fine_Filter 16
#define HEPA_Filter 17

#define AI_INDEX 0
#define DI_INDEX 6

#define RXD2 16
#define TXD2 17

//Indexes for various readings in output_counts array
#define RO_INDEX 0  //{0000 00 [A/M Relay] [BUZZER] [SPARE] [SPARE] [SPARE] [SPARE] [Fresh Air] [Fire Damper Return] [Fire Damper Supply] [AHU ON/OFF]}

#define RA_Temp_AI_Damp_Time 1
#define RA_Temp_Lower 2
#define RA_Temp_Higher 3

#define Chilled_Water_AO 5
#define Hot_Water_AO 6
#define VFD_AO 7
#define SPARE_AO 8

#define RA_Humidity_AI_Damp_Time 9
#define RA_Humidity_Lower 10
#define RA_Humidity_Higher 11

#define VFD_AI_Damp_Time 12
#define VFD_Lower 13
#define VFD_Higher 14

#define Air_Velocity_AI_Damp_Time 15
#define Air_Velocity_Lower 16
#define Air_Velocity_Higher 17

#define CO2_AI_Dampening_Time 18
#define CO2_Lower 19
#define CO2_Higher 20

#define SPARE_AI_Damp_Time 21
#define SPARE_AI_Lower 22
#define SPARE_AI_Higher 23

#define AO_INDEX 5

//Indexes for various readings in fullVals array
#define AI_FULLVALS_INDEX 0
#define DAC_FULLVALS_INDEX 13

//Analog inputs variables indexes
#define enabled 0
#define dt 1
#define zero 2
#define full 3

//PID variables indexes
#define pidMode 0
#define kp 1
#define ki 2
#define kd 3
#define kt 4

//String constants for nextion display
const String set_temp = "userSettings.set_temp";

//Analog inputs strings
const String RATempEnabled = "mtnSettings.ai1";
const String RATempDt = "mtnSettings.ai1_dt";
const String RATempZero = "mtnSettings.ai1_zero";
const String RATempFull = "mtnSettings.ai1_full";

//Analog inputs PID strings
const String RATempPIDMode = "mtnSettings.ai1_pid_mode";
const String RATempPIDKp = "mtnSettings.ai1_p";
const String RATempPIDKi = "mtnSettings.ai1_i";
const String RATempPIDKd = "mtnSettings.ai1_d";
const String RATempPIDKt = "mtnSettings.ai1_t";
