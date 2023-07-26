// TTGO T-Call pins
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22
// BME280 pins
#define I2C_SDA_2            18
#define I2C_SCL_2            19

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

#include <Wire.h>

// I2C for SIM800 (to keep it running when powered from battery)
TwoWire I2CPower = TwoWire(0);

bool setPowerBoostKeepOn(int en)
  {
    I2CPower.beginTransmission(IP5306_ADDR);
    I2CPower.write(IP5306_REG_SYS_CTL0);
    if (en) 
      I2CPower.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
    else 
      I2CPower.write(0x35); // 0x37 is default reg value
    return I2CPower.endTransmission() == 0;
  }

void modemInit()
  {
    // Start I2C communication
    I2CPower.begin(I2C_SDA, I2C_SCL, 400000);
  
    // Keep power when running from battery
    bool isOk = setPowerBoostKeepOn(1);
    SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));
  
    // Set modem reset, enable, power pins
    pinMode(MODEM_PWKEY, OUTPUT);
    pinMode(MODEM_RST, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);
    digitalWrite(MODEM_PWKEY, LOW);
    digitalWrite(MODEM_RST, HIGH);
    digitalWrite(MODEM_POWER_ON, HIGH);
  
    // Set GSM module baud rate and UART pins
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(3000);
  
    // Restart SIM800 module, it takes quite some time
    SerialMon.println("Initializing modem...");
    modem.restart();
    // use modem.init() if you don't need the complete restart
    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem: ");
    SerialMon.println(modemInfo);
  
    SerialMon.print("Waiting for network...");
    if(!modem.waitForNetwork(240000L))
      {
        SerialMon.println(" fail");
        delay(10000);
        return;
      }
    SerialMon.println(" OK");
  
    if(modem.isNetworkConnected()) 
      SerialMon.println("Network connected");
    
    delay(1000);
  }
