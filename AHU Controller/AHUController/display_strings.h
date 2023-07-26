//String constants for nextion display

//Home Strings
const String CurrentTemp = "home.current_temp";
const String CurrentHum = "home.current_rh";
const String SecCountDown = "home.sec";
const String WarningNo = "home.warning";

//Warning Strings
const String Reading = "warnings.reading";

//Date Time Strings
const String Date = "mtnSettings.date";
const String Time = "mtnSettings.time";
const String CurrentDate = "changeDateTime.curDate";
const String CurrentTime = "changeDateTime.curTime";

//Schedular Strings
const String SchedulerEnabled = "mtnSettings.scheduler";
const String Scheduls[7][8] = {
                                {"mtnSettings.sunOn1", "mtnSettings.sunOff1", "mtnSettings.sunOn2", "mtnSettings.sunOff2", "mtnSettings.sunOn3", "mtnSettings.sunOff3","mtnSettings.sunOn4", "mtnSettings.sunOff4"},
                                {"mtnSettings.monOn1", "mtnSettings.monOff1", "mtnSettings.monOn2", "mtnSettings.monOff2", "mtnSettings.monOn3", "mtnSettings.monOff3","mtnSettings.monOn4", "mtnSettings.monOff4"},
                                {"mtnSettings.tueOn1", "mtnSettings.tueOff1", "mtnSettings.tueOn2", "mtnSettings.tueOff2", "mtnSettings.tueOn3", "mtnSettings.tueOff3","mtnSettings.tueOn4", "mtnSettings.tueOff4"},
                                {"mtnSettings.wedOn1", "mtnSettings.wedOff1", "mtnSettings.wedOn2", "mtnSettings.wedOff2", "mtnSettings.wedOn3", "mtnSettings.wedOff3","mtnSettings.wedOn4", "mtnSettings.wedOff4"},
                                {"mtnSettings.thuOn1", "mtnSettings.thuOff1", "mtnSettings.thuOn2", "mtnSettings.thuOff2", "mtnSettings.thuOn3", "mtnSettings.thuOff3","mtnSettings.thuOn4", "mtnSettings.thuOff4"},
                                {"mtnSettings.friOn1", "mtnSettings.friOff1", "mtnSettings.friOn2", "mtnSettings.friOff2", "mtnSettings.friOn3", "mtnSettings.friOff3","mtnSettings.friOn4", "mtnSettings.friOff4"},
                                {"mtnSettings.satOn1", "mtnSettings.satOff1", "mtnSettings.satOn2", "mtnSettings.satOff2", "mtnSettings.satOn3", "mtnSettings.satOff3","mtnSettings.satOn4", "mtnSettings.satOff4"}
                              };

//Holidays Strings
const String WeeklyOff = "mtnSettings.weekly_off";
const String SecWeeklyOff = "mtnSettings.sec_off";
const String AltWeeklyOff = "mtnSettings.alt_off";

//AHU Settings
const String AHUMode = "home.mode";
const String AHUStatus = "home.ahu_status";
const String VFDSpeed = "mtnSettings.vfd_speed";
const String FineWarn = "mtnSettings.fine_warn";
const String FineClog = "mtnSettings.fine_clog";
const String HepaWarn = "mtnSettings.hepa_warn";
const String HepaClog = "mtnSettings.hepa_clog";
const String CO2Low = "mtnSettings.co2_low";
const String CO2High = "mtnSettings.co2_high";
const String CO2Alarm = "mtnSettings.co2_alarm";
const String InternalPara = "mtnSettings.int_sens";

//User Settings Strings
const String SetTemp = "userSettings.set_temp";
const String SetHum = "userSettings.set_humidity";

//Password Strings
const String UserPin = "password.user_pin";
const String FactoryPin = "password.factory_pin";
const String MaintainPin = "password.maintain_pin";

//Return Air Temperature Inputs Strings
const String RATempEnabled = "mtnSettings.ai1";
const String RATempDt = "mtnSettings.ai1_dt";
const String RATempZero = "mtnSettings.ai1_zero";
const String RATempFull = "mtnSettings.ai1_full";

//Return Air Temperature PID Strings
const String RATempPIDMode = "mtnSettings.ai1_pid_mode";
const String RATempPIDKp = "mtnSettings.ai1_p";
const String RATempPIDKi = "mtnSettings.ai1_i";
const String RATempPIDKd = "mtnSettings.ai1_d";
const String RATempPIDKt = "mtnSettings.ai1_t";
const String RATempPIDRamp = "mtnSettings.ai1_ramp";
const String RATempPIDLower = "mtnSettings.ai1_lower";
const String RATempPIDUpper = "mtnSettings.ai1_upper";

//Return Air Humidity Inputs Strings
const String RAHumEnabled = "mtnSettings.ai2";
const String RAHumDt = "mtnSettings.ai2_dt";
const String RAHumZero = "mtnSettings.ai2_zero";
const String RAHumFull = "mtnSettings.ai2_full";

//Return Air Humidity PID Strings
const String RAHumPIDMode = "mtnSettings.ai2_pid_mode";
const String RAHumPIDKp = "mtnSettings.ai2_p";
const String RAHumPIDKi = "mtnSettings.ai2_i";
const String RAHumPIDKd = "mtnSettings.ai2_d";
const String RAHumPIDKt = "mtnSettings.ai2_t";
const String RAHumPIDRamp = "mtnSettings.ai2_ramp";
const String RAHumPIDLower = "mtnSettings.ai2_lower";
const String RAHumPIDUpper = "mtnSettings.ai2_upper";

//VFD Inputs Strings
const String VFDEnabled = "mtnSettings.ai3";
const String VFDDt = "mtnSettings.ai3_dt";
const String VFDZero = "mtnSettings.ai3_zero";
const String VFDFull = "mtnSettings.ai3_full";

//VFD PID Strings
const String VFDPIDMode = "mtnSettings.ai3_pid_mode";
const String VFDPIDKp = "mtnSettings.ai3_p";
const String VFDPIDKi = "mtnSettings.ai3_i";
const String VFDPIDKd = "mtnSettings.ai3_d";
const String VFDPIDKt = "mtnSettings.ai3_t";
const String VFDPIDRamp = "mtnSettings.ai3_ramp";
const String VFDPIDLower = "mtnSettings.ai3_lower";
const String VFDPIDUpper = "mtnSettings.ai3_upper";

//Air Velocity Sensor Strings
const String AVSEnabled = "mtnSettings.ai4";
const String AVSDt = "mtnSettings.ai4_dt";
const String AVSZero = "mtnSettings.ai4_zero";
const String AVSFull = "mtnSettings.ai4_full";

//CO2 Strings
const String CO2Enabled = "mtnSettings.ai5";
const String CO2Dt = "mtnSettings.ai5_dt";
const String CO2Zero = "mtnSettings.ai5_zero";
const String CO2Full = "mtnSettings.ai5_full";
