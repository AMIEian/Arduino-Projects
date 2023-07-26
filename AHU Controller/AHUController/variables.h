#define ADC_MAX_COUNTS 10000
#define DAC_MAX_COUNTS 10000

#define MAX_DC_INPUT_VOLTAGE 10
#define MAX_DC_OUTPUT_VOLTAGE 10

//Indexes for various readings in inputs array
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

//RO VALUES MASKS
#define AHU_RELAY_ON 1
#define AHU_RELAY_OFF 0xFFFE
#define FIRE_DAMPER_SUPPLY_ON 2
#define FIRE_DAMPER_SUPPLY_OFF 0xFFFD
#define FIRE_DAMPER_RETURN_ON 4
#define FIRE_DAMPER_RETURN_OFF 0xFFFB
#define FRESH_AIR_ON 8
#define FRESH_AIR_OFF 0xF7
#define BUZZER_ON 256
#define BUZZER_OFF 0xFEFF
#define AM_RELAY_ON 512
#define AM_RELAY_OFF 0xFDFF

//Indexes for various readings in outputs array
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

#define CO2_AI_Damp_Time 18
#define CO2_Lower 19
#define CO2_Higher 20

#define SPARE_AI_Damp_Time 21
#define SPARE_AI_Lower 22
#define SPARE_AI_Higher 23

#define AO_INDEX 5

//Indexes for various readings in fullVals array
#define AI_FULLVALS_INDEX 0
#define DAC_FULLVALS_INDEX 13

//Warnings
#define HEPA_NEAR_CLOGGING 1
#define HEPA_CLOGGED 2
#define FINE_NEAR_CLOGGING 3
#define FINE_CLOGGED 4
#define AHU_TRIPPED 5
#define AHU_FAILED_DURING_RUNNING 6
#define FIRE_DAMPER_SUPPLY_CLOSED 7
#define FIRE_DAMPER_RETURN_CLOSED 8
#define FIRE_ALARM 9
#define FIRE_DAMPER_SUPPLY_OPEN_FAILED 10
#define FIRE_DAMPER_SUPPLY_CLOSE_FAILED 11
#define FIRE_DAMPER_RETURN_OPEN_FAILED 12
#define FIRE_DAMPER_RETURN_CLOSE_FAILED 13
#define FILTER1_CLOGGED 14
#define FILTER2_CLOGGED 15
#define AHU_DOOR_OPEN 16
#define CO2_LEVEL_HIGH 17
#define AHU_FAIL_TO_START 18

//AHU Status
#define AHU_OFF 0
#define AHU_ON 1
#define AHU_COUNTDOWN 2
#define AHU_INIT 3
#define AHU_TURNING_ON 4
#define AHU_TURNING_OFF 5

//SCADA ARRAY Indexes
#define SCADA_ACK 0
#define SCADA_AHU_STATUS 1
#define SCADA_WARNING 2
#define SCADA_WARNING_COUNTS 3

//AHUDateTime Array Indexes
#define DATE 0
#define TIME 1

//AHUDate Array Indexes
#define DAY 0
#define MONTH 1
#define YEAR 2

//AHUTime Array Indexes
#define HOURS 0
#define MINUTES 1

//Holidays Array Indexes
#define WEEKLY_OFF 0
#define SEC_WEEKLY_OFF 1
#define ALT_WEEKLY_OFF 2

//Schedule Array Indexes
#define ON_TIME_1 0
#define OFF_TIME_1 1
#define ON_TIME_2 2
#define OFF_TIME_2 3
#define ON_TIME_3 4
#define OFF_TIME_3 5
#define ON_TIME_4 6
#define OFF_TIME_4 7

//Passwords indexes
#define userPin 0
#define factoryPin 1
#define maintainPin 2

//User Settings indexes
#define setTemp 0
#define setHum 1
#define setVFD 2
//AHU Settings indexes
#define ahuMode 0
#define ahuStatus 1
#define vfdSpeed 2
#define fineWarn 3
#define fineClog 4
#define hepaWarn 5
#define hepaClog 6
#define co2Low 7
#define co2High 8
#define co2Alarm 9
#define internalPara 10
#define trhSens 11
#define fineDpt 12
#define hepaDpt 13
#define rhCtrl 14
#define vfdSpeedCtrl 15
#define freshAirDampCtrl 16
#define fireDampSupCtrl 17
#define fireDampRetCtrl 18
#define fireAlarmIn 19

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
#define ramp 5
#define lower 6
#define upper 7
